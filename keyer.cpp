#include "keyer.h"
#include "oosmos.h"

#include "encoder.h"
#include "adcbtnph.h"
#include "memory.h"
#include "morse.h "

//>>>EVENTS
enum {
  ev_PlayMemory = 1,
  ev_SpeedChange = 2
};

#ifdef oosmos_DEBUG
  static const char * OOSMOS_EventNames(int EventCode)
  {
    switch (EventCode) {
      case ev_PlayMemory: return "ev_PlayMemory";
      case ev_SpeedChange: return "ev_SpeedChange";
      default: return "";
    }
  }
#endif
//<<<EVENTS

typedef union
{
  oosmos_sEvent Event;
  sAdcButtonEvent ButtonEvent;
} uEvents;

struct keyerTag
{
  //>>>DECL
  oosmos_sStateMachine(ROOT, uEvents, 1);
    oosmos_sOrtho KeyerActive_State;
      oosmos_sOrthoRegion KeyerActive_Region1_State;
        oosmos_sLeaf KeyerActive_Region1_DahSound_State;
        oosmos_sLeaf KeyerActive_Region1_Silent_State;
        oosmos_sLeaf KeyerActive_Region1_Choice1_State;
        oosmos_sLeaf KeyerActive_Region1_Choice2_State;
        oosmos_sLeaf KeyerActive_Region1_DitSound_State;
      oosmos_sOrthoRegion KeyerActive_Region2_State;
        oosmos_sLeaf KeyerActive_Region2_WaitForSpeedChange_State;
      oosmos_sOrthoRegion KeyerActive_Region3_State;
        oosmos_sLeaf KeyerActive_Region3_PlayingMemory_State;
        oosmos_sLeaf KeyerActive_Region3_WaitForMemory_State;
//<<<DECL

  pin *m_pDahPin;
  pin *m_pDitPin;
  pin *m_pKeyPin;
  pin *m_pLEDPin;

  encoder *m_pEncoder;

  pin *m_pEncoderAPin;
  pin *m_pEncoderBPin;

  adc *m_pButtonAdc;
  adcbtnph *m_pButton;

  uint8_t m_Memory;
  uint8_t *m_SelectedMemory;
  uint8_t m_CurrentCharacterInMemoryIndex;
  uint8_t m_CurrentCharacter;

  int16_t m_CPM;
  uint16_t mUnitTimeMS;
};

  // dit: 1 unit
  // dah: 3 units
  // intra character space (the gap between dits and dahs within a character): 1 unnit
  // inter character space (the gab between characters of a word): 3 units
  // word space (the gap between 2 words): 7 units


// Local Static functions

static bool IsDitPressed(const keyer *pKeyer)
{
  return pinIsOn(pKeyer->m_pDitPin);
}

static bool IsDahPressed(const keyer *pKeyer)
{
  return pinIsOn(pKeyer->m_pDahPin);
}

static void DitThread(const keyer *pKeyer, oosmos_sState *pState)
{
  oosmos_ThreadBegin();
  pinOn(pKeyer->m_pKeyPin);
  pinOn(pKeyer->m_pLEDPin);
  oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS);
  pinOff(pKeyer->m_pKeyPin);
  pinOff(pKeyer->m_pLEDPin);

  oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS);
  oosmos_ThreadEnd();
}

static void DahThread(const keyer *pKeyer, oosmos_sState *pState)
{
  oosmos_ThreadBegin();
  pinOn(pKeyer->m_pKeyPin);
  pinOn(pKeyer->m_pLEDPin);
  oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS * 3);
  pinOff(pKeyer->m_pKeyPin);
  pinOff(pKeyer->m_pLEDPin);

  oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS );
  oosmos_ThreadEnd();
}

static void CalculateElementLengths(keyer *pKeyer)
{
  // dit: 1 unit
  // dah: 3 units
  // intra character space (the gap between dits and dahs within a character): 1 unnit
  // inter character space (the gab between characters of a word): 3 units
  // word space (the gap between 2 words): 7 units
  pKeyer->m_CPM += encoderGetCount(pKeyer->m_pEncoder);
  oosmos_Min(pKeyer->m_CPM, 1);
  encoderReset(pKeyer->m_pEncoder);
  pKeyer->mUnitTimeMS = 6000 / pKeyer->m_CPM;
}

static void GetMemoryInfo(keyer *pKeyer)
{
  uEvents *pEvent = &pKeyer->m_CurrentEvent;
  //  oosmos_DebugPrint("Pressed %d %d %d\n", ((sAdcButtonEvent *) pEvent)->Event.m_Code, ((sAdcButtonEvent *) pEvent)->Event.m_pContext, ((sAdcButtonEvent *) pEvent)->Value);
  pKeyer->m_Memory = ((sAdcButtonEvent *)pEvent)->Value;
  pKeyer->m_CurrentCharacterInMemoryIndex = 0;
  if (pKeyer->m_Memory == 1)
    pKeyer->m_SelectedMemory = &Memory1[0];
  if (pKeyer->m_Memory == 2)
    pKeyer->m_SelectedMemory = &Memory2[0];
  if (pKeyer->m_Memory == 3)
    pKeyer->m_SelectedMemory = &Memory3[0];
  if (pKeyer->m_Memory == 4)
    pKeyer->m_SelectedMemory = &Memory4[0];
}

static void PlayingMemoryThread(keyer *pKeyer, oosmos_sState *pState)
{
  oosmos_ThreadBegin();
  for (;;)
  {
    pKeyer->m_CurrentCharacter = eeprom_read_byte((const uint8_t *)&pKeyer->m_SelectedMemory[pKeyer->m_CurrentCharacterInMemoryIndex]);

    oosmos_DebugPrint("%d\n", pKeyer->m_CurrentCharacter);

    if (pKeyer->m_CurrentCharacter == 0)
    {
      oosmos_ThreadExit();
    };

    if (pKeyer->m_CurrentCharacter == 0x20)
    {
      /* space */
      // had probably already a character "end", so we add up to 7
      oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS * 4);
    }
    else if (pKeyer->m_CurrentCharacter == 0x7C)
    {
      /* add delay */
      oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS);
    }
    else
    {
      /* convert to morse code */
      pKeyer->m_CurrentCharacter = code[pKeyer->m_CurrentCharacter - 0x20];
      for (;;)
      {
        /* only 1 left, symbol finished, break */
        if (pKeyer->m_CurrentCharacter == 1)
        {
          break;
        }
        if ((pKeyer->m_CurrentCharacter & 0x1) == 0)
        {
          pinOn(pKeyer->m_pKeyPin);
          pinOn(pKeyer->m_pLEDPin);
          oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS);
          pinOff(pKeyer->m_pKeyPin);
          pinOff(pKeyer->m_pLEDPin);
          oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS);
        }
        else
        {
          pinOn(pKeyer->m_pKeyPin);
          pinOn(pKeyer->m_pLEDPin);
          oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS * 3);
          pinOff(pKeyer->m_pKeyPin);
          pinOff(pKeyer->m_pLEDPin);
          oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS);
        }

        /* shift right */
        pKeyer->m_CurrentCharacter = pKeyer->m_CurrentCharacter >> 1;
      }
      oosmos_ThreadDelayMS(pKeyer->mUnitTimeMS * 3);
    }

    pKeyer->m_CurrentCharacterInMemoryIndex++;
  }
  oosmos_ThreadEnd();
}

//>>>CODE
static bool KeyerActive_Region1_DahSound_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      DahThread(pKeyer, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_Choice1_State);
    }
  }

  return false;
}

static bool KeyerActive_Region1_Silent_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      if (IsDahPressed(pKeyer)) {
        return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_DahSound_State);
      }
      if (IsDitPressed(pKeyer)) {
        return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_DitSound_State);
      }
      return true;
    }
  }

  return false;
}

static bool KeyerActive_Region1_Choice1_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  if (oosmos_EventCode(pEvent) == oosmos_ENTER) {
    if (IsDitPressed(pKeyer)) {
      return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_DitSound_State);
    }
    else {
      return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_Silent_State);
    }
  }

  return false;
}

static bool KeyerActive_Region1_Choice2_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  if (oosmos_EventCode(pEvent) == oosmos_ENTER) {
    if (IsDahPressed(pKeyer)) {
      return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_DahSound_State);
    }
    else {
      return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_Silent_State);
    }
  }

  return false;
}

static bool KeyerActive_Region1_DitSound_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      DitThread(pKeyer, pState);
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pKeyer, pState, KeyerActive_Region1_Choice2_State);
    }
  }

  return false;
}

static void OOSMOS_Action1(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  CalculateElementLengths(pKeyer);

  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static bool KeyerActive_Region2_WaitForSpeedChange_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_SpeedChange: {
      return oosmos_TransitionAction(pKeyer, pState, KeyerActive_Region2_WaitForSpeedChange_State, pEvent, OOSMOS_Action1);
    }
  }

  return false;
}

static bool KeyerActive_Region3_PlayingMemory_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case oosmos_POLL: {
      PlayingMemoryThread(pKeyer, pState);
      if (IsDitPressed(pKeyer) || IsDahPressed(pKeyer)) {
        return oosmos_Transition(pKeyer, pState, KeyerActive_Region3_WaitForMemory_State);
      }
      return true;
    }
    case oosmos_COMPLETE: {
      return oosmos_Transition(pKeyer, pState, KeyerActive_Region3_WaitForMemory_State);
    }
  }

  return false;
}

static void OOSMOS_Action2(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  GetMemoryInfo(pKeyer);

  oosmos_UNUSED(pState);
  oosmos_UNUSED(pEvent);
}

static bool KeyerActive_Region3_WaitForMemory_State_Code(void * pObject, oosmos_sState * pState, const oosmos_sEvent * pEvent)
{
  keyer * pKeyer = (keyer *) pObject;

  switch (oosmos_EventCode(pEvent)) {
    case ev_PlayMemory: {
      return oosmos_TransitionAction(pKeyer, pState, KeyerActive_Region3_PlayingMemory_State, pEvent, OOSMOS_Action2);
    }
  }

  return false;
}
//<<<CODE

extern keyer *keyerNew(pin *pDahPin, pin *pDitPin, pin *pKeyPin, pin *pLEDPin, pin *pEncoderAPin, pin *pEncoderBPin, adc *pButtonAdc, unsigned CPM)
{
  oosmos_Allocate(pKeyer, keyer, 1, NULL);

  //>>>INIT
  oosmos_StateMachineInit(pKeyer, ROOT, NULL, KeyerActive_State);
    oosmos_OrthoInit(pKeyer, KeyerActive_State, ROOT, NULL);
      oosmos_OrthoRegionInit(pKeyer, KeyerActive_Region1_State, KeyerActive_State, KeyerActive_Region1_Silent_State, NULL);
        oosmos_LeafInit(pKeyer, KeyerActive_Region1_DahSound_State, KeyerActive_Region1_State, KeyerActive_Region1_DahSound_State_Code);
        oosmos_LeafInit(pKeyer, KeyerActive_Region1_Silent_State, KeyerActive_Region1_State, KeyerActive_Region1_Silent_State_Code);
        oosmos_LeafInit(pKeyer, KeyerActive_Region1_Choice1_State, KeyerActive_Region1_State, KeyerActive_Region1_Choice1_State_Code);
        oosmos_LeafInit(pKeyer, KeyerActive_Region1_Choice2_State, KeyerActive_Region1_State, KeyerActive_Region1_Choice2_State_Code);
        oosmos_LeafInit(pKeyer, KeyerActive_Region1_DitSound_State, KeyerActive_Region1_State, KeyerActive_Region1_DitSound_State_Code);
      oosmos_OrthoRegionInit(pKeyer, KeyerActive_Region2_State, KeyerActive_State, KeyerActive_Region2_WaitForSpeedChange_State, NULL);
        oosmos_LeafInit(pKeyer, KeyerActive_Region2_WaitForSpeedChange_State, KeyerActive_Region2_State, KeyerActive_Region2_WaitForSpeedChange_State_Code);
      oosmos_OrthoRegionInit(pKeyer, KeyerActive_Region3_State, KeyerActive_State, KeyerActive_Region3_WaitForMemory_State, NULL);
        oosmos_LeafInit(pKeyer, KeyerActive_Region3_PlayingMemory_State, KeyerActive_Region3_State, KeyerActive_Region3_PlayingMemory_State_Code);
        oosmos_LeafInit(pKeyer, KeyerActive_Region3_WaitForMemory_State, KeyerActive_Region3_State, KeyerActive_Region3_WaitForMemory_State_Code);

  oosmos_Debug(pKeyer, OOSMOS_EventNames);
//<<<INIT

  pKeyer->m_pDahPin = pDahPin;
  pKeyer->m_pDitPin = pDitPin;
  pKeyer->m_pKeyPin = pKeyPin;
  pKeyer->m_pLEDPin = pLEDPin;

  pKeyer->m_pEncoder = encoderNew(pEncoderAPin, pEncoderBPin);

  pKeyer->m_pButtonAdc = pButtonAdc;
  pKeyer->m_pButton = adcbtnphNew(pKeyer->m_pButtonAdc, 500);

  pKeyer->m_CPM = CPM;

  oosmos_sQueue *const pKeyerEventQueue = &pKeyer->m_EventQueue;

  encoderSubscribeChangeEvent(pKeyer->m_pEncoder, pKeyerEventQueue, ev_SpeedChange, NULL);

  adcbtnphSubscribeButtonPress(pKeyer->m_pButton, pKeyerEventQueue, ev_PlayMemory, NULL);

  CalculateElementLengths(pKeyer);

  return pKeyer;
}

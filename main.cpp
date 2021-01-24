//
// OOSMOS - The Object-Oriented State Machine Operating System
//
// Copyright (C) 2014-2020  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "oosmos.h"
#include "pin.h"
#include "adc.h"
#include "keyer.h"
#include "prt.h"

extern void setup()
{
  pin * pDitPin = pinNew(10,  pinIn_Pullup,  pinActiveLow);
  pin * pDahPin = pinNew(9,  pinIn_Pullup,  pinActiveLow);
  pin * pKeyPin = pinNew(11, pinOut, pinActiveHigh);
  pin * pLEDPin = pinNew(13, pinOut, pinActiveHigh);

  pin * pEncoderAPin = pinNew(A1, pinIn_Pullup, pinActiveLow);
  pin * pEncoderBPin = pinNew(A0, pinIn_Pullup, pinActiveLow);

  adc * pButtonAdc = adcNew(A2);

  const int CPM = 150;

  keyerNew(pDahPin, pDitPin, pKeyPin, pLEDPin, pEncoderAPin, pEncoderBPin, pButtonAdc, CPM);
}

extern void loop()
{
  oosmos_RunStateMachines();
}

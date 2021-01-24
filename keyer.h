#ifndef KEYER_H
#define KEYER_H

#include "oosmos.h"
#include "pin.h"
#include "adc.h"
#include <stdint.h>

typedef struct keyerTag keyer;

extern keyer * keyerNew(pin * pDahPin, pin * pDitPin, pin * pKeyPin, pin * pLEDPin, pin * pEncoderAPin, pin * pEncoderBPin, adc * pButtonAdc, unsigned CPM);

#endif

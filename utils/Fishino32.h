/*
   Copyright 2017 Alessandro Pasqualini
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
     http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   @author    Alessandro Pasqualini <alessandro.pasqualini.1105@gmail.com>
   @url       https://github.com/alessandro1105

   This project has been developed with the contribution of Futura Elettronica.
   - http://www.futurashop.it
   - http://www.elettronicain.it
   - https://www.open-electronics.org
*/
#ifndef D7S_FISHINO32_H
#define D7S_FISHINO32_H

#include <Arduino.h>
#include <DTWI.h>
#include <Wire.h>

// DTWI constants
#define __DTWI0_BASE    _I2C1_BASE_ADDRESS
#define __DTWI0_BUS_IRQ _I2C1_BUS_IRQ
#define __DTWI0_VECTOR  _I2C_1_VECTOR
#define __DTWI0_IPL_ISR IPL3SOFT
#define __DTWI0_IPL     5 // Interrupt priority: must be at least 5
#define __DTWI0_SPL     0
#define __DTWI0_SCL_PIN  22
#define __DTWI0_SDA_PIN  21

// User defined DTWI class
class _DTWI0 : public DTWI {

    // needed to get to pDTWI
    friend void __attribute__((nomips16)) IntDtwi0Handler(void);

private:
    static _DTWI0 * p_DTWI0;

public:
    _DTWI0();
};

extern TwoWire &___wire();
#define _Wire ___wire()

// The Arduino IDE seems to ignore this file, so we need to include it manually
#include "Fishino32.cpp"

#endif
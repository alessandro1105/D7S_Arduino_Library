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

#include <utils/Fishino32.h>

_DTWI0 * _DTWI0::p_DTWI0 = NULL;

// the class methods
_DTWI0::_DTWI0() : DTWI(((p32_i2c *) __DTWI0_BASE), __DTWI0_BUS_IRQ, __DTWI0_VECTOR, __DTWI0_IPL, __DTWI0_SPL, __DTWI0_SCL_PIN, __DTWI0_SDA_PIN) {
    // save the instance away so the interrupt routine can
    // get back to this instance
    p_DTWI0 = this;

    // set the interrupt vector
    setIntVector(__DTWI0_VECTOR, IntDtwi0Handler);
};

// the associated interrupt routine.
#if defined(__PIC32MZXX__)
void __attribute__((nomips16,at_vector(_DTWI0_VECTOR),interrupt(_DTWI0_IPL_ISR))) IntDtwi0Handler(void)
#else
void __attribute__((interrupt(),nomips16)) IntDtwi0Handler(void)
#endif
{
    _DTWI0::p_DTWI0->stateMachine();
}

TwoWire &___wire() {
   static _DTWI0 d;
   static TwoWire tw(d);
   return tw;
}

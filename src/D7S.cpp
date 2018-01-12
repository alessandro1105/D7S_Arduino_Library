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

#include "D7S.h"

//----------------------- PUBLIC INTERFACE -----------------------

//--- CONSTRUCTOR/DESTROYER ---
D7SClass::D7SClass() {
   //reset handler array
   for (int i = 0; i < 4; i++) {
      _handlers[i] = NULL;
   }

   //reset events variable
   _events = 0;

}

//--- BEGIN ---
//used to initialize Wire
void D7SClass::begin() {
   //begin Wire
   WireD7S.begin();
}

//--- STATUS ---
//return the currect state
d7s_status D7SClass::getState() {
   //read the STATE register at 0x1000
   return (d7s_status) (read8bit(0x10, 0x00) & 0x07);
}

//return the currect state
d7s_axis_state D7SClass::getAxisInUse() {
   //read the AXIS_STATE register at 0x1001
   return (d7s_axis_state) (read8bit(0x10, 0x01) & 0x03);
}

//--- SETTINGS ---
//change the threshold in use
void D7SClass::setThreshold(d7s_threshold threshold) {
   //check if threshold is valid
   if (threshold < 0 || threshold > 1) {
      return;
   }
   //read the CTRL register at 0x1004
   uint8_t reg = read8bit(0x10, 0x04);
   //new register value with the threshold
   reg = (((reg >> 4) << 1) | (threshold & 0x01)) << 3;
   //update register
   write8bit(0x10, 0x04, reg);
}

//change the axis selection mode
void D7SClass::setAxis(d7s_axis_settings axisMode) {
   //check if axisMode is valid
   if (axisMode < 0 or axisMode > 4) {
      return;
   }
   //read the CTRL register at 0x1004
   uint8_t reg = read8bit(0x10, 0x04);
   //new register value with the threshold
   reg = (axisMode << 4) | (reg & 0x0F);
   //update register
   write8bit(0x10, 0x04, reg);
}

//--- LASTEST DATA ---
//get the lastest SI at specified index (up to 5) [m/s]
float D7SClass::getLastestSI(uint8_t index) {
   //check if the index is in bound
   if (index < 0 || index > 4) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + index, 0x08)) / 1000;
}

//get the lastest PGA at specified index (up to 5) [m/s^2]
float D7SClass::getLastestPGA(uint8_t index) {
   //check if the index is in bound
   if (index < 0 || index > 4) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + index, 0x0A)) / 1000;
}

//get the lastest Temperature at specified index (up to 5) [Celsius]
float D7SClass::getLastestTemperature(uint8_t index) {
   //check if the index is in bound
   if (index < 0 || index > 4) {
      return 0;
   }
   //return the value
   return (float) ((int16_t) read16bit(0x30 + index, 0x06)) / 10;
}

//--- RANKED DATA ---
//get the ranked SI at specified position (up to 5) [m/s]
float D7SClass::getRankedSI(uint8_t position) {
   //check if the position is in bound
   if (position < 0 || position > 4) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + position +5, 0x08)) / 1000;
}

//get the ranked PGA at specified position (up to 5) [m/s^2]
float D7SClass::getRankedPGA(uint8_t position) {
   //check if the position is in bound
   if (position < 0 || position > 4) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + position +5, 0x0A)) / 1000;
}

//get the ranked Temperature at specified position (up to 5) [Celsius]
float D7SClass::getRankedTemperature(uint8_t position) {
   //check if the position is in bound
   if (position < 0 || position > 4) {
      return 0;
   }
   //return the value
   return (float) ((int16_t) read16bit(0x30 + position +5, 0x06)) / 10;
}

//--- INSTANTANEUS DATA ---
//get instantaneus SI (during an earthquake) [m/s]
float D7SClass::getInstantaneusSI() {
   //return the value
   return ((float) read16bit(0x20, 0x00)) / 1000;
}

//get instantaneus PGA (during an earthquake) [m/s^2]
float D7SClass::getInstantaneusPGA() {
   //return the value
   return ((float) read16bit(0x20, 0x02)) / 1000;
}

//--- CLEAR MEMORY ---
//delete both the lastest data and the ranked data
void D7SClass::clearEarthquakeData() {
   //write clear command
   write8bit(0x10, 0x05, 0x01);
}

//delete initializzazion data
void D7SClass::clearInstallationData() {
   //write clear command
   write8bit(0x10, 0x05, 0x08);
}

//delete offset data
void D7SClass::clearLastestOffsetData() {
   //write clear command
   write8bit(0x10, 0x05, 0x04);
}

//delete selftest data
void D7SClass::clearSelftestData() {
   //write clear command
   write8bit(0x10, 0x05, 0x02);
}

//delete all data
void D7SClass::clearAllData() {
   //write clear command
   write8bit(0x10, 0x05, 0x0F);
}

//--- INITIALIZATION ---
//initialize the d7s (start the initial installation mode)
void D7SClass::initialize() {
   //write INITIAL INSTALLATION MODE command
   write8bit(0x10, 0x03, 0x02);
}

//--- SELFTEST ---
//start autodiagnostic and resturn the result (OK/ERROR)
void D7SClass::selftest() {
   //write SELFTEST command
   write8bit(0x10, 0x03, 0x04);
}

//return the result of self-diagnostic test (OK/ERROR)
d7s_mode_status D7SClass::getSelftestResult() {
   //return result of the selftest
   return (d7s_mode_status) ((read8bit(0x10, 0x02) & 0x07) >> 2);
}

//--- OFFSET ACQUISITION ---
//start offset acquisition and return the rersult (OK/ERROR)
void D7SClass::acquireOffset() {
   //write OFFSET ACQUISITION MODE command
   write8bit(0x10, 0x03, 0x03);
}

//return the result of offset acquisition test (OK/ERROR)
d7s_mode_status D7SClass::getAcquireOffsetResult() {
   //return result of the offset acquisition
   return (d7s_mode_status) ((read8bit(0x10, 0x02) & 0x0F) >> 3);
}

//--- SHUTOFF/COLLAPSE EVENT ---
//after each earthquakes it's important to reset the events calling resetEvents() to prevent polluting the new data with the old one
//return true if the collapse condition is met (it's the sencond bit of _events)
uint8_t D7SClass::isInCollapse() {
   //updating the _events variable
   readEvents();
   //return the second bit of _events
   return (_events & 0x02) >> 1;
}

//return true if the shutoff condition is met (it's the first bit of _events)
uint8_t D7SClass::isInShutoff() {
   //updating the _events variable
   readEvents();
   //return the second bit of _events
   return _events & 0x01;
}

//reset shutoff/collapse events
void D7SClass::resetEvents() {
   //reset the EVENT register (read to zero-ing it)
   read8bit(0x10, 0x02);
   //reset the events variable
   _events = 0;
}

//--- EARTHQUAKE EVENT ---
//return true if an earthquake is occuring
uint8_t D7SClass::isEarthquakeOccuring() {
   //if D7S is in NORMAL MODE NOT IN STANBY (after the first 4 sec to initial delay) there is an earthquake
   return getState() == NORMAL_MODE_NOT_IN_STANBY;
}

//--- READY STATE ---
uint8_t D7SClass::isReady() {
   return getState() == NORMAL_MODE;
}

//--- INTERRUPT ---
//enable interrupt INT1 on specified pin
void D7SClass::enableInterruptINT1(uint8_t pin) {
   //enable pull up resistor
   pinMode(pin, INPUT_PULLUP);
   //attach interrupt
   attachInterrupt(digitalPinToInterrupt(pin), isr1, FALLING);
}

//enable interrupt INT2 on specified pin
void D7SClass::enableInterruptINT2(uint8_t pin) {
   //enable pull up resistor
   pinMode(pin, INPUT_PULLUP);
   // Fishino32 cannot handle CHANGE mode on interrupts, so we need to register FALLING mode first and on the isr register
   // as RISING the same pin detaching the previus interrupt
   #if defined(_FISHINO_PIC32_) || defined(_FISHINO32_) || defined(_FISHINO32_120_) || defined(_FISHINO32_MX470F512H_) || defined(_FISHINO32_MX470F512H_120_)
      pinINT2 = pin;
      //attach interrupt
      attachInterrupt(digitalPinToInterrupt(pin), isr2, FALLING);
   #else
      //attach interrupt
      attachInterrupt(digitalPinToInterrupt(pin), isr2, CHANGE);
   #endif
}

//start interrupt handling
void D7SClass::startInterruptHandling() {
   //enabling interrupt handling
   _interruptEnabled = 1;
}

//stop interrupt handling
void D7SClass::stopInterruptHandling() {
   //disabling interrupt handling
   _interruptEnabled = 0;
}

//assing the handler to the specific event
void D7SClass::registerInterruptEventHandler(d7s_interrupt_event event, void (*handler) ()) {
   //check if event is in bound (it's the index to the handlers array)
   if (event < 0 || event > 3) {
      return;
   }
   //copy the pointer to the array
   _handlers[event] = handler;
}

void D7SClass::registerInterruptEventHandler(d7s_interrupt_event event, void (*handler) (float, float, float)) {
  registerInterruptEventHandler(event, (void (*)()) handler);
}


//----------------------- PRIVATE INTERFACE -----------------------

//--- READ ---
//read 8 bit from the specified register
uint8_t D7SClass::read8bit(uint8_t regH, uint8_t regL) {

   //DEBUG
   #ifdef DEBUG
      Serial.println("--- read8bit ---");
      Serial.print("REG: 0x");
      Serial.print(regH, HEX);
      Serial.println(regL, HEX);
   #endif

   //setting up i2c connection
   WireD7S.beginTransmission(D7S_ADDRESS);
   
   //write register address
   WireD7S.write(regH); //register address high
   delay(10); //delay to prevent freezing
   WireD7S.write(regL); //register address low
   delay(10); //delay to prevent freezing
   
   //send RE-START message
   uint8_t status = WireD7S.endTransmission(false);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[RE-START]: ");
      //send RE-START message
      Serial.println(status);
   #endif

   //if the status != 0 there is an error
   if (status != 0) {
      //retry
      return read8bit(regH, regL);
   }

   //request 1 byte
   WireD7S.requestFrom(D7S_ADDRESS, 1);
   //wait until the data is received
   while (WireD7S.available() < 1)
      ;
   //read the data
   uint8_t data = WireD7S.read();

   //DEBUG
   #ifdef DEBUG
      Serial.println("--- read8bit ---");
   #endif
  
   //return the data
   return data;
}

//read 16 bit from the specified register
uint16_t D7SClass::read16bit(uint8_t regH, uint8_t regL) {

   //DEBUG
   #ifdef DEBUG
      Serial.println("--- read16bit ---");
      Serial.print("REG: 0x");
      Serial.print(regH, HEX);
      Serial.println(regL, HEX);
   #endif

   //setting up i2c connection
   WireD7S.beginTransmission(D7S_ADDRESS);
   
   //write register address
   WireD7S.write(regH); //register address high
   delay(10); //delay to prevent freezing
   WireD7S.write(regL); //register address low
   delay(10); //delay to prevent freezing
   
   //send RE-START message
   uint8_t status = WireD7S.endTransmission(false);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[RE-START]: ");
      //send RE-START message
      Serial.println(status);
   #endif

   //if the status != 0 there is an error
   if (status != 0) {
      //retry again
      return read16bit(regH, regL);
   }

   //request 2 byte
   WireD7S.requestFrom(D7S_ADDRESS, 2);
   //wait until the data is received
   while (WireD7S.available() < 2)
      ;
   //read the data
   uint8_t msb = WireD7S.read();
   uint8_t lsb = WireD7S.read();

   //DEBUG
   #ifdef DEBUG
      Serial.println("--- read16bit ---");
   #endif

   //return the data
   return (msb << 8) | lsb;
}

//--- WRITE ---
//write 8 bit to the register specified
void D7SClass::write8bit(uint8_t regH, uint8_t regL, uint8_t val) {
   //DEBUG
   #ifdef DEBUG
      Serial.println("--- write8bit ---");
   #endif

   //setting up i2c connection
   WireD7S.beginTransmission(D7S_ADDRESS);
   
   //write register address
   WireD7S.write(regH); //register address high
   delay(10); //delay to prevent freezing
   WireD7S.write(regL); //register address low
   delay(10); //delay to prevent freezing
   
   //write data
   WireD7S.write(val);
   delay(10); //delay to prevent freezing
   //closing the connection (STOP message)
   uint8_t status = WireD7S.endTransmission(true);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[STOP]: ");
      //closing the connection (STOP message)
      Serial.println(status);
      Serial.println("--- write8bit ---");
   #endif
}

//--- READ EVENTS ---
//read the event (SHUTOFF/COLLAPSE) from the EVENT register
void D7SClass::readEvents() {
   //read the EVENT register at 0x1002 and obtaining only the first two bits
   uint8_t events = read8bit(0x10, 0x02) & 0x03;
   //updating the _events variable
   _events |= events;
}

//--- INTERRUPT HANDLER ---
//handle the INT1 events
void D7SClass::int1() {
   //enabling interrupts
   interrupts();
   //if the interrupt handling is enabled
   if (_interruptEnabled) {
      //check what event triggered the interrupt
      if (isInShutoff()) {
         //if the handler is defined
         if (_handlers[2]) {
            _handlers[2](); //SHUTOFF_EVENT EVENT
         }
      } else {
         //if the handler is defined
         if (_handlers[3]) {
            _handlers[3](); //COLLAPSE_EVENT EVENT
         }
      }
   }
}

//handle the INT2 events
void D7SClass::int2() {
   //enabling interrupts
   interrupts();
   //if the interrupt handling is enabled
   if (_interruptEnabled) {
      //check what in what state the D7S is
      if (isEarthquakeOccuring()) { //earthquake started
         // Fishino32 cannot handle CHANGE mode on interrupts, so we need to register FALLING mode first and on the isr register
         // as RISING the same pin detaching the previus interrupt
         #if defined(_FISHINO_PIC32_) || defined(_FISHINO32_) || defined(_FISHINO32_120_) || defined(_FISHINO32_MX470F512H_) || defined(_FISHINO32_MX470F512H_120_)
            // Detaching the previus interrupt as FALLING
            detachInterrupt(digitalPinToInterrupt(pinINT2));
            // Attaching the same interrupt as RISING
            attachInterrupt(digitalPinToInterrupt(pinINT2), isr2, RISING);
         #endif
         //if the handler is defined
         if (_handlers[0]) {
            _handlers[0](); //START_EARTHQUAKE EVENT
         }
      } else { //earthquake ended
         // Fishino32 cannot handle CHANGE mode on interrupts, so we need to register FALLING mode first and on the isr register
         // as RISING the same pin detaching the previus interrupt
         #if defined(_FISHINO_PIC32_) || defined(_FISHINO32_) || defined(_FISHINO32_120_) || defined(_FISHINO32_MX470F512H_) || defined(_FISHINO32_MX470F512H_120_)
            // Detaching the previus interrupt as FALLING
            detachInterrupt(digitalPinToInterrupt(pinINT2));
            // Attaching the same interrupt as RISING
            attachInterrupt(digitalPinToInterrupt(pinINT2), isr2, FALLING);
         #endif
         //if the handler is defined
         if (_handlers[1]) {
            ((void (*)(float, float, float)) _handlers[1])(getLastestSI(0), getLastestPGA(0), getLastestTemperature(0)); //END_EARTHQUAKE EVENT
         }
      }
   }
}

//--- ISR HANDLER ---
//it handle the FALLING event that occur to the INT1 D7S pin (glue routine)
void D7SClass::isr1() {
   D7S.int1();
}

//it handle the CHANGE event thant occur to the INT2 D7S pin (glue routine)
void D7SClass::isr2() {
   D7S.int2();
}

//extern object
D7SClass D7S;
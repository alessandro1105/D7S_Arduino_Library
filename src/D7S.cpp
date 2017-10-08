/*
   Copyright 2016 Alessandro Pasqualini
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
     http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
   @author         Alessandro Pasqualini <alessandro.pasqualini.1105@gmail.com>
   @url            https://github.com/alessandro1105
*/

#include <Arduino.h>
#include <D7S.h>
#include <Wire.h>

//----------------------- PUBLIC INTERFACE -----------------------

//--- CONSTRUCTOR/DESTROYER ---
D7SClass::D7SClass() {
   //reset handler array
   for (int i = 0; i < 4; i++) {
      _handlers[i] = NULL;
   }

   //reset eventTriggered
   _eventTriggered = 0;

   //DEBUG
   #ifdef DEBUG
      Serial.begin(9600);
   #endif
}

//--- BEGIN ---
//used to initialize Wire
void D7SClass::begin() {
   //begin Wire
   Wire.begin();
   //wait while the D7S is busy
   while (getState() != NORMAL_MODE) {
      delay(D7S_DELAY_IN_PROCESSING);
   }
}

//--- STATUS ---
//return the currect state
d7s_status D7SClass::getState() {
   //read the STATE register at 0x1000
   return read8bit(0x10, 0x00) & 0x6;
}

//return the currect state
d7s_axis_state D7SClass::getAxisInUse() {
   //read the AXIS_STATE register at 0x1001
   return read8bit(0x10, 0x01) & 0x3;
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
   //set that the event is triggered (no interrupt)
   _eventTriggered = 1;
   //write INITIAL INSTALLATION MODE command
   write8bit(0x10, 0x03, 0x02);
   //white until intial installation ends
   while (getState() != NORMAL_MODE) {
      delay(D7S_DELAY_IN_PROCESSING);
   }
   _eventTriggered = 0;
}

//--- SELFTEST ---
//start autodiagnostic and resturn the result (OK/ERROR)
d7s_mode_status D7SClass::selftest() {
   //set that the event is triggered (no interrupt)
   _eventTriggered = 1;
   //write SELFTEST command
   write8bit(0x10, 0x03, 0x04);
   //white until selftest ends
   while (getState() != NORMAL_MODE) {
      delay(D7S_DELAY_IN_PROCESSING);
   }
   //set that the event triggered is ended (no interrupt)
   _eventTriggered = 0;
   //return result of the selftest
   return (read8bit(0x10, 0x02) & 0x07) >> 2;
}

//--- OFFSET ACQUISITION ---
//start offset acquisition and return the rersult (OK/ERROR)
d7s_mode_status D7SClass::acquireOffset() {
   //set that the event is triggered (no interrupt)
   _eventTriggered = 1;
   //write OFFSET ACQUISITION MODE command
   write8bit(0x10, 0x03, 0x03);
   //white until offset acquisition ends
   while (getState() != NORMAL_MODE) {
      delay(D7S_DELAY_IN_PROCESSING);
   }
   //set that the event triggered is ended (no interrupt)
   _eventTriggered = 0;
   //return result of the selftest
   return (read8bit(0x10, 0x02) & 0x0F) >> 3;
}

//--- SHUTOFF/COLLAPSE EVENT ---
//return the status of the shutoff/collapse condition (NONE/SHUTOFF/COLLAPSE)
d7s_event_status D7SClass::getEvent() {
   //read the EVENT register at 0x1002
   uint8_t reg = read8bit(0x10, 0x02) & 0x03;
   //if reg == 0x00 => NONE
   if (reg == NONE) {
      return NONE;
   //if reg == 0x02 => COLLAPSE event occur
   } else if (reg == COLLAPSE) {
      return COLLAPSE;
   //SHUTOFF event occur
   } else { //if COLLAPSE and SHUTOFF happend simultaneusly it will be returned only SHUTOFF
      return SHUTOFF;
   }
}

//--- EARTHQUAKE EVENT ---
//return true if an earthquake is occuring
uint8_t D7SClass::isEarthquakeOccuring() {
   //if D7S is in NORMAL MODE NOT IN STANBY (after the first 4 sec to initial delay) there is an earthquake
   return read8bit(0x10, 0x00) == NORMAL_MODE_NOT_IN_STANBY;
}

//--- INTERRUPT ---
//enable interrupt INT1 on specified pin
void D7SClass::enableInterruptINT1(uint8_t pin = D7S_INT1_PIN) {
   //enable pull up resistor
   pinMode(pin, INPUT_PULLUP);
   //attach interrupt
   attachInterrupt(digitalPinToInterrupt(pin), isr1, FALLING);
}

//enable interrupt INT2 on specified pin
void D7SClass::enableInterruptINT2(uint8_t pin = D7S_INT2_PIN) {
   //enable pull up resistor
   pinMode(pin, INPUT_PULLUP);
   //attach interrupt
   attachInterrupt(digitalPinToInterrupt(pin), isr2, CHANGE);
}

//assing the handler to the specific event
void D7SClass::addEventHandler(d7s_interrupt_event event, void (*handler) ()) {
   //check if event is in bound (it's the index to the handlers array)
   if (event < 0 || event > 3) {
      return;
   }
   //copy the pointer to the array
   _handlers[event] = handler;
}


//----------------------- PRIVATE INTERFACE -----------------------

//--- READ ---
//read 8 bit from the specified register
uint8_t D7SClass::read8bit(uint8_t regH, uint8_t regL) {

   //DEBUG
   #ifdef DEBUG
      Serial.println("--- read8bit ---");
   #endif

   //setting up i2c connection
   Wire.beginTransmission(D7S_ADDRESS);
   //write register address
   Wire.write(regH); //register address high
   Wire.write(regL); //register address low
   //status of the Wire connection
   uint8_t status = Wire.endTransmission(false);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[RE-START]: ");
      //send RE-START message
      Serial.println(status);
   #endif

   //if the status != 0 there is an error
   if (status != 0) {
      //close the connection
      Wire.endTransmission(true);
      //retry
      return read8bit(regH, regL);
   }
   //request 1 byte
   Wire.requestFrom(D7S_ADDRESS, 1);
   //wait until the data is received
   while (Wire.available() < 1)
      ;
   //read the data
   uint8_t data = Wire.read();
   //status of the Wire connection
   status = Wire.endTransmission(true);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[STOP]: ");
      //closing the connection (STOP message)
      Serial.println(status);
   #endif
  
   //delay
   delay(D7S_WIRE_DELAY);

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
   #endif

   //setting up i2c connection
   Wire.beginTransmission(D7S_ADDRESS);
   //write register address
   Wire.write(regH); //register address high
   Wire.write(regL); //register address low
   //status of the Wire connection
   uint8_t status = Wire.endTransmission(false);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[RE-START]: ");
      //send RE-START message
      Serial.println(status);
   #endif

   //if the status != 0 there is an error
   if (status != 0) {
      //close the connection
      Wire.endTransmission(true);
      //retry
      return read16bit(regH, regL);
   }

   //request 2 byte
   Wire.requestFrom(D7S_ADDRESS, 2);
   //wait until the data is received
   while (Wire.available() < 2)
      ;
   //read the data
   uint8_t msb = Wire.read();
   uint8_t lsb = Wire.read();
   //status of the Wire connection
   status = Wire.endTransmission(true);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[STOP]: ");
      //closing the connection (STOP message)
      Serial.println(status);
   #endif

   //delay
   delay(D7S_WIRE_DELAY);

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
   Wire.beginTransmission(D7S_ADDRESS);
   //write register address
   Wire.write(regH); //register address high
   Wire.write(regL); //register address low
   //write data
   Wire.write(val);

   //DEBUG
   #ifdef DEBUG
      Serial.print("[STOP]: ");
      //closing the connection (STOP message)
      Serial.println(Wire.endTransmission(true));
   #else
      //closing the connection (STOP message)
      Wire.endTransmission(true);
   #endif

   //delay
   delay(D7S_WIRE_DELAY);

   //DEBUG
   #ifdef DEBUG
      Serial.println("--- write8bit ---");
   #endif
}

//--- INTERRUPT HANDLER ---
//handle the INT1 events
void D7SClass::int1() {
   //check what event triggered the interrupt
   if (getEvent() == SHUTOFF) {
      //if the handler is defined
      if (_handlers[2]) {
         _handlers[2]();
      }
   } else {
      //if the handler is defined
      if (_handlers[3]) {
         _handlers[3]();
      }
   }
}

//handle the INT2 events
void D7SClass::int2() {
   //if the event is triggered by the user no interrupt
   if (_eventTriggered) {
      return;
   }
   //check what in what state the D7S is
   if (getState() == NORMAL_MODE_NOT_IN_STANBY) { //earthquake started
      //if the handler is defined
      if (_handlers[0]) {
         _handlers[0]();
      }
   } else { //earthquake ended
      //if the handler is defined
      if (_handlers[1]) {
         _handlers[1]();
      }
   }
}

//--- ISR HANDLER ---
//it handle the FALLING event that occur to the INT1 D7S pin (glue routine)
static void D7SClass::isr1() {
   D7S.int1();
}

//it handle the CHANGE event thant occur to the INT2 D7S pin (glue routine)
static void D7SClass::isr2() {
   D7S.int2();
}

//extern object
D7SClass D7S;
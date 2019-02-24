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

   // Reset the occuring variable
   _occuring = 0;
   _shutoffHappened = 0;

   // Interrupt disabled
   _interruptEnabled = 0;
   
   // Reset interrupt pins variables
   _pinINT1 = 0;
   _pinINT2 = 0;
   _pinINT2ChangeCapable = 1;
}

//--- BEGIN ---
//used to initialize Wire
void D7SClass::begin() {
   //begin Wire
   Wire.begin();
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
   //return the value
   return ((float) read16bit(0x30 + index, 0x08)) / 1000;
}

//get the lastest PGA at specified index (up to 5) [m/s^2]
float D7SClass::getLastestPGA(uint8_t index) {
   //return the value
   return ((float) read16bit(0x30 + index, 0x0A)) / 1000;
}

//get the lastest Temperature at specified index (up to 5) [Celsius]
float D7SClass::getLastestTemperature(uint8_t index) {
   //return the value
   return (float) ((int16_t) read16bit(0x30 + index, 0x06)) / 10;
}

//--- RANKED DATA ---
//get the ranked SI at specified position (up to 5) [m/s]
float D7SClass::getRankedSI(uint8_t position) {
   //return the value
   return ((float) read16bit(0x30 + position +5, 0x08)) / 1000;
}

//get the ranked PGA at specified position (up to 5) [m/s^2]
float D7SClass::getRankedPGA(uint8_t position) {
   //return the value
   return ((float) read16bit(0x30 + position +5, 0x0A)) / 1000;
}

//get the ranked Temperature at specified position (up to 5) [Celsius]
float D7SClass::getRankedTemperature(uint8_t position) {
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
// Attach interrupts
void D7SClass::attachInterruptHandling(uint8_t pinINT1, uint8_t pinINT2, uint8_t pinINT2ChangeCapable = 1) {
   // INT1
   pinMode(pinINT1, INPUT_PULLUP); // Enable pull up resistor
   attachInterrupt(digitalPinToInterrupt(pinINT1), isr1, FALLING); // Attach isr to interrupt event

   // INT2
   pinMode(pinINT2, INPUT_PULLUP); // Enable pull up resistor
   delay(500);
   if (pinINT2ChangeCapable) {
      attachInterrupt(digitalPinToInterrupt(pinINT2), isr2, CHANGE); // Attach isr to interrupt event
   } else {
      attachInterrupt(digitalPinToInterrupt(pinINT2), isr2, FALLING); // Attach isr to interrupt event
   }

   // Save the pin data
   _pinINT1 = pinINT1;
   _pinINT2 = pinINT2;
   _pinINT2ChangeCapable = pinINT2ChangeCapable;

   // Set that the interrupts are enabled
   _interruptEnabled = 0;

}

// Detach interrupts
void D7SClass::detachInterruptHandling() {
   detachInterrupt(digitalPinToInterrupt(_pinINT1));
   detachInterrupt(digitalPinToInterrupt(_pinINT2));
}


// Start interrupt listening
void D7SClass::startInterruptListening() {
   _interruptEnabled = 1;
}

// Stop interrupt listening
void D7SClass::stopInterruptListening() {
   _interruptEnabled = 0;
}

// Register interrupt event handler
void D7SClass::registerInterruptEventHandler(d7s_interrupt_event event, void (*handler) ()) {
   _handlers[event] = handler;
}

// Unregister interrupt event handler
void D7SClass::unregisterInterruptEventHandler(d7s_interrupt_event event) {
   _handlers[event] = NULL;
}


//----------------------- PRIVATE INTERFACE -----------------------

//--- READ ---
//read 8 bit from the specified register
uint8_t D7SClass::read8bit(uint8_t regH, uint8_t regL) {
   //DEBUG
   #ifdef DEBUG
      Serial.println("--- read8bit ---");
      Serial.print("regH: 0x");
      Serial.println(regH, HEX);
      Serial.print("regL: 0x");
      Serial.println(regH, HEX);
   #endif

   //setting up i2c connection
   Wire.beginTransmission(D7S_ADDRESS);
   
   //write register address
   Wire.write(regH); //register address high
   Wire.write(regL); //register address low
   
   //send RE-START message
   uint8_t status = Wire.endTransmission(false);

   //DEBUG
   #ifdef DEBUG
      Serial.print("RE-START status: ");
      Serial.println(status);
   #endif

   //if the status != 0 there is an error
   if (status != 0) {
      #ifdef DEBUG
        Serial.println("--- read8bit ---");
      #endif
      //status is not 0 so we need to retry the reading
      //before retry is better to delay for some time
      delay(100);
      return read8bit(regH, regL);
   }

   //request 2 byte
   Wire.requestFrom(D7S_ADDRESS, 1);

   //we need to set up a timer to prevent deadlock
   long time = millis();
   //wait until the data is received
   while (Wire.available() < 2 && time + 100 > millis())
      ;

   //let's check if the data is ready or the timer expires
   if (Wire.available() < 1) {
      #ifdef DEBUG
        Serial.println("--- read8bit ---");
      #endif
      //the data is not ready and we need to retry the reading
      //before retry is better to delay for some time
      delay(100);
      return read8bit(regH, regL);
   }

   //read the data
   uint8_t data = Wire.read();

   //DEBUG
   #ifdef DEBUG
      Serial.print('data: 0x');
      Serial.println(data);
      Serial.println("--- read16bit ---");
   #endif

   //return the data
   return data;
}

//read 16 bit from the specified register
uint16_t D7SClass::read16bit(uint8_t regH, uint8_t regL) {

   //DEBUG
   #ifdef DEBUG
      Serial.println("--- read16bit ---");
      Serial.print("regH: 0x");
      Serial.println(regH, HEX);
      Serial.print("regL: 0x");
      Serial.println(regH, HEX);
   #endif

   //setting up i2c connection
   Wire.beginTransmission(D7S_ADDRESS);
   
   //write register address
   Wire.write(regH); //register address high
   Wire.write(regL); //register address low
   
   //send RE-START message
   uint8_t status = Wire.endTransmission(false);

   //DEBUG
   #ifdef DEBUG
      Serial.print("RE-START status: ");
      Serial.println(status);
   #endif

   //if the status != 0 there is an error
   if (status != 0) {
      #ifdef DEBUG
        Serial.println("--- read16bit ---");
      #endif
      //status is not 0 so we need to retry the reading
      //before retry is better to delay for some time
      delay(100);
      return read16bit(regH, regL);
   }

   //request 2 byte
   Wire.requestFrom(D7S_ADDRESS, 2);

   //we need to set up a timer to prevent deadlock
   long time = millis();
   //wait until the data is received
   while (Wire.available() < 2 && time + 100 > millis())
      ;

   //let's check if the data is ready or the timer expires
   if (Wire.available() < 2) {
      #ifdef DEBUG
        Serial.println("--- read16bit ---");
      #endif
      //the data is not ready and we need to retry the reading
      //before retry is better to delay for some time
      delay(100);
      return read16bit(regH, regL);
   }

   //read the data
   uint8_t msb = Wire.read();
   uint8_t lsb = Wire.read();

   //DEBUG
   #ifdef DEBUG
      Serial.print('msb: 0x');
      Serial.println(msb);
      Serial.print('lsb: 0x');
      Serial.println(lsb);
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
      Serial.print("regH: 0x");
      Serial.println(regH, HEX);
      Serial.print("regL: 0x");
      Serial.println(regH, HEX);
      Serial.print("val: 0x");
      Serial.println(regH, HEX);
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
      //closing the connection (STOP message)
      uint8_t status = Wire.endTransmission(true);

      Serial.print("status: ");
      Serial.println(status);
      Serial.println("--- write8bit ---");
   #else
      Wire.endTransmission(true);
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

// Handle INT1 events
void D7SClass::int1() {
   // Check if interrupt handling is enabled
   if (!_interruptEnabled) {
      return;
   }

   // If the earthquake is occuring, the event is shutoff
   if (!_shutoffHappened) {
      if (_handlers[SHUTOFF_EVENT] != NULL) {
         _handlers[SHUTOFF_EVENT](); // Call the user handler
      }

      _shutoffHappened = 1;
   
   // If the earthquake is not occuring, the event is collapse
   } else {
      if (_handlers[COLLAPSE_EVENT] != NULL) {
         _handlers[COLLAPSE_EVENT](); // Call the user handler
      }
   }
}

// Handle INT2 events
void D7SClass::int2() {
   // Check if interrupt handling is enabled
   if (!_interruptEnabled) {
      return;
   }

   // If the earthquake was occuring, it's come to an end
   if (_occuring) {
      _occuring = 0;

      if (_handlers[END_EARTHQUAKE] != NULL) {
         _handlers[END_EARTHQUAKE](); // Call the user handler
      }

      // If the pin INT2 isn't capable of handling CHANGE interrupt event
      if (!_pinINT2ChangeCapable) {
         detachInterrupt(digitalPinToInterrupt(_pinINT2));
         // Attaching the same interrupt as RISING
         attachInterrupt(digitalPinToInterrupt(_pinINT2), isr2, RISING);
      }

      // Reset shutoff
      _shutoffHappened = 0;      

   // The earthquake is starting
   } else {
      _occuring = 1;

      if (_handlers[START_EARTHQUAKE] != NULL) {
         _handlers[START_EARTHQUAKE](); // Call the user handler
      }

      // If the pin INT2 isn't capable of handling CHANGE interrupt event
      if (!_pinINT2ChangeCapable) {
         detachInterrupt(digitalPinToInterrupt(_pinINT2));
         attachInterrupt(digitalPinToInterrupt(_pinINT2), isr2, FALLING);
      }
   }
}


//--- ISR GLUE ROUTINE ---
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
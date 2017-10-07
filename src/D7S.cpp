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
   //read the CTRL register at 0x1004
   uint8_t reg = read8bit(0x10, 0x04);
   //new register value with the threshold
   reg = (((reg >> 4) << 1) | (threshold & 0x01)) << 3;
   //update register
   write8bit(0x10, 0x04, reg);
}

//change the axis selection mode
void D7SClass::setAxis(d7s_axis_settings axisMode) {
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
   if (index < 1 || index > 5) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + index -1, 0x08)) / 1000;
}

//get the lastest PGA at specified index (up to 5) [m/s^2]
float D7SClass::getLastestPGA(uint8_t index) {
   //check if the index is in bound
   if (index < 1 || index > 5) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + index -1, 0x0A)) / 1000;
}

//get the lastest Temperature at specified index (up to 5) [Celsius]
float D7SClass::getLastestTemperature(uint8_t index) {
   //check if the index is in bound
   if (index < 1 || index > 5) {
      return 0;
   }
   //return the value
   return (float) ((int16_t) read16bit(0x30 + index -1, 0x06)) / 10;
}

//--- RANKED DATA ---
//get the ranked SI at specified position (up to 5) [m/s]
float D7SClass::getRankedSI(uint8_t position) {
   //check if the position is in bound
   if (position < 1 || position > 5) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + position +4, 0x08)) / 1000;
}

//get the ranked PGA at specified position (up to 5) [m/s^2]
float D7SClass::getRankedPGA(uint8_t position) {
   //check if the position is in bound
   if (position < 1 || position > 5) {
      return 0;
   }
   //return the value
   return ((float) read16bit(0x30 + position +4, 0x0A)) / 1000;
}

//get the ranked Temperature at specified position (up to 5) [Celsius]
float D7SClass::getRankedTemperature(uint8_t position) {
   //check if the position is in bound
   if (position < 1 || position > 5) {
      return 0;
   }
   //return the value
   return (float) ((int16_t) read16bit(0x30 + position +4, 0x06)) / 10;
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


//----------------------- PRIVATE INTERFACE -----------------------

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

   //DEBUG
   #ifdef DEBUG
      Serial.print("[RE-START]: ");
      //send RE-START message
      Serial.println(Wire.endTransmission(false));
   #else
      //send RE-START message
      Wire.endTransmission(false);
   #endif

   //request 1 byte
   Wire.requestFrom(D7S_ADDRESS, 1);
   //wait until the data is received
   while (Wire.available() < 1)
      ;
   //read the data
   uint8_t data = Wire.read();

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

   //DEBUG
   #ifdef DEBUG
      Serial.print("[RE-START]: ");
      //send RE-START message
      Serial.println(Wire.endTransmission(false));
   #else
      //send RE-START message
      Wire.endTransmission(false);
   #endif

   //request 2 byte
   Wire.requestFrom(D7S_ADDRESS, 2);
   //wait until the data is received
   while (Wire.available() < 2)
      ;
   //read the data
   uint8_t msb = Wire.read();
   uint8_t lsb = Wire.read();

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
      Serial.println("--- read16bit ---");
   #endif

   //return the data
   return (msb << 8) | lsb;
}

//write 8 bit to the register specified
void write8bit(uint8_t regH, uint8_t regL, uint8_t val) {
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
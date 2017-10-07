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

//--- CONSTRUCTOR ---
D7SClass::D7SClass() {

   //DEBUG
   #ifdef DEBUG
      Serial.begin(9600);
   #endif

}

//used to initialize Wire
void D7SClass::begin() {
   //begin Wire
   Wire.begin();
}

//return the currect state
d7s_status D7SClass::getState() {
   //read the STATE register at 0x1000
   return read8bit(0x10, 0x00) & 0x6;
}

//return the currect state
d7s_status D7SClass::getAxisInUse() {
   //read the AXIS_STATE register at 0x1001
   return read8bit(0x10, 0x01) & 0x3;
}




//----------------------- PRIVATE INTERFACE -----------------------

//read 8 bit from the specified register (register address splitted into highbits and lowbits)
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

//read 16 bit from the specified register (register address splitted into highbits and lowbits)
uint8_t D7SClass::read16bit(uint8_t regH, uint8_t regL) {

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
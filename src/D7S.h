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

#ifndef D7S_H
#define D7S_H

#include <Arduino.h>
#include <Wire.h>

// If the board is Fishino32 then we need to fix I2C interrupts priority
#if defined(_FISHINO_PIC32_) || defined(_FISHINO32_) || defined(_FISHINO32_120_) || defined(_FISHINO32_MX470F512H_) || defined(_FISHINO32_MX470F512H_120_)
   // Include fix file
   #include "../utils/Fishino32.h"
   // Define the new Wire instance to use
   #define WireD7S _Wire
#else 
   #define WireD7S Wire
#endif

//--- ADDRESS ---
#define D7S_ADDRESS 0x55 //D7S address on the I2C bus

//--- DEBUG ----
//comment this line to disable all debug information
#define DEBUG


//d7s state
enum d7s_status {
   NORMAL_MODE = 0x00,
   NORMAL_MODE_NOT_IN_STANBY = 0x01, //earthquake in progress
   INITIAL_INSTALLATION_MODE = 0x02,
   OFFSET_ACQUISITION_MODE = 0x03,
   SELFTEST_MODE = 0x04
};

//d7s axis settings
enum d7s_axis_settings {
   FORCE_YZ = 0x00,
   FORCE_XZ = 0x01,
   FORXE_XY = 0x02,
   AUTO_SWITCH = 0x03,
   SWITCH_AT_INSTALLATION = 0x04 
};

//axis state
enum d7s_axis_state {
   AXIS_YZ = 0x00,
   AXIS_XZ = 0x01,
   AXIS_XY = 0x02
};

//d7s threshold settings
enum d7s_threshold {
   THRESHOLD_HIGH = 0x00,
   THRESHOLD_LOW = 0x01
};

//message status (selftes, offset acquisition)
enum d7s_mode_status {
   D7S_OK = 0,
   D7S_ERROR = 1
};

//events handled externaly by the using using an handler (the d7s int1, int2 must be connected to interrupt pin)
enum d7s_interrupt_event {
   START_EARTHQUAKE = 0, //INT 2
   END_EARTHQUAKE = 1, //INT 2
   SHUTOFF_EVENT = 2, //INT 1
   COLLAPSE_EVENT = 3 //INT 1
};


//class D7S
class D7SClass {

   public: 

      //--- CONSTRUCTOR/DESTROYER ---
      D7SClass(); //constructor

      //--- BEGIN ---
      void begin(); //used to initialize Wire

      //--- STATUS ---
      d7s_status getState(); //return the currect state
      d7s_axis_state getAxisInUse(); //return the current axis in use

      //--- SETTINGS ---
      void setThreshold(d7s_threshold threshold); //change the threshold in use
      void setAxis(d7s_axis_settings axisMode); //change the axis selection mode

      //--- LASTEST DATA ---
      float getLastestSI(uint8_t index); //get the lastest SI at specified index (up to 5) [m/s]
      float getLastestPGA(uint8_t index); //get the lastest PGA at specified index (up to 5) [m/s^2]
      float getLastestTemperature(uint8_t index); //get the lastest Temperature at specified index (up to 5) [Celsius]

      //--- RANKED DATA ---
      float getRankedSI(uint8_t position); //get the ranked SI at specified position (up to 5) [m/s]
      float getRankedPGA(uint8_t position); //get the ranked PGA at specified position (up to 5) [m/s^2]
      float getRankedTemperature(uint8_t position); //get the ranked Temperature at specified position (up to 5) [Celsius]

      //--- INSTANTANEUS DATA ---
      float getInstantaneusSI(); //get instantaneus SI (during an earthquake) [m/s]
      float getInstantaneusPGA(); //get instantaneus PGA (during an earthquake) [m/s^2]

      //--- CLEAR MEMORY ---
      void clearEarthquakeData(); //delete both the lastest data and the ranked data
      void clearInstallationData(); //delete initializzazion data
      void clearLastestOffsetData(); //delete offset data
      void clearSelftestData(); //delete selftest data
      void clearAllData(); //delete all data

      //--- INITIALIZATION ---
      void initialize(); //initialize the d7s (start the initial installation mode)

      //--- SELFTEST ---
      void selftest(); //trigger self-diagnostic test
      d7s_mode_status getSelftestResult(); //return the result of self-diagnostic test (OK/ERROR)

      //--- OFFSET ACQUISITION ---
      void acquireOffset(); //trigger offset acquisition
      d7s_mode_status getAcquireOffsetResult(); //return the result of offset acquisition test (OK/ERROR)

      //--- SHUTOFF/COLLAPSE EVENT ---
      //after each earthquakes it's important to reset the events calling resetEvents() to prevent polluting the new data with the old one
      uint8_t isInCollapse(); //return true if the collapse condition is met (it's the sencond bit of _events)
      uint8_t isInShutoff(); //return true if the shutoff condition is met (it's the first bit of _events)
      void resetEvents(); //reset shutoff/collapse events

      //--- EARTHQUAKE EVENT ---
      uint8_t isEarthquakeOccuring(); //return true if an earthquake is occuring

      //--- READY STATE ---
      uint8_t isReady();

      //--- INTERRUPT ---
      void enableInterruptINT1(uint8_t pin); //enable interrupt INT1 on specified pin
      void enableInterruptINT2(uint8_t pin); //enable interrupt INT2 on specified pin
      void startInterruptHandling(); //start interrupt handling
      void stopInterruptHandling(); //stop interrupt handling
      void registerInterruptEventHandler(d7s_interrupt_event event, void (*handler) ()); //assing the handler to the specific event
      void registerInterruptEventHandler(d7s_interrupt_event event, void (*handler) (float, float, float)); //assing the handler to the specific event

   private:
      //handler array (it cointaint the pointer to the user defined array)
      void (*_handlers[4]) ();

      //variable to track event (first bit => SHUTOFF, second bit => COLLAPSE)
      uint8_t _events;

      //enable interrupt handling
      uint8_t _interruptEnabled;

      //--- READ ---
      uint8_t read8bit(uint8_t regH, uint8_t regL); //read 8 bit from the specified register
      uint16_t read16bit(uint8_t regH, uint8_t regL); //read 16 bit from the specified register

      //--- WRITE ---
      void write8bit(uint8_t regH, uint8_t regL, uint8_t val); //write 8 bit to the register specified

      //--- READ EVENTS ---
      void readEvents(); //read the event (SHUTOFF/COLLAPSE) from the EVENT register

      //--- EVENT HANDLER ---
      void int1(); //handle the INT1 events
      void int2(); //handle the INT2 events

      //--- ISR HANDLER ---
      static void isr1(); //it handle the FALLING event that occur to the INT1 D7S pin (glue routine)
      static void isr2(); //it handle the CHANGE event thant occur to the INT2 D7S pin (glue routine)

      // Fishino32 cannot handle CHANGE mode on interrupts, so we need to register FALLING mode first and on the isr register
      // as RISING the same pin detaching the previus interrupt
      #if defined(_FISHINO_PIC32_) || defined(_FISHINO32_) || defined(_FISHINO32_120_) || defined(_FISHINO32_MX470F512H_) || defined(_FISHINO32_MX470F512H_120_)
         uint8_t pinINT2;
      #endif

};

extern D7SClass D7S;

#endif
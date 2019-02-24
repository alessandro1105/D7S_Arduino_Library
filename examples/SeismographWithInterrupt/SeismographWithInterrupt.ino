/*
   Copyright 2019 Alessandro Pasqualini
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

#include <D7S.h>

#if defined(_FISHINO32_) // Fishino32 interrupt pins
  #define INT1_PIN 3 //interrupt pin INT1 of D7S attached to pin 3 of Fishino32
  #define INT2_PIN 5 //interrupt pin INT2 of D7S attached to pin 5 of Fishino32
  #define INT2_CHANGING false //interrupt pin INT2 capable of handling CHANGE event

#elif defined(ESP8266) // Esp8266 interrupt pins (tested on WeMos D1 R1)
  #define INT1_PIN D7 //interrupt pin INT1 of D7S attached to pin D7 of ESP8266
  #define INT2_PIN D8 //interrupt pin INT2 of D7S attached to pin D8 of ESP8266
  #define INT2_CHANGING true //interrupt pin INT2 capable of handling CHANGE event

#elif defined(ESP32) // Esp8266 interrupt pins (tested on WeMos D1 R1)
  #define INT1_PIN 13 //interrupt pin INT1 of D7S attached to pin D7 of ESP8266
  #define INT2_PIN 15 //interrupt pin INT2 of D7S attached to pin D8 of ESP8266
  #define INT2_CHANGING true //interrupt pin INT2 capable of handling CHANGE event  

#else // Arduino UNO/Fishino UNO interrupt pins
  #define INT1_PIN 2 //interrupt pin INT1 of D7S attached to pin 2 of Arduino
  #define INT2_PIN 3 //interrupt pin INT2 of D7S attached to pin 3 of Arduino
  #define INT2_CHANGING true //interrupt pin INT2 capable of handling CHANGE event
#endif

// Varibale to handle printing of earthqauke readings
bool occuring = false; // Shows if an earthqauke is occuring
bool printed = true; // Shows if the information of the earthquake have been printed


//--- EVENT HANDLERS --
//function to handle the start of an earthquake
void startEarthquakeHandler() {
  Serial.println("-------------------- EARTHQUAKE STARTED! --------------------\n");

  // Earthquake is occuring
  occuring = true;

  //Do not call any D7S methods inside this function, because D7S library uses Wire,
  //which is based on interrupts internally, and it could result in a deadlock due to interrupts
  //priorities. In most platforms Wire has a lower priority over user defined interrupts, so deadlock.
}

// //function to handle the end of an earthquake
void endEarthquakeHandler() {
  Serial.println("-------------------- EARTHQUAKE ENDED! --------------------\n");

  // Earthquake is ended
  occuring = false;
  // information must be printed
  printed = false;

  //Do not call any D7S methods inside this function, because D7S library uses Wire,
  //which is based on interrupts internally, and it could result in a deadlock due to interrupts
  //priorities. In most platforms Wire has a lower priority over user defined interrupts, so deadlock.
}

//function to handle shutoff event
void shutoffHandler() {
  //put here the code to handle the shutoff event
  Serial.println("-------------------- SHUTOFF! --------------------");
  Serial.println("Better shutting off all electronic devices!\n");

  //Do not call any D7S methods inside this function, because D7S library uses Wire,
  //which is based on interrupts internally, and it could result in a deadlock due to interrupts
  //priorities. In most platforms Wire has a lower priority over user defined interrupts, so deadlock.
}

//function to handle collapse event
void collapseHandler() {
  //put here the code to handle the collapse event
  Serial.println("-------------------- COLLAPSE! --------------------");
  Serial.println("Better shutting off all electronic devices!\n");

  //Do not call any D7S methods inside this function, because D7S library uses Wire,
  //which is based on interrupts internally, and it could result in a deadlock due to interrupts
  //priorities. In most platforms Wire has a lower priority over user defined interrupts, so deadlock.
}


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //--- STARTING ---
  Serial.print("Starting D7S communications (it may take some time)...");
  //start D7S connection
  D7S.begin();
  //wait until the D7S is ready
  while (!D7S.isReady()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("STARTED");

  //--- SETTINGS ---
  //setting the D7S to switch the axis at inizialization time
  Serial.println("Setting D7S sensor to switch axis at inizialization time.");
  D7S.setAxis(SWITCH_AT_INSTALLATION);

  //--- INTERRUPT SETTINGS ---
  // Attach interrupts
  D7S.attachInterruptHandling(INT1_PIN, INT2_PIN, INT2_CHANGING);
  //registering event handler
  D7S.registerInterruptEventHandler(START_EARTHQUAKE, &startEarthquakeHandler); //START_EARTHQUAKE event handler
  D7S.registerInterruptEventHandler(END_EARTHQUAKE, &endEarthquakeHandler); //END_EARTHQUAKE event handler
  D7S.registerInterruptEventHandler(SHUTOFF_EVENT, &shutoffHandler); //SHUTOFF_EVENT event handler
  D7S.registerInterruptEventHandler(COLLAPSE_EVENT, &collapseHandler); //COLLAPSE_EVENT event handler

  //--- INITIALIZZATION ---
  Serial.println("Initializing the D7S sensor in 2 seconds. Please keep it steady during the initialization process.");
  delay(2000);
  Serial.print("Initializing...");
  //start the initial installation procedure
  D7S.initialize();
  //wait until the D7S is ready (the initializing process is ended)
  while (!D7S.isReady()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("INITIALIZED!");

  //--- RESETTING EVENTS ---
  //reset the events shutoff/collapse memorized into the D7S
  D7S.resetEvents();

  //--- STARTING INTERRUPT HANDLING ---
  D7S.startInterruptListening();

  //--- READY TO GO ---
  Serial.println("\nListening for earthquakes!");

}

void loop() {

  if (!occuring && !printed) {
    //printing the SI
    Serial.print("\tSI: ");
    Serial.print(D7S.getLastestSI(1));
    Serial.println(" [m/s]");

    //printing the PGA
    Serial.print("\tPGA (Peak Ground Acceleration): ");
    Serial.print(D7S.getLastestPGA(1));
    Serial.println(" [m/s^2]");

    //printing the temperature at which the earthquake has occured
    Serial.print("\tTemperature: ");
    Serial.print(D7S.getLastestTemperature(1));
    Serial.println(" [°C]\n");

    // Reset events
    D7S.resetEvents();

    printed = true;
  }
}

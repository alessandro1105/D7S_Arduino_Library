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

#include <D7S.h>

//interrupt pin INT1 of D7S attached to pin 2 of Arduino
#define INT1_PIN 2
//interrupt pin INT2 of D7S attached to pin 3 of Arduino
#define INT2_PIN 3

//--- EVENT HANDLERS --
//function to handle the start of an earthquake
void startEarthquakeHandler() {
  Serial.println("-------------------- EARTHQUAKE STARTED! --------------------\n");
}

//function to handle the end of an earthquake
void endEarthquakeHandler(float si, float pga, float temperature) {
  Serial.println("-------------------- EARTHQUAKE ENDED! --------------------");
  //printing the SI
  Serial.print("\tSI: ");
  Serial.print(si);
  Serial.println(" [m/s]");

  //printing the PGA
  Serial.print("\tPGA (Peak Ground Acceleration): ");
  Serial.print(pga);
  Serial.println(" [m/s^2]");

  //printing the temperature at which the earthquake has occured
  Serial.print("\tTemperature: ");
  Serial.print(temperature);
  Serial.println(" [°C]\n");

  //reset earthquake events
  D7S.resetEvents();
}

//function to handle shutoff event
void shutoffHandler() {
  //put here the code to handle the shutoff event
  Serial.println("-------------------- SHUTOFF! --------------------\n");
  Serial.println("Shutting down all device!");
  //stop all device
  while (1)
    ;
}

//function to handle collapse event
void collapseHandler() {
  //put here the code to handle the collapse event
  Serial.println("-------------------- COLLAPSE! --------------------\n");
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
  //enabling interrupt INT1 on pin 2 of Arduino
  D7S.enableInterruptINT1(INT1_PIN);
  D7S.enableInterruptINT2(INT2_PIN);

  //registering event handler
  D7S.registerInterruptEventHandler(START_EARTHQUAKE, &startEarthquakeHandler); //START_EARTHQUAKE event handler
  D7S.registerInterruptEventHandler(END_EARTHQUAKE, &endEarthquakeHandler); //END_EARTHQUAKE event handler
  D7S.registerInterruptEventHandler(SHUTOFF_EVENT, &shutoffHandler); //SHUTOFF_EVENT event handler
  D7S.registerInterruptEventHandler(COLLAPSE_EVENT, &collapseHandler); //COLLAPSE_EVENT event handler

  //--- INITIALIZZATION ---
  Serial.println("Initializing the D7S sensor in 2 seconds. Please keep it steady during the initializing process.");
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

  //--- CHECKING FOR PREVIUS COLLAPSE ---
  //check if there there was a collapse (if this is the first time the D7S is put in place the installation data may be wrong)
  if (D7S.isInCollapse()) {
    collapseHandler();
  }

  //--- RESETTING EVENTS ---
  //reset the events shutoff/collapse memorized into the D7S
  D7S.resetEvents();

  //--- STARTING INTERRUPT HANDLING ---
  D7S.startInterruptHandling();

  //--- READY TO GO ---
  Serial.println("\nListening for earthquakes!");

}

void loop() {
  // put your main code here, to run repeatedly:
}

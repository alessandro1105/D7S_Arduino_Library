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

#include <D7S.h>

//old earthquake data
float oldSI = 0;
float oldPGA = 0;

//function to handle collapse event
void handleCollapse() {
  //put here the code to handle the collapse event
  Serial.println("COLLAPSE!");
}

//function to handle shutoff event
void handleShutoff() {
  //put here the code to handle the shutoff event
  Serial.println("SHUTOFF!");
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
  Serial.println("STARTED");

  //--- SETTINGS ---
  //setting the D7S to switch the axis at inizialization time
  Serial.println("Setting D7S sensor to switch axis at inizialization time.");
  D7S.setAxis(SWITCH_AT_INSTALLATION);

  //--- INITIALIZZAZION ---
  Serial.println("Initializing the D7S sensor in 2 seconds. Please keep it steady during the initializing process.");
  delay(2000);
  Serial.print("Initializing...");
  //start the initial installation procedure
  D7S.initialize();
  //finished
  Serial.println("INITIALIZED!");

  //--- DELAY ---
  delay(1000);

  //--- READY TO GO ---
  Serial.println("\nListening for earthquakes!");

}

void loop() {

	//checking if there is an earthquake occuring right now
  if (D7S.isEarthquakeOccuring()) {

    //get the current event
    byte event = D7S.getEvent();

    //if there is a collapse
    if (event == COLLAPSE) {
      //handle collapse event
      handleCollapse();

    //if there is a shutoff
    } else if (event == SHUTOFF) {
      //handle shutoff event
      handleShutoff();
    }

    //print information about the current earthquake
    float currentSI = D7S.getInstantaneusSI();
    float currentPGA = D7S.getInstantaneusPGA();

    if (currentSI > oldSI || currentPGA > oldPGA) {
      Serial.print("\tInstantaneus SI: ");
      Serial.print(currentSI);
      Serial.println(" [m/s]");

      //getting the lastest PGA at position i
      Serial.print("\tInstantaneus PGA (Peak Ground Acceleration): ");
      Serial.print(currentPGA);
      Serial.println(" [m/s^2]\n");

      //save the current data
      oldSI = currentSI;
      oldPGA = currentPGA;
    }
    
  } else {
    //reset the old earthquake value
    oldPGA = 0;
    oldSI = 0;
  }
}

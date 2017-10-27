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
  Serial.println("STARTED\n");

  //--- LASTEST DATA ---
  Serial.println("--- LASTEST EARTHQUAKES MEASURED ---\n");
  //print the lastest 5 earthquakes registered with all data
  for (int i = 0; i < 5; i++) { //the index must be from 0 to 4 (5 earthquakes total)
  	Serial.print("Earthquake n. ");
  	Serial.println(i+1);

  	//getting the lastest SI at index i
  	Serial.print("\tSI: ");
  	Serial.print(D7S.getLastestSI(i));
  	Serial.println(" [m/s]");

  	//getting the lastest PGA at index i
  	Serial.print("\tPGA (Peak Ground Acceleration): ");
  	Serial.print(D7S.getLastestPGA(i));
  	Serial.println(" [m/s^2]");

  	//getting the temperature at which the earthquake at index i has occured
  	Serial.print("\tTemperature: ");
  	Serial.print(D7S.getLastestTemperature(i));
  	Serial.println(" [°C]\n");
  }
}

void loop() {
	// put your main code here, to run repeatedly:
}

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

  //making a selftest to check the D7S sensor
  Serial.print("Starting selftest...");
  D7S.selftest();
  //wait until the D7S selftest is ended
  while (!D7S.isReady()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("FINSHED\n");

  Serial.print("The result of the selftest is: ");

  //checking the result
  if (D7S.getSelftestResult() == D7S_OK) {
    Serial.println("SUCCESS!");
  } else {
    Serial.println("ERROR!");
  }

}

void loop() {
	// put your main code here, to run repeatedly:
}

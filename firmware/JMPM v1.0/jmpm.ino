/*
   Copyright (c) 2020, Jm Casler
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "credentials.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "esp_wifi.h"

#include <ArduinoOTA.h>
#include <Preferences.h>
#include "EmonLib.h"                   // Include Emon Library

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

EnergyMonitor emon1;                   // Create an instance
EnergyMonitor emon2;                   // Create an instance
EnergyMonitor emon3;                   // Create an instance
EnergyMonitor emon4;                   // Create an instance
EnergyMonitor emon5;                   // Create an instance

uint32_t Pin_Voltage = 36;
uint32_t Pin_Current_1 = 39;
uint32_t Pin_Current_2 = 34;
uint32_t Pin_Current_3 = 35;
uint32_t Pin_Current_4 = 32;
uint32_t Pin_Current_5 = 33;

const uint32_t number_of_current_sensors = 5;

uint8_t has_voltage_sensor = 0;

const uint8_t numReadings = 20;
uint8_t readIndex = 0;

const uint8_t historyLength = 60;
uint8_t historyFrequency = 10; // Save to history every x seconds.
uint8_t historyIndex = 0;

double currentSensorReadings[number_of_current_sensors][numReadings];
double currentSensorAverages[number_of_current_sensors];

uint32_t currentSensorHistoryTimestamp[historyLength];
double currentSensorHistory[historyLength][number_of_current_sensors];

// Create object to store our persistant data
Preferences preferences;

const char *ssid = mySSID;
const char *password = myPASSWORD;


WebServer server(80);

// Set your Static IP address
//IPAddress local_IP(192, 168, 4, 11);
// Set your Gateway IP address
//IPAddress gateway(192, 168, 4, 1);
// Set your subnet
//IPAddress subnet(255, 255, 254, 0);

int reconnectCount = 0;

uint32_t wifi_reconnects = 0;

uint32_t rebootCounter;

uint32_t bootSalt = 0;

int16_t currentRSSI;
uint32_t lastMillis = 0;

void TaskWeb( void *pvParameters );
void TaskWifi( void *pvParameters );

void setup(void) {
  // DO NOT REMOVE THIS! It gives the serial port some time to accept a firmware update.
  delay(100);

  Serial.begin(115200);

  randomSeed(analogRead(Pin_Current_1) * analogRead(Pin_Current_2) * analogRead(Pin_Current_3) * analogRead(Pin_Current_4)  * analogRead(Pin_Current_5) );
  bootSalt = random(2147483647);

  Serial.println("");
  Serial.println("JMIM v1.0");


  //  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
  analogReadResolution(ADC_BITS); // Configure ADC to 12bit reads
  analogSetCycles(16);
  analogSetClockDiv(1); // 1338mS


  pinMode(Pin_Voltage, INPUT);
  pinMode(Pin_Current_1, INPUT);
  pinMode(Pin_Current_2, INPUT);
  pinMode(Pin_Current_3, INPUT);
  pinMode(Pin_Current_4, INPUT);
  pinMode(Pin_Current_5, INPUT);

  // Initialize our array of arrays that hold the sensor data.
  for (uint8_t sensor = 0; sensor < number_of_current_sensors; sensor++) {
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      currentSensorReadings[sensor][thisReading] = 0;
    }
  }

  // Calibration ... 35A scale = (2000 turns / 68ohm burden) = 29.4
  // Calibration ... 120A scale = (2000 turns / 19.33ohm burden) = 103.4
  float calibration = 29.4;
  emon1.current(Pin_Current_1, calibration);             // Current: input pin, calibration.
  emon2.current(Pin_Current_2, calibration);             // Current: input pin, calibration.
  emon3.current(Pin_Current_3, calibration);             // Current: input pin, calibration.
  emon4.current(Pin_Current_4, calibration);             // Current: input pin, calibration.
  emon5.current(Pin_Current_5, calibration);             // Current: input pin, calibration.

  // ------------------------------------------


  Serial.println("Setup Preferences (Flash Storage)");
  preferences.begin("IMJM", false);

  // Note: Namespace name is limited to 15 chars.
  rebootCounter = preferences.getUInt("rebootCounter", 0);

  rebootCounter++;
  preferences.putUInt("rebootCounter", rebootCounter);

  Serial.println("Setup Preferences (Flash Storage) - Done");

  // ------------------------------------------

  // ------------------------------------------

  Serial.println("IP Setup");
  // Configures static IP address
  /*

    if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("  STA Failed to configure");
    }
  */
  Serial.println("IP Setup - Done");
  // ------------------------------------------

  Serial.println("WiFi Station Setup");
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE); // Disable power saving

  WiFi.begin(ssid, password);
  Serial.println("WiFi Station Setup - Done");
  // ------------------------------------------

  // Wait for connection
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    reconnectCount++;
  }

  Serial.println("");
  Serial.print("  Connected to ");
  Serial.println(ssid);
  Serial.print("  IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("  Subnet Mas: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("  Gateway address ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("  Device MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("  RSSI: ");
  Serial.println(WiFi.RSSI());
  Serial.println("Connecting to WiFi - Done");

  /*
     Handlers for HTTP Server
  */
  Serial.println("HTTP server Configuration...");
  server.on("/", handleStrip);
  server.on("/json/stats", handleReturnJSON_stats);
  server.on("/json/current/live", handleReturnJSON_Current_Live);
  server.on("/json/current/history", handleReturnJSON_Current_History);
  server.on("/strip", handleStrip);
  server.on("/clearPreferences", handleClearPreferences);
  server.on("/reboot", handleReboot);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started!");

  // Now set up two tasks to run independently.
  xTaskCreatePinnedToCore(
    TaskWeb
    ,  "TaskWeb"   // A name just for humans
    ,  2048  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL
    ,  ARDUINO_RUNNING_CORE);

  // ------------------------------------------

  Serial.println("");
  Serial.printf("Number of times rebooted: %u\n", rebootCounter);
  Serial.println("Startup Complete. Let the fun begin!");
  Serial.println("Have a briliant day.");
  Serial.println("");


}


void loop(void) {

  uint32_t samples = 740;

  currentSensorReadings[0][readIndex] = emon1.calcIrms(samples);  // Calculate Irms only
  currentSensorReadings[1][readIndex] = emon2.calcIrms(samples);  // Calculate Irms only
  currentSensorReadings[2][readIndex] = emon3.calcIrms(samples);  // Calculate Irms only
  currentSensorReadings[3][readIndex] = emon4.calcIrms(samples);  // Calculate Irms only
  currentSensorReadings[4][readIndex] = emon5.calcIrms(samples);  // Calculate Irms only

  //Serial.println(currentSensorReadings[0][readIndex]);

  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  for (uint8_t sensor = 0; sensor < number_of_current_sensors; sensor++) {
    double total = 0;
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      total = total + currentSensorReadings[sensor][thisReading];
    }
    double average = total / numReadings;
    currentSensorAverages[sensor] = average;
  }

  currentRSSI = WiFi.RSSI();

  // Do this every historyFrequency
  if (millis() - lastMillis > (1000 * historyFrequency)) {

    //Serial.println(" Seconds ...");
    //Serial.println(loopCount);

    for (uint8_t sensor = 0; sensor < number_of_current_sensors; sensor++) {
      currentSensorHistoryTimestamp[historyIndex] = (millis() / 1000);
      currentSensorHistory[historyIndex][0] = currentSensorAverages[0];
      currentSensorHistory[historyIndex][1] = currentSensorAverages[1];
      currentSensorHistory[historyIndex][2] = currentSensorAverages[2];
      currentSensorHistory[historyIndex][3] = currentSensorAverages[3];
      currentSensorHistory[historyIndex][4] = currentSensorAverages[4];
    }

    for (uint8_t sensor = 0; sensor < number_of_current_sensors; sensor++) {
      //        Serial.print(sensor);
      //        Serial.print(" - ");
      Serial.print(currentSensorAverages[sensor]);
      Serial.print("A ");
      Serial.print(currentSensorAverages[sensor] * 120);
      Serial.print("W ");
      Serial.print("     ");
    }
    Serial.println();

    historyIndex++;
    // if we're at the end of the array...
    if (historyIndex >= historyLength) {
      // ...wrap around to the beginning:
      historyIndex = 0;
    }

    lastMillis = millis();
  }
  // Check that WiFi is connected. If not, reconnect.
  reconnectWiFi();

}


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskWeb(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;) // A Task shall never return or exit.
  {
    server.handleClient();
  }
}

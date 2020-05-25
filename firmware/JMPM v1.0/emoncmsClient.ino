void TaskEmoncms(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
  uint8_t emoncmsInterval = 5;
  String emoncmsAddress = "192.168.1.100:8080";
  String emoncmsNode = "jmpm_dev";
  String emoncmsAPIKey = "longhairystring";
  uint16_t volt0 = 120;
  uint16_t volt1 = 120;
  uint16_t volt2 = 120;
  uint16_t volt3 = 120;
  uint16_t volt4 = 120;

  String httpClient_code = "";
  String httpClient_error = "";
  String httpClient_data = "";

  for (;;) // A Task shall never return or exit.
  {
    // Only make the request if we're connected to WiFi
    if ( WiFi.status() ==  WL_CONNECTED ) {
      HTTPClient http;

      String httpRequest = "http://" + emoncmsAddress + "/input/post?node=" + emoncmsNode + "&json={power1:" + String(currentSensorAverages[0] * volt0) + ",power2:" + String(currentSensorAverages[1] * volt1) + ",power3:" + String(currentSensorAverages[2] * volt2) + ",power4:" + String(currentSensorAverages[3] * volt3) + ",power5:" + String(currentSensorAverages[4] * volt4) + "}&apikey=" + emoncmsAPIKey;
      //Serial.println(httpRequest);

      http.begin(httpRequest); //HTTP

      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        httpClient_code = String(httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          httpClient_data = http.getString();
        }
      } else {
        httpClient_error = http.errorToString(httpCode).c_str();
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
      //delay(5 * 1000);
    }
    vTaskDelay( xDelay * emoncmsInterval );
  }
}

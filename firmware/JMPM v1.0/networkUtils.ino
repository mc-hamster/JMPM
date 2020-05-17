
/*
   TODO:
     - If RSSI is too low, scan for another AP that we can connect and reconnect
     - Blink an LED
*/
void reconnectWiFi() {

  if ( WiFi.status() !=  WL_CONNECTED ) {

    Serial.println("Lost connection to AP. Reconnecting");
    WiFi.begin(  );

    // Keep track that connection was lost.
    wifi_reconnects++;

    // Clear the display

    while (WiFi.status() != WL_CONNECTED) {
      delay( 500 );

      Serial.printf(".");

      reconnectCount++;

      if (WiFi.status() ==  WL_CONNECTED) {
        Serial.println("Connection to AP restored!");

      }
    }
  }
}

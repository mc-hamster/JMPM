
void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x += 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}

void handleStrip() {





  String out = "";
  out += "<head>\n";
  out += "<meta name='viewport' content='initial-scale=1, user-scalable=no'>\n";
  out += "<title>JMPM v0.001</title>";
  out += "</head>";
  out += "<body>";
  out += "<form>\n";

  out += "<a href=/json/current/live>JSON - Live Data</a><br>\n";
  out += "<a href=/json/current/history>JSON - Historical Data</a><br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<a href=/json/stats>JSON - Stats</a><br>\n";
  out += "<a href=/reboot>Reboot Device</a><br>\n";

  out += "</form>\n";
  out += "</body>";

  server.send(200, "text/html", out);
  //Serial.println(out);
  return;
}



void handleReturnJSON_Current_Live() {
  String out = "";
  out += "{\n";
  out += "  \"data\" : {\n";
  out += "    \"salt\" : " + String(bootSalt) + ",\n";
  out += "    \"currents\" : [";



  out += String(currentSensorAverages[0]) + ",";
  out += String(currentSensorAverages[1]) + ",";
  out += String(currentSensorAverages[2]) + "," ;
  out += String(currentSensorAverages[3]) + "," ;
  out += String(currentSensorAverages[4]);
  out += "]\n";
  out += "  }\n";
  out += "}\n";

  server.send ( 200, "application/json", out );
  return;

}
void handleReturnJSON_Current_History() {
  String out = "";
  out += "{\n";
  out += "  \"data\" : {\n";
  out += "    \"salt\" : " + String(bootSalt) + ",\n";
  out += "    \"saveEverySeconds\" : " + String(historyFrequency) + ",\n";  
  out += "    \"currents\" : [\n";


  for (uint8_t history_id = 0; history_id < historyLength; history_id++) {
    out += "      [";
    out += String(currentSensorHistoryTimestamp[history_id]);
    out += ",";
    out += String(currentSensorHistory[history_id][1]);
    out += ",";
    out += String(currentSensorHistory[history_id][2]);
    out += ",";
    out += String(currentSensorHistory[history_id][3]);
    out += ",";
    out += String(currentSensorHistory[history_id][4]);
    out += ",";
    out += String(currentSensorHistory[history_id][5]);
    if (history_id < historyLength - 1) {
      out += "],\n";

    } else {
      out += "]\n";

    }
  }
  out += "      ]\n";


  out += "  }\n";
  out += "}\n";

  server.send ( 200, "application/json", out );
  return;

}

void handleReturnJSON_stats() {
  String out = "";
  out += "{\n";
  out += "  \"data\" : {\n";
  out += "    \"rebootCounter\" : " + String(rebootCounter) + ",\n";
  out += "    \"salt\" : " + String(bootSalt) + ",\n";
  out += "    \"saveEverySeconds\" : " + String(historyFrequency) + ",\n";  
  out += "    \"wifiReconnectCount\" : " + String(reconnectCount) + ",\n";
  out += "    \"rssi\" : " + String(currentRSSI) + "\n";
  out += "  }\n";
  out += "}\n";

  server.send ( 200, "application/json", out );
  return;

}


void handleClearPreferences() {

  String out = "";
  out += "<meta http-equiv=refresh content=\"2; url=/\">";
  out += "Preferences Cleared. Rebooting.";



  server.send ( 200, "text/html", out );

  preferences.clear();
  preferences.end();
  ESP.restart();

  delay(500); // Need to have a delay so HTTP server has time to deliver content to client.
  return;

}

void handleReboot() {

  String out = "";
  out += "<meta http-equiv=refresh content=\"3; url=/\">";
  out += "Rebooting ... Will <a href=/>redirect</a> in 3 seconds ...";

  server.send ( 200, "text/html", out );
  delay(500); // Need to have a delay so HTTP server has time to deliver content to client.

  preferences.end();
  ESP.restart();

  return;

}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

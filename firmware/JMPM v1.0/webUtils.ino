/* Style */
String style =
  "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
  "input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
  "#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
  "#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
  "form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
  ".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

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

void handleRoot() {


  String out = "";
  out += "<head>\n";
  out += "<meta name='viewport' content='initial-scale=1, user-scalable=no'>\n";
  out += "<title>JMPM v0.001</title>";
  out += "</head>";
  out += "<body>";

  out += "<a href=/json/current/live>JSON - Live Data</a><br>\n";
  out += "<a href=/json/current/history>JSON - Historical Data</a><br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<form action=/json/current/calibrate>\n";
  out += "Calibrate Sensors<br>(35A = 29.40; 120A = 103.40)<br>\n";
  out += "<select id=current name=current>\n";
  out += "  <option value=1>1</option>\n";
  out += "  <option value=2>2</option>\n";
  out += "  <option value=3>3</option>\n";
  out += "  <option value=4>4</option>\n";
  out += "  <option value=5>5</option>\n";
  out += "</select>\n";
  out += "Value <input type=text id=value name=value>\n";
  out += "<input type=submit value=Submit>\n";
  out += "</form>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<br>\n";
  out += "<a href=/json/stats>JSON - Stats</a><br>\n";
  out += "<a href=/clearPreferences>Clear Preferences</a><br>\n";
  out += "<a href=/update/form>Firmware Update</a><br>\n";
  out += "<a href=/reboot>Reboot Device</a><br>\n";
  //out += style;
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

void handleReturnJSON_Current_Calibrate() {

  String paramCurrent = server.arg("current");
  String paramValue = server.arg("value");

  if (server.hasArg("current") == true && server.hasArg("value") == true) {
    if (paramCurrent.toInt() == 1) {
      Serial.println("b");
      calibration_1 = paramValue.toDouble();
      preferences.putDouble("calibration_1", calibration_1);
    } else if (paramCurrent.toInt() == 2) {
      calibration_2 = paramValue.toDouble();
      preferences.putDouble("calibration_2", calibration_2);

    } else if (paramCurrent.toInt() == 3) {
      calibration_3 = paramValue.toDouble();
      preferences.putDouble("calibration_3", calibration_3);

    } else if (paramCurrent.toInt() == 4) {
      calibration_4 = paramValue.toDouble();
      preferences.putDouble("calibration_4", calibration_4);

    } else if (paramCurrent.toInt() == 5) {
      calibration_5 = paramValue.toDouble();
      preferences.putDouble("calibration_5", calibration_5);

    }
  }

  String out = "";
  out += "{\n";
  out += "  \"data\" : {\n";
  out += "    \"calibration_1\" : " + String(calibration_1) + "\n";
  out += "    \"calibration_2\" : " + String(calibration_2) + "\n";
  out += "    \"calibration_3\" : " + String(calibration_3) + "\n";
  out += "    \"calibration_4\" : " + String(calibration_4) + "\n";
  out += "    \"calibration_5\" : " + String(calibration_5) + "\n";
  out += "  }\n";
  out += "}\n";

  server.send ( 200, "application/json", out );
  return;

}

void handleReturnJSON_stats() {
  uint32_t upTime = millis();

  String out = "";
  out += "{\n";
  out += "  \"data\" : {\n";
  out += "    \"calibration_1\" : " + String(calibration_1) + "\n";
  out += "    \"calibration_2\" : " + String(calibration_2) + "\n";
  out += "    \"calibration_3\" : " + String(calibration_3) + "\n";
  out += "    \"calibration_4\" : " + String(calibration_4) + "\n";
  out += "    \"calibration_5\" : " + String(calibration_5) + "\n";
  out += "    \"upTime\" : " + String(upTime) + "\n";
  out += "    \"upTimeSec\" : " + String(upTime / 1000) + "\n";
  out += "    \"upTimeMin\" : " + String(upTime / 1000 / 60) + "\n";
  out += "    \"rssi\" : " + String(currentRSSI) + "\n";
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


void handleOtaIndex() {
  /* Server Index Page */
  String updateIndex =
    "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
    "<label id='file-input' for='file'>   Choose file...</label>"
    "<input type='submit' class=btn value='Update'>"
    "<br><br>"
    "<div id='prg'></div>"
    "<br><div id='prgbar'><div id='bar'></div></div><br></form>"
    "<script>"
    "function sub(obj){"
    "var fileName = obj.value.split('\\\\');"
    "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
    "};"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    "$.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "$('#bar').css('width',Math.round(per*100) + '%');"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!') "
    "},"
    "error: function (a, b, c) {"
    "}"
    "});"
    "});"
    "</script>" + style;

  server.send ( 200, "text/html", updateIndex );
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

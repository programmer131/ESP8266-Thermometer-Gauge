#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SimpleDHT.h>

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = D2;
SimpleDHT11 dht11;


const char index_html[] PROGMEM={"<!DOCTYPE html><html lang=\"en\"><head><title>My first chart using FusionCharts Suite XT</title><script type=\"text/javascript\" src=\"http://static.fusioncharts.com/code/latest/fusioncharts.js\"></script><script type=\"text/javascript\" src=\"http://static.fusioncharts.com/code/latest/themes/fusioncharts.theme.fint.js?cacheBust=56\"></script><script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script><script type=\"text/javascript\">$(document).ready(function() { FusionCharts.ready(function(){ var fusioncharts = new FusionCharts({ type: 'thermometer', renderAt: 'chart-container', id: 'myThm', width: '240', height: '310', dataFormat: 'json', dataSource: { \"chart\": { \"caption\": \"Temperature Monitor\", \"subcaption\": \" bedroom\", \"lowerLimit\": \"-10\", \"upperLimit\": \"50\", \"decimals\": \"1\", \"numberSuffix\": \"°C\", \"showhovereffect\": \"1\", \"thmFillColor\": \"#008ee4\", \"showGaugeBorder\": \"1\", \"gaugeBorderColor\": \"#008ee4\", \"gaugeBorderThickness\": \"1\", \"gaugeBorderAlpha\": \"30\", \"thmOriginX\": \"100\", \"chartBottomMargin\": \"20\", \"valueFontColor\": \"#000000\", \"theme\": \"fint\" }, \"value\": \"20\", \"annotations\": { \"showbelow\": \"0\", \"groups\": [{ \"id\": \"indicator\", \"items\": [ { \"id\": \"background\", \"type\": \"rectangle\", \"alpha\": \"50\", \"fillColor\": \"#AABBCC\", \"x\": \"$gaugeEndX-40\", \"tox\": \"$gaugeEndX\", \"y\": \"$gaugeEndY+54\", \"toy\": \"$gaugeEndY+72\" } ] }] }, }, \"events\": { \"initialized\": function(evt, arg) { evt.sender.dataUpdate = setInterval(function() { var someUrl = \"/temp\";$.ajax({url: someUrl,dataType: \"text\",success: function(response) {evt.sender.feedData(\"&value=\" + response);},timeout: 2000}) }, 3000); updateAnnotation = function(evtObj, argObj) { var code, chartObj = evtObj.sender, val = chartObj.getData(), annotations = chartObj.annotations; if (val <= 30) { code = \"#00FF00\"; } else if (val < 40 && val > 30) { code = \"#ff9900\"; } else { code = \"#ff0000\"; } annotations.update(\"background\", { \"fillColor\": code }); }; }, 'renderComplete': function(evt, arg) { updateAnnotation(evt, arg); }, 'realtimeUpdateComplete': function(evt, arg) { updateAnnotation(evt, arg); }, 'disposed': function(evt, arg) { clearInterval(evt.sender.dataUpdate); } }});fusioncharts.render();});});</script></head><body> <div align=\"center\" id=\"chart-container\">FusionCharts XT will load here!</div></body></html>"};

//uncomment these two lines and write your ssid/password
//const char* ssid = "ssid";
//const char* password = "pswd";


ESP8266WebServer server(80);
String dataToSend="";
void handleRoot() {
  server.send_P(200, "text/html;charset=UTF-8", index_html);
}

void handleTemp(){
  server.send(200,"text",dataToSend);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  server.send(404, "text/plain", message);
}


byte temperature;
byte humidity = 0;
 
void runPeriodicFunc()
{
    static const unsigned long REFRESH_INTERVAL1 = 1000; // ms
    static unsigned long lastRefreshTime1 = 0;
    
    if(millis() - lastRefreshTime1 >= REFRESH_INTERVAL1)
    {   
     Serial.println("=================================");
      Serial.println("Sample DHT11...");
      int err = SimpleDHTErrSuccess;
      if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
        Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
        return;
      }
      Serial.print((int)temperature); Serial.print(" *C, "); 
      Serial.print((int)humidity); Serial.println(" H");

     dataToSend="";
     dataToSend+=String(temperature);                       
    lastRefreshTime1 = millis();
   }
}

void setup(void){
  Serial.begin(115200);  
  WiFi.begin(ssid, password);
  Serial.println("");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP address:");  
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);  
  server.on("/temp",handleTemp);
    
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  runPeriodicFunc();
  server.handleClient();
}

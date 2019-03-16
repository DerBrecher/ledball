#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <secrets.h>

//Define for Connection to the LED Controll Sever
#define LEDSERVERPI "192.168.178.147"
#define LEDSERVERPORT 5000
#define LEDSTATEURI "/api/ledstate"

#define POLLINGRATE 1000/1 //Polls per secound



//Global Timer Variables
uint32_t currentMillis = 0;
uint32_t lastMillisPolling = 0;

HTTPClient http;
StaticJsonDocument<256> doc;

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(SSIDHDS, WPA2HDS);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  currentMillis = millis();
  if ((currentMillis - lastMillisPolling) > POLLINGRATE) {
    lastMillisPolling = currentMillis;
    Serial.println("Timer Working");
    getCurrentLEDState();
  }
}


void getCurrentLEDState() {
  http.begin(LEDSERVERPI, LEDSERVERPORT, LEDSTATEURI);
  int httpCode = http.GET();

  Serial.print("Http Code is: ");
  Serial.println(httpCode);
  if (httpCode == 200) {
    //Serial.print("Paylout is: ");
    //Serial.println(http.getString());    
    deserializeJson(doc, http.getString());
    Serial.print("Red is: ");
    int red = doc["color"]["red"];
    Serial.println(red);
  }
  http.end();
}

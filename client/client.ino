#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NeoPixelBus.h>

#include <secrets.h>

//Define for Connection to the LED Controll Sever
#define LEDSERVERPI "192.168.99.34"
#define LEDSERVERPORT 5000
#define LEDSTATEURI "/api/ledstate"

#define POLLINGRATE 1000/1 //Polls per secound


//Global Timer Variables
uint32_t currentMillis = 0;
uint32_t lastMillisPolling = 0;

//WS2812 Array
int PixelCount = 49;
NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount);


HTTPClient http;
StaticJsonDocument<256> doc;

void setup() {
  Serial.begin(115200);
  Serial.println();

  //WiFi.begin(SSIDHDS, WPA2HDS);
  WiFi.begin(SSIDWZ, WPA2WZ);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  strip.Begin();
  Serial.println("LED initialized");
}

void loop() {
  currentMillis = millis();
  if ((currentMillis - lastMillisPolling) > POLLINGRATE) {
    lastMillisPolling = currentMillis;
    Serial.println("Timer Working");
    getCurrentLEDState();
  }
  strip.Show();
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
    int green = doc["color"]["green"];
    int blue = doc["color"]["blue"];
    setStripColor(red, green, blue);
  }
  http.end();
}

void setStripColor(int red, int green, int blue) {
  for (int i = 0; i < strip.PixelCount(); i++) {
    *(strip.Pixels() + i * 3) = red;
    *(strip.Pixels() + i * 3 + 1) = green;
    *(strip.Pixels() + i * 3 + 2) = blue;
    strip.Dirty();
  }
}

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <secrets.h>
/*
  Name        :   Led Ball Controller
  Programmer  :   Nico Weidenfeller
  Created     :   21.02.2019
  Version     :   1.0
  Description :   Controller for a 400-led Disco Ball (size can be changed with the Resolution) with a Resolution of 16 * 25

  ToDoList    :   => Check - Add gamma correction
                  - When gamma correction is used fix fade to new color with gamma correction

  Error Help  :   1. If the ball blinks red and black its an Effect Watchdog Error. If this happens an Effect to longer than 10 sec to complete
                  2. If the ball has a green ring going around it. Then the WiFi is disconnected. If this happens the ball will try to restart itself after 3 effect cycles
                  3. If the ball has a red ring going around it. Then its a General Error. If this happens try to restart the ball, and check the States if it happens again.

  Patchnotes  :   Version 1.0
                  Initial Code

  EffectList  :   1. fadeall()              => Fades all pixels to black by an nscale8 number
                  2. black()                => Makes all LEDs black (no brightness change)
                  3. setSolidColor()        => Fills all LEDs with new Color
                  4. setFadeColor()         => Fades all LEDs to new Color
                  5. setSolidBrightness()   => Sets the Brightness to the new value
                  6. setFadeBrightness()    => Fades the Brightness to the new value
                  7. fillSoild()            => Fills the Matrix with a choosen Color
                  8. fadeAllToBlack()       => Fades the Matrix to Black
                  9: fadeAllToPrecent()     => Fades the Matrix to a Percentage
*/

#define max(a,b) ((a)>(b)?(a):(b))

boolean DevMode = true;

//Debug Defines
#define DEBUG_NETWORK
//#define DEBUG_STATES

//FPS
#define FPS 60

//LED Ball
#define NUM_LEDS    400
//Hardware support forces PIN 3 as Pixel Output
NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(NUM_LEDS);

//Matrix Specs
#define matrix_x 16
#define matrix_y 25
CRGB leds[matrix_x][matrix_y];

//MQTT (Temp for Testing Only)
WiFiClient    wifiMqtt;
PubSubClient  mqtt_Client(wifiMqtt);
#define mqtt_server       "192.168.99.23"
#define mqtt_username     "pi"
#define mqtt_password     "raspberry"
#define mqtt_port         1883
#define mqtt_client_name  "MumLedBallControllerTest11"

//LED State Server
#define LEDSERVERIP "192.168.99.34"
#define LEDSERVERPORT 5000
#define LEDSTATEURI "/api/ledstate"

#define POLLINGRATE 1000/1

//WIFI Parameter
const char* wifi_ssid     = SSIDWZ;
const char* wifi_password = WPA2WZ;

//-------------------------------------Mqtt Paths--------------------------------------//
const char* mqtt_LED_setcolor       = "ledball/setcolor";
const char* mqtt_LED_getcolor       = "ledball/getcolor";
const char* mqtt_LED_setbrightness  = "ledball/setbrightness";
const char* mqtt_LED_getbrightness  = "ledball/getbrightness";
const char* mqtt_LED_setstatus      = "ledball/setstatus";
const char* mqtt_LED_getstatus      = "ledball/getstatus";
const char* mqtt_LED_seteffect      = "ledball/seteffect";
const char* mqtt_LED_geteffect      = "ledball/geteffect";
const char* mqtt_LED_EffectSpeed    = "ledball/effectspeed";
const char* mqtt_LED_FadeSpeed      = "ledball/fadespeed";
const char* mqtt_LED_Random_Effect  = "ledball/randomeffect";
const char* mqtt_LED_Random_Color   = "ledball/randomcolor";
const char* mqtt_LED_RainbowActive  = "ledball/rainbowactive";
//-----------------------------------End Mqtt Paths------------------------------------//

//Effect Parameter
boolean Power = false;          //On / Off
boolean Random_Color = false;   //On / Off
boolean Random_Effect = false;  //On / Off
uint8_t Color_Red = 0;          //0 - 255
uint8_t Color_Green = 255;      //0 - 255
uint8_t Color_Blue = 0;         //0 - 255
uint8_t Brightness = 150;       //0 - 255
uint8_t Effect_Speed = 10;      //0 - 255
uint8_t Fade_Speed = 10;        //0 - 255
uint8_t Effect_Number = 0;      //0 - 255
uint8_t Effect_Direction = 2;   // 8 == Up // 6 == Right // 2 == Down // 4 == Left
boolean Rainbow = false;        //On / Off
boolean Rainbow_Active = false; //On / Off


//Gamma Correction Array
const uint8_t gamma8[] = {
  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,
  2,  3,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,
  7,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
  15, 15, 16, 17, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 24,
  24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35,
  36, 36, 37, 38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 61, 62,
  63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78,
  79, 80, 81, 82, 83, 84, 85, 86, 88, 89, 90, 91, 92, 93, 94, 95,
  97, 98, 99, 100, 101, 102, 103, 105, 106, 107, 108, 109, 111, 112, 113, 114,
  115, 117, 118, 119, 120, 122, 123, 124, 125, 127, 128, 129, 131, 132, 133, 134,
  136, 137, 138, 140, 141, 142, 144, 145, 146, 148, 149, 151, 152, 153, 155, 156,
  157, 159, 160, 162, 163, 164, 166, 167, 169, 170, 172, 173, 174, 176, 177, 179,
  180, 182, 183, 185, 186, 188, 189, 191, 192, 194, 195, 197, 198, 200, 201, 203,
  205, 206, 208, 209, 211, 212, 214, 216, 217, 219, 220, 222, 224, 225, 227, 228,
  230, 232, 233, 235, 237, 238, 240, 242, 243, 245, 247, 248, 250, 252, 253, 255
};

//Error Variables /Normal Variables for  WiFi
int CantFindWiFiCounter = 0;
boolean CantFindWiFi = false;
boolean SaveLastMainState = false;
int CantFindWiFiLastMainState;
int CounterToRestartESP = 0; //If Counter is 5 the ESP Will Restart itself

//Main Control
int MainState = 0;
int LastMainState = 0;
boolean DevelopMode = false;  //Not used

boolean MainStateRGBCheck = false;
boolean MainStateStatusEffectStartupReady = false;

//Light Controller
int LightState = 0;
int LastLightState = 0;

//Other States
int CheckRGBState = 0;

//Timer / Delay
unsigned long PrevMillis_Example              = 0;
unsigned long PrevMillis_RGBCheck             = 0;
unsigned long PrevMillis_EffectNoWiFi         = 0;
unsigned long PrevMillis_GeneralError         = 0;
unsigned long PrevMillis_EffectStartupReady   = 0;
unsigned long PrevMillis_FadeBrightness       = 0;
unsigned long PrevMillis_FadeColor            = 0;
unsigned long PrevMillis_EffectRainDrop       = 0;
unsigned long PrevMillis_RandomColor          = 0;
unsigned long PrevMillis_RainbowColor         = 0;
unsigned long PrevMillis_DiscoBall            = 0;
unsigned long PrevMillis_Rave                 = 0;
unsigned long PrevMillis_RingRun              = 0;
unsigned long PrevMillis_DiscoField           = 0;
unsigned long PrevMillis_JSONPolling          = 0;

unsigned long TimeOut_Example                 = 1000; // 1.00 Seconds
unsigned long TimeOut_RGBCheck                = 2000; // 2.00 Seconds
unsigned long TimeOut_EffectNoWiFi            = 60;   // 0.06 Seconds    => Effect Speed
unsigned long TimeOut_GeneralError            = 60;   // 0.06 Seconds    => Effect Speed
unsigned long TimeOut_EffectStartupReady      = 30;   // 0.03 Seconds    => Effect Speed
unsigned long TimeOut_FadeBrightness          = 10;   // 0.01 Seconds    => Effect Speed
unsigned long TimeOut_FadeColor               = 20;   // 0.02 Seconds    => Effect Speed
unsigned long TimeOut_EffectRainDrop          = 40;   // 0.04 Seconds    => Effect Speed
unsigned long TimeOut_RandomColor             = 4000; // 4.00 Seconds    => Effect Speed
unsigned long TimeOut_RainbowColor            = 40;   // 0.04 Seconds    => Effect Speed
unsigned long TimeOut_DiscoBall               = 40;   // 0.04 Seconds    => Effect Speed
unsigned long TimeOut_Rave                    = 100;  // 0.10 Seconds    => Effect Speed
unsigned long TimeOut_RingRun                 = 40;   // 0.04 Seconds    => Effect Speed
unsigned long TimeOut_DiscoField              = 40;   // 0.04 Seconds    => Effect Speed

//Speical for Show
unsigned long PrevMillis_FPS  = 0;

/*
  unsigned long CurMillis_Example = millis();
  if (CurMillis_Example - PrevMillis_Example >= TimeOut_Example) {
    PrevMillis_Example = CurMillis_Example;
    Serial.println("Without Delay Test Message");
  }
*/

//------------------Variables for Main Control------------------//
//-----RGBCheck
int RGBCheckColor = 0;

//------------------Variables for Status Effects------------------//
//-----StatusEffectNoWifiConnected
int NoWifiCounter = 0;

//-----StatusEffectError
int PosXStatusEffectError = 0;
boolean Error = false;

//-----StatusEffectStartupReady
int PosXStatusEffectStartupReady = 0;
int CounterFinishedStartupSequence = 0;
int hueStartupReady = 0;


//------------------Variables for Light Effects------------------//
//-----FadeBrightness
uint8_t oldBrightness = 0;

//-----FadeColor
double oldColorRed   = 0;
double oldColorGreen = 0;
double oldColorBlue  = 0;

double StepRed = 0;
double StepGreen = 0;
double StepBlue = 0;

uint8_t prevColorRed   = 0;
uint8_t prevColorGreen = 0;
uint8_t prevColorBlue  = 0;

//-----RainDrop
boolean DummyLEDMatrix[matrix_x][matrix_y];

//-----RandomColor
int IndexRainbow = 0;
int LastIndexRainbow = 0;
boolean NextIndex = false;

//-----DiscoBall
int NumberPixels = 15;

//-----DiscoField


//-----Rave
boolean InitRave = true;

//-----RingRung
int PosXEffectRingRun = 0;
int PosYEffectRingRun = 0;

//-----DiscoField
int CounterNewFieldGen = 0;



void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Start Setup");

  Serial.println("Set MQTT Parameter");
  mqtt_Client.setServer(mqtt_server, mqtt_port);
  mqtt_Client.setCallback(callback);
  Serial.println("Finished setting MQTT Parameter");

  Serial.println("Start Wifi Connection");
  reconnect_wifi();
  Serial.println("Finished Wifi Connection");

  Serial.println("Start MQTT Server Connection");
  //reconnect_MqttClient();
  Serial.println("Finished MQTT Server Connection");

  Serial.println("Initialize FastLED");
  strip.Begin();
  //LEDS.setBrightness(100);
  Serial.println("Finished Initialize FastLED");

  Serial.println("Finished Setup");
}

void loop() {
  if (int(NoWifiCounter) >= 10) {
    ESP.restart();
  }

  //---------------------------Network Loop---------------------------//
  mqtt_Client.loop();

  //Reconnect to WiFi if it loses connection
  if (WiFi.status() != WL_CONNECTED) {
    reconnect_wifi();
  } else {
    if (!mqtt_Client.connected()) {
      reconnect_MqttClient();
    }
  }

  //---------------------------Main Loop---------------------------//

  //Print Main Control State and Light State for Debug
#ifdef DEBUG_STATES
  //Main State
  if (MainState != LastMainState) {
    LastMainState = MainState;
    Serial.println("/----Main State----/");
    Serial.print("   Main State : ");
    Serial.println(MainState);
    Serial.println("/------------------/");
  }
  //Light State
  if (LightState != LastLightState) {
    LastLightState = LightState;
    Serial.println("/----Light State----/");
    Serial.print("   Light State : ");
    Serial.println(LightState);
    Serial.println("/-------------------/");
  }
#endif

  //Main Procedure over the Led Ball
  MainControl();
  jsonHandler();
  //Test Section for Effects etc...

}

//---------------------------Main Control---------------------------//
void MainControl() {

  switch (MainState) {

    case 0:
      //Shows the Matrix in Red,Green and Blue to Look for broken Leds
      if (DevMode) {
        MainState = 20;
      } else {
        MainStateRGBCheck = RGBCheck();
        if (MainStateRGBCheck == true) {
          MainState = 10;
        }
      }
      break;

    case 10:
      ResetRGBCheck();
      MainState = 20;
      break;

    case 20:
      InitParameter();
      MainState = 30;
      break;

    case 30:
      //InitPixelArray();
      MainState = 31;
      break;

    case 31:
      InitEffectArrays();
      MainState = 40;
      break;

    case 40:
      if (DevMode) {
        MainState = 50;
      } else {
        MainStateStatusEffectStartupReady = StatusEffectStartupReady();
        if (MainStateStatusEffectStartupReady) {
          MainState = 50;
        }
      }
      delay(3000);
      black();
      break;

    case 50:
      LightControl();
      break;

    default:
      StatusEffectError();
      break;

  }

}

boolean RGBCheck() {
  unsigned long CurMillis_RGBCheck = millis();
  if (CurMillis_RGBCheck - PrevMillis_RGBCheck >= TimeOut_RGBCheck) {
    PrevMillis_RGBCheck = CurMillis_RGBCheck;
    for (int i = 0; i < matrix_x; i++) {
      for (int u = 0; u < matrix_y; u++) {
        switch (RGBCheckColor) {
          //Check RED
          case 0:  leds[i][u] = CRGB(255, 0, 0);
            break;

          //Check Green
          case 1:  leds[i][u] = CRGB(0, 255, 0);
            break;

          //Check Blue
          case 2:  leds[i][u] = CRGB(0, 0, 255);
            break;
        }
      }
    }
    RGBCheckColor++;
    ShowMatrix();
  }
  if (RGBCheckColor >= 4) {
    return true;
  } else {
    return false;
  }
}

void ResetRGBCheck() {
  black();
}

void InitParameter() {
  //Init Parameter
  oldBrightness = Brightness;
  oldColorRed   = Color_Red;
  oldColorGreen = Color_Green;
  oldColorBlue  = Color_Blue;
}


void InitEffectArrays() {

}
//---------------------------End Main Control---------------------------//

//---------------------------Status Effects---------------------------//

void StatusEffectError() {
  unsigned long CurMillis_GeneralError = millis();
  if (CurMillis_GeneralError - PrevMillis_GeneralError >= TimeOut_GeneralError) {
    PrevMillis_GeneralError = CurMillis_GeneralError;
    for (int i = 0; i < matrix_y; i++) {
      leds[PosXStatusEffectError][i] = CRGB(255, 0, 0);
    }
    ShowMatrix();
    fadeall(40);
    if (PosXStatusEffectError >= matrix_x - 1) {
      PosXStatusEffectError = 0;
    } else {
      PosXStatusEffectError++;
    }
  }
}

boolean StatusEffectStartupReady() {
  unsigned long CurMillis_EffectStartupReady = millis();
  if (CurMillis_EffectStartupReady - PrevMillis_EffectStartupReady >= TimeOut_EffectStartupReady) {
    PrevMillis_EffectStartupReady = CurMillis_EffectStartupReady;
    for (int i = 0; i < matrix_y; i++) {
      leds[PosXStatusEffectStartupReady][i] = CHSV(hueStartupReady++, 255, 255);
    }
    ShowMatrix();
    if (CounterFinishedStartupSequence >= 9) {
      for (int i = 0; i < matrix_y; i++) {
        leds[PosXStatusEffectStartupReady][i] = CRGB(0, 0, 0);
      }
      ShowMatrix();
    }
    if (PosXStatusEffectStartupReady >= matrix_x - 1) {
      PosXStatusEffectStartupReady = 0;
      CounterFinishedStartupSequence++;
    } else {
      PosXStatusEffectStartupReady++;
    }
  }
  //Effect Finished
  if (CounterFinishedStartupSequence >= 10) {
    return true;
  } else {
    return false;
  }
}

//---------------------------End Status Effects---------------------------//

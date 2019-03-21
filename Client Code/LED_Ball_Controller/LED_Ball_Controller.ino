//#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>

//Include Secret Header
#include <secrets.h>

/*
  Name          :   Led Ball Controller
  Programmer    :   Nico Weidenfeller
  Created       :   21.02.2019
  Last Modifed  :   20.03.2019  //Not Working
  Version       :   1.1
  Description   :   Controller for a 400-led Disco Ball (size can be changed with the Resolution) with a Resolution of 16 * 25

  ToDoList      :   =>
                    - Add Boundry Check for MQTT Parameter
                    - Disable MQTT Parameter Sync depending on the used Effect to prevent overide
                    - When gamma correction is used fix fade to new color with gamma correction

  Error Help    :   1. If the ball blinks red and black its an Effect Watchdog Error. If this happens an Effect to longer than 10 sec to complete
                    2. If the ball has a green ring going around it. Then the WiFi is disconnected. If this happens the ball will try to restart itself after 3 effect cycles
                    3. If the ball has a red ring going around it. Then its a General Error. If this happens try to restart the ball, and check the States if it happens again.

  Patchnotes    :   Version 1.0
                      Initial Code
                    Version 1.1
                      Overhaul of the State Machines and Code cleanup

  EffectList    :   1. fadeall()              => Fades all pixels to black by an nscale8 number
                    2. black()                => Makes all LEDs black (no brightness change)
                    3. setSolidColor()        => Fills all LEDs with new Color
                    4. setFadeColor()         => Fades all LEDs to new Color
                    5. setSolidBrightness()   => Sets the Brightness to the new value
                    6. setFadeBrightness()    => Fades the Brightness to the new value
                    7. fillSoild()            => Fills the Matrix with a choosen Color
                    8. fadeAllToBlack()       => Fades the Matrix to Black
                    9: fadeAllToPrecent()     => Fades the Matrix to a Percentage

  Main Tap Info :   Owns the status machines of the LED ball and initialize the programm
*/

//Developer mode skips some initializations and blocks startup effects for shorter Startup time.
boolean DevMode = true;

//--- LED Ball Parameters ---//
#define NUM_LEDS 400                      //Number of LEDs from the Ball
#define matrix_x 16                       //Number of LEDs on the X-Axis
#define matrix_y 25                       //Number of LEDs on the Y-Axis

#define LED_PIN 2                         //Data Pin for the LED Matrix
#define CHIPSET WS2812B                   //Chipset of the LEDs
#define COLOR_ORDER RGB                   //Color Order of the LEDs
CRGB ledoutput[NUM_LEDS];                 //1D matrix that represents the real construction of the led strips. Gets pushed out to the Dataline
CRGB leds[matrix_x][matrix_y];            //2D matrix that mimics the given Matrix on the ball, for easier effect programming. Gets converted into the 1D Matrix
boolean activatedGammaCorrection = true;  //Activates or deactivates the gamma correction

//--- Main State Machine ---//
int MainState = 0;                  //Status of the Main State Machine
#define FPS 60                      //FPS with which the LED Matrix get pushed out

//--- Light State Machine ---//
int LightState = 0;                 //Status of the Light State Machine

//Debug Defines
#define DEBUG_NETWORK
//#define DEBUG_STATES


//LED Ball Control Parameter
//Truns the Ball ON / OFF
boolean Power;            //On / Off

//Controls the Color shown on the Ball
boolean RandomColor;      //On / Off
boolean RainbowColor      //On / Off
boolean RandomColorSync   //On / Off
uint8_t Red;              //0 - 255
uint8_t Green;            //0 - 255
uint8_t Blue;             //0 - 255

//Controls the Brightness of the Ball
uint8_t Brightness;       //0 - 255

//Controls the Effect show on the Ball
boolean RandomEffect;     //On / Off
uint8_t EffectSpeed;      //0 - 255
uint8_t FadeSpeed;        //0 - 255
uint8_t EffectNumber;     //0 - 255
uint8_t EffectDirection;  // 8 == Up // 6 == Right // 2 == Down // 4 == Left


//Error Variables /Normal Variables for  WiFi
int CantFindWiFiCounter = 0;
boolean CantFindWiFi = false;
boolean SaveLastMainState = false;
int CantFindWiFiLastMainState;
int CounterToRestartESP = 0; //If Counter is 5 the ESP Will Restart itself


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


/*-------------------------------------- Setup --------------------------------------*/
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Start Setup");
  Serial.println("LED Ball Version 1.0");
  Serial.println("Programmer : Nico Weidenfeller");
  Serial.println("");

  Serial.println("Set MQTT Parameter");
  mqtt_Client.setServer(mqtt_server, mqtt_port);
  mqtt_Client.setCallback(callback);
  Serial.println("Finished setting MQTT Parameter");

  Serial.println("Start Wifi Connection");
  wifi();
  Serial.println("Finished Wifi Connection");

  Serial.println("Initialize FastLED");
  LEDS.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(ledoutput, NUM_LEDS);
  LEDS.setBrightness(100);
  Serial.println("Finished Initialize FastLED");

  Serial.println("Finished Setup");
}


/*-------------------------------------- Loop --------------------------------------*/
void loop() {

  mqtt_Client.loop();

  /*--------------------- Cyclic monitoring and processing ---------------------*/
  //Publishes the LED Matrix at an Set FPS Rate
  unsigned long CurMillis_PublishMatrix = millis();
  if (CurMillis_PublishMatrix - PrevMillis_PublishMatrix >= int(FPS / 1000)) {
    PrevMillis_PublishMatrix = CurMillis_PublishMatrix;
    ShowMatrix();
  }

  //Syncs Main Parameter with the Paramter from MQTT
  syncParameter();

  //Monitoring Wifi Connection
  if (int(NoWifiCounter) >= 10) {
    ESP.restart();
  }

  //Reconnect to WiFi if it loses connection. After that try to connect to MQTT
  if (WiFi.status() != WL_CONNECTED) {
    wifi();
  } else {
    if (!mqtt_Client.connected()) {
      mqtt();
    }
  }

  //Serial Print for Information
  printer();

  //--------------------------- Main State machine of the LED ball ---------------------------//

  switch (MainState) {

    case 0: //RGB Check to spot defect LEDs
      if (DevMode) {
        MainState = 10;
      } else {
        boolean RGBCheckFinished = RGBCheckEffect();
        if (RGBCheckFinished) {
          black();
          MainState = 10;
        }
      }
      break;

    case 10: //Initialization of LED Ball Parameter
      InitParameter();
      MainState = 20;
      break;

    case 20: //Initialization of Arrays who are used in Effects and have to be preloaded
      InitEffectArrays();
      MainState = 30;
      break;

    case 30: //Startup Effect to show the User that the Ball is ready to use
      if (DevMode) {
        MainState = 100;
      } else {
        boolean StartupEffectFinished = StartupReadyEffect();
        if (StartupEffectFinished) {
          MainState = 40;
        }
      }
      break;

    case 40: //Waits 3 Seconds after StartupEffect and Blacks out the Ball
      delay(3000);
      black();
      MainState = 100;
      break;

    case 100: //Goes into the State Machine for the Light Effects
      lightControl();
      break;

    case 999: //Error Mode when no WiFi is Connected
      break;

    default: //General Error State in the State Machine
      GeneralErrorEffect();
      break;

  }

}

//--------------------------- Light State machine of the LED ball ---------------------------//
void lightControl() {

  switch (LightState) {

    case 0:
      break;

    case 10:
      break;

    default:
      break;
  }

}

//--------------------------- Sync Parameter ---------------------------//
void syncParameter() {
  //Syncs the Parameter from MQTT. Depending on the Effect / Mode some Parameter will get ignored
  Power             = mqtt_Power;
  RandomColor       = mqtt_RandomColor;
  RainbowColor      = mqtt_RainbowColor;
  RandomColorSync   = mqtt_RandomColorSync;
  Red               = mqtt_Red;
  Green             = mqtt_Green;
  Blue              = mqtt_Blue;
  Brightness        = mqtt_Brightness;
  RandomEffect      = mqtt_RandomEffect;
  EffectSpeed       = mqtt_EffectSpeed;
  FadeSpeed         = mqtt_FadeSpeed;
  EffectNumber      = mqtt_EffectNumber;
  EffectDirection   = mqtt_EffectDirection;
}

//--------------------------- Init Parameter ---------------------------//
void InitParameter() {
  //Initializing the LED Ball parameters will be overwritten later by the commands of MQTT
  Power             = false;
  RandomColor       = false;
  RainbowColor      = false;
  RandomColorSync   = false;
  Red               = 0;
  Green             = 128;
  Blue              = 255;
  Brightness        = 150;
  RandomEffect      = false;
  EffectSpeed       = 10;
  FadeSpeed         = 10;
  EffectNumber      = 0;
  EffectDirection   = 8;
}

//--------------------------- Init Effect Arrays ---------------------------//
void InitEffectArrays() {

}

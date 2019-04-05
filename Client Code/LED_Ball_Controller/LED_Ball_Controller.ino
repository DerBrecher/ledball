//#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>

//Include Secret Header
#include <secrets.h>

//for Serial print Startup Info
#define Name        "Led Ball Controller"
#define Programmer  "Nico Weidenfeller"
#define Created     "21.02.2019"
#define LastModifed "05.04.2019"
#define Version     "1.1.6"

/*
  Name          :   Led Ball Controller
  Programmer    :   Nico Weidenfeller
  Created       :   21.02.2019
  Last Modifed  :   05.04.2019
  Version       :   1.1.6
  Description   :   Controller for a 400-led Disco Ball (size can be changed with the Resolution) with a Resolution of 16 * 25

  ToDoList      :   =>
                    - Disable MQTT Parameter Sync depending on the used Effect to prevent overide
                    - When gamma8 correction is used fix fade to new color with gamma8 correction

  Error Help    :   1. If the ball has a green ring going around it. Then the WiFi is disconnected. If this happens the ball will try to restart itself after 3 effect cycles
                    2. If the ball has a red ring going around it. Then its a General Error. If this happens try to pick an other effect. If this doesnt change anything
                       try to restart the ball, and check the States if it happens again.

  Patchnotes    :   Version 1.0
                      Initial Code
                    Version 1.1
                      Overhaul of the State Machines and Code cleanup
                    Version 1.1.1
                      Cleanup in the Main and Network Tab. Added Debug Messages for Network in the Information Tab. And some small Bugfixes
                    Version 1.1.2
                      Finished Cleanup of the whole Programm. Fixed some Bugs.
                    Version 1.1.3
                      Fixed MQTT bug with Channel publish crashing the esp. Fixed WiFi and MQTT connection and auto restart
                    Version 1.1.4
                      Fixed direction state and publish command from the color modes for MQTT
                    Version 1.1.5
                      Fixed Brigthness bug, Color Picker bug, Effect Speed Bug and added more Debug options.
                    Version 1.1.6
                      Fixed Status Effect were not displayed.

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

//*************************************************************************************************//
//--------------------------------------------- SHARED --------------------------------------------//
//*************************************************************************************************//

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
boolean activatedGammaCorrection = true;  //Activates or deactivates the gamma8 correction

//--- Main State Machine ---//
int MainState = 0;                  //Status of the Main State Machine
int MainStateMemory = 0;            //Rember last Main State
#define FPS 60                      //FPS with which the LED Matrix get pushed out
unsigned long PrevMillis_PublishMatrix = 0;

//--- Light State Machine ---//
int LightState = 0;                 //Status of the Light State Machine

//Normal Color

//Rainbow Color
int IndexRainbow = 0;

//Random Color
int IndexRandomColor = 0;
int LastIndexRandomColor = 0;

//Random Color Sync

//Color Control State
int ColorControl = 0;

//--- Defines for Debugs ---//
//Will be Serial printed in the Information Tab
#define DEBUG_MAIN_STATE    //Prints Main State
#define DEBUG_LIGHT_STATE   //Prints Light State
//#define DEBUG_COLOR         //Prints the Color Control mode
#define DEBUG_EFFECTS       //Prints the used Effect
//#define DEBUG_NETWORK       //Prints MQTT Parameter

//LED Ball Control Parameter
//Truns the Ball ON / OFF
boolean Power;            //On / Off

//Controls the Color shown on the Ball
boolean RandomColor;      //On / Off
boolean RainbowColor;     //On / Off
boolean RandomColorSync;  //On / Off
uint8_t Red;              //0 - 255
uint8_t Green;            //0 - 255
uint8_t Blue;             //0 - 255

//Controls the Brightness of the Ball
uint8_t Brightness;       //0 - 255

//Controls the Effect show on the Ball
boolean RandomEffect;     //On / Off
uint8_t EffectSpeed;      //0 - 100
uint8_t FadeSpeed;        //0 - 255
uint8_t EffectNumber;     //0 - 255
uint8_t EffectDirection;  // 8 == Up // 6 == Right // 2 == Down // 4 == Left


//Timer / Delay
unsigned long PrevMillis_Example              = 0;
//----Color----//
unsigned long PrevMillis_NormalColor          = 0;
unsigned long PrevMillis_RandomColor          = 0;
unsigned long PrevMillis_RainbowColor         = 0;
unsigned long PrevMillis_RandomColorSync      = 0;
//----Brightness----//
unsigned long PrevMillis_FadeBrightness       = 0;
//----Effects----//
unsigned long PrevMillis_Effect               = 0;    //Timeout is Controlled by EffectSpeed
unsigned long PrevMillis_GeneralError         = 0;
unsigned long PrevMillis_RGBCheck             = 0;
unsigned long PrevMillis_EffectStartupReady   = 0;
unsigned long PrevMillis_NoWiFiConnection     = 0;


unsigned long TimeOut_Example                 = 1000;   // 1.00 Seconds
//----Color----//
unsigned long TimeOut_NormalColor             = 30;     // 0.03 Seconds
unsigned long TimeOut_RandomColor             = 4000;   // 4.00 Seconds
unsigned long TimeOut_RainbowColor            = 30;     // 0.03 Seconds
unsigned long TimeOut_RandomColorSync         = 30;     // 0.03 Seconds
//----Brightness----//
unsigned long TimeOut_FadeBrightness          = 10;     // 0.01 Seconds
//----Effects----//
unsigned long TimeOut_GeneralError            = 50;     // 0.03 Seconds
unsigned long TimeOut_RGBCheck                = 3000;   // 3.00 Seconds
unsigned long TimeOut_EffectStartupReady      = 30;     // 0.03 Seconds
unsigned long TimeOut_NoWiFiConnection        = 50;     // 0.03 Seconds


//Speical for Show
unsigned long PrevMillis_FPS  = 0;  //Timeout is Controlled by FPS/1000

/*
  unsigned long CurMillis_Example = millis();
  if (CurMillis_Example - PrevMillis_Example >= TimeOut_Example) {
    PrevMillis_Example = CurMillis_Example;
    Serial.println("Without Delay Test Message");
  }
*/

//*************************************************************************************************//
//-------------------------------------------- EFFECTS --------------------------------------------//
//*************************************************************************************************//

//--- Fade ---//
//Actual Brightness of the LEDs
uint8_t actualBrightness = 0;

//Actual Color of the LEDs (No Gamma Correction)
double actualColorRed   = 0;
double actualColorGreen = 0;
double actualColorBlue  = 0;

//Needed for Check if the Color has changed between the last cycle
uint8_t prevColorRed    = 0;
uint8_t prevColorGreen  = 0;
uint8_t prevColorBlue   = 0;

//Steps with which  the Color fades
double FadeStepRed      = 0;
double FadeStepGreen    = 0;
double FadeStepBlue     = 0;

//--- RGBCheckEffect ---//
int RGBCheckColor = 0;

//--- GeneralErrorEffect ---//
int PosXStatusEffectError = 0;

//--- StatusEffectStartupReady ---//
int PosXStatusEffectStartupReady = 0;
int CounterFinishedStartupSequence = 0;
int hueStartupReady = 0;

//-----RainDrop
boolean DummyLEDMatrix[matrix_x][matrix_y];

//-----RandomColor
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


//gamma8 Array that replaces the calculate RGB Values in the function ShowMatrix if activated
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

//*************************************************************************************************//
//------------------------------------------ INFORMATION ------------------------------------------//
//*************************************************************************************************//

//Memory from the Parameter
boolean mem_mqtt_Power;

boolean mem_mqtt_RandomColor;
boolean mem_mqtt_RainbowColor;
boolean mem_mqtt_RandomColorSync;
uint8_t mem_mqtt_Red;
uint8_t mem_mqtt_Green;
uint8_t mem_mqtt_Blue;

uint8_t mem_mqtt_Brightness;

boolean mem_mqtt_RandomEffect;
uint8_t mem_mqtt_EffectSpeed;
uint8_t mem_mqtt_FadeSpeed;
uint8_t mem_mqtt_EffectNumber;
uint8_t mem_mqtt_EffectDirection;

//Memory form the Main State
int mem_MainState;

//Memory form the Light State
int mem_LightState;

//memory from the Color Control
int mem_ColorControl;

//memory form the Effect
int mem_EffectNumber;

//*************************************************************************************************//
//-------------------------------------------- NETWORK --------------------------------------------//
//*************************************************************************************************//
WiFiClient    wifiMqtt;
PubSubClient  mqtt_Client(wifiMqtt);
#define mqtt_server       "192.168.99.23"
#define mqtt_username     "pi"
#define mqtt_password     "raspberry"
#define mqtt_port         1883
#define mqtt_client_name  "MumLedBallControllerTest2"

//------------------------------------- Mqtt Paths --------------------------------------//
//Parameter [mqtt_Power]
const char* mqtt_state_Power           = "ledball/state/Power";
const char* mqtt_command_Power         = "ledball/command/Power";

//Parameter [mqtt_RandomColor]
const char* mqtt_state_RandomColor     = "ledball/state/RandomColor";
const char* mqtt_command_RandomColor   = "ledball/command/RandomColor";

//Parameter [boolean mqtt_RainbowColor]
const char* mqtt_state_RainbowColor    = "ledball/state/RainbowColor";
const char* mqtt_command_RainbowColor  = "ledball/command/RainbowColor";

//Parameter [mqtt_RandomColorSync]
const char* mqtt_state_RandomColorSync    = "ledball/state/RandomColorSync";
const char* mqtt_command_RandomColorSync  = "ledball/command/RandomColorSync";

//Parameter [mqtt_Red, mqtt_Green, mqtt_Blue]
const char* mqtt_state_Color          = "ledball/state/Color";
const char* mqtt_command_Color        = "ledball/command/Color";

//Parameter [mqtt_Brightness]
const char* mqtt_state_Brightness     = "ledball/state/Brightness";
const char* mqtt_command_Brightness   = "ledball/command/Brightness";

//Parameter [mqtt_RandomEffect]
const char* mqtt_state_RandomEffect     = "ledball/state/RandomEffect";
const char* mqtt_command_RandomEffect   = "ledball/command/RandomEffect";

//Parameter [mqtt_EffectSpeed]
const char* mqtt_state_EffectSpeed     = "ledball/state/EffectSpeed";
const char* mqtt_command_EffectSpeed   = "ledball/command/EffectSpeed";

//Parameter [mqtt_FadeSpeed]
const char* mqtt_state_FadeSpeed     = "ledball/state/FadeSpeed";
const char* mqtt_command_FadeSpeed   = "ledball/command/FadeSpeed";

//Parameter [mqtt_EffectNumber]
const char* mqtt_state_EffectNumber     = "ledball/state/EffectNumber";
const char* mqtt_command_EffectNumber   = "ledball/command/EffectNumber";

//Parameter [mqtt_EffectDirection]
const char* mqtt_state_EffectDirection     = "ledball/state/EffectDirection";
const char* mqtt_command_EffectDirection   = "ledball/command/EffectDirection";

//---- LED Ball Control Parameter from MQTT ----//
/*
   Mirror image of the real parameters. Are only used to
   prevent confusion of origin (personal programming preferenc)
*/
boolean mqtt_Power;

boolean mqtt_RandomColor;
boolean mqtt_RainbowColor;
boolean mqtt_RandomColorSync;
uint8_t mqtt_Red;
uint8_t mqtt_Green;
uint8_t mqtt_Blue;

uint8_t mqtt_Brightness;

boolean mqtt_RandomEffect;
uint8_t mqtt_EffectSpeed;
uint8_t mqtt_FadeSpeed;
uint8_t mqtt_EffectNumber;
uint8_t mqtt_EffectDirection;


//WiFi
boolean StartWifiConnection = true;
boolean ShowNoWifiEffect    = false;
int NoWifiCounter           = 0;

//Mqtt
boolean NewData = false;

//Timer / Delay
unsigned long PrevMillis_NoWiFiCounter  = 0;

unsigned long TimeOut_NoWiFiCounter     = 300; // 0.50 Seconds

//*************************************************************************************************//
//----------------------------------------- START PROGRAMM ----------------------------------------//
//*************************************************************************************************//

/*-------------------------------------- Setup --------------------------------------*/
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("Name          : ");
  Serial.println(Name);
  Serial.print("Programmer    : ");
  Serial.println(Programmer);
  Serial.print("Created       : ");
  Serial.println(Created);
  Serial.print("Last Modifed  : ");
  Serial.println(LastModifed);
  Serial.print("Version       : ");
  Serial.println(Version);
  Serial.println("");

  Serial.println("Start Setup");

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

  //Monitoring Wifi Connection. If it doesnt reconnect in 300ms * 10 then restart ESP
  //Reconnect to WiFi if it loses connection. After that try to connect to MQTT
  if (!mqtt_Client.connected()) {
    mqtt();
  }

  if (WiFi.status() != WL_CONNECTED) {
    wifi();
    if (MainState != 999) {
      MainStateMemory = MainState;
      MainState = 999;
    }
  }
  if (WiFi.status() == WL_CONNECTED and MainState == 999) {
    MainState = MainStateMemory;
  }

  //Serial Print for Information and Diagnose
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

    case 10: //Initialization of the MQTT Parameter
      InitMqttParameter();
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
      //Syncs Main Parameter with the Paramter from MQTT
      syncParameter();
      //Light Control
      lightControl();
      break;

    case 999: //Error Mode when no WiFi is Connected
      noWifiConnection();
      break;

    default: //General Error State in the State Machine
      GeneralErrorEffect();
      break;

  }

}

//--------------------------- Light State machine of the LED ball ---------------------------//
void lightControl() {

  //Decides wich Color Effect is used
  if (not RandomColor && not RainbowColor && not RandomColorSync) {
    //Normal Color
    ColorPickerNormal();
    ColorControl = 0;
  }
  if (RandomColor && not RainbowColor && not RandomColorSync) {
    //Random Color
    ColorPickerRandom();
    ColorControl = 1;
  }
  if (not RandomColor && RainbowColor && not RandomColorSync) {
    //Rainbow Color
    ColorPickerRainbow();
    ColorControl = 2;
  }
  if (not RandomColor && not RainbowColor &&  RandomColorSync) {
    //Random Color Sync
    ColorPickerRandomSync();
    ColorControl = 3;
  }

  //Decides wich Effect is used
  if (Power) {
    if (RandomEffect) {
      LightState = 0;
    } else {
      LightState = 20;
    }
  } else {
    LightState = 30;
  }


  //Light State machine
  switch (LightState) {

    case 10: //Random Effect Picker

      break;

    case 20: //Single Effect Picker
      switch (EffectNumber) {

        case 0: //Effect Black
          black();
          break;

        case 1: //Effect Fill Solid
          fillSolid();
          break;

        case 2: //Effect Rain Drop
          RainDrop();
          break;

        case 3: //Effect Ring Run
          RingRun();
          break;

        case 4: //Effect Disco Ball
          DiscoBall();
          break;

        case 5: //Effect Disco Field
          DiscoField();
          break;

        case 6: //Effect Rave
          Rave();
          break;

        case 7: //Effect Equalizer
          Equalizer();
          break;

        default: //Effect Status Effect Error
          GeneralErrorEffect();
          break;

      }
      break;

    case 30: //No Power State
      fadeall(40);
      break;

    default: //Default fade all
      fadeall(40);
      break;
  }

}

//--------------------------- Normal Color ---------------------------//
void ColorPickerNormal() {
  //Normal COlor Picker
  Red = mqtt_Red;
  Green = mqtt_Green;
  Blue = mqtt_Blue;
  //Fade Color
  FadeColor();
  //Fade Brightness
  FadeBrightness();
}


//--------------------------- Random Color ---------------------------//
void ColorPickerRandom() {
  //Random Color Picker
  unsigned long CurMillis_RandomColor = millis();
  if (CurMillis_RandomColor - PrevMillis_RandomColor >= TimeOut_RandomColor) {
    PrevMillis_RandomColor = CurMillis_RandomColor;
    while (true) {
      IndexRandomColor = int(random(0, 5));
      if (IndexRandomColor != LastIndexRandomColor) {
        break;
      }
    }
    LastIndexRandomColor = IndexRandomColor;
  }
  switch (IndexRandomColor) {

    case 0:
      Red   = 255;
      Green = 0;
      Blue  = 0;
      FadeColor();
      break;

    case 1:
      Red   = 255;
      Green = 255;
      Blue  = 0;
      FadeColor();
      break;

    case 2:
      Red   = 0;
      Green = 255;
      Blue  = 0;
      FadeColor();
      break;

    case 3:
      Red   = 0;
      Green = 255;
      Blue  = 255;
      FadeColor();
      break;

    case 4:
      Red   = 0;
      Green = 0;
      Blue  = 255;
      FadeColor();
      break;

    case 5:
      Red   = 255;
      Green = 0;
      Blue  = 255;
      FadeColor();
      break;

    default:
      Red   = 0;
      Green = 0;
      Blue  = 0;
      FadeColor();
      break;

  }
  //Fade Brightness
  FadeBrightness();
}


//--------------------------- Rainbow Color ---------------------------//
void ColorPickerRainbow() {
  //Rainbow procedure
  switch (IndexRainbow) {
    case 0:
      Red   = 255;
      Green = 0;
      Blue  = 0;
      NextIndex = FadeColor();
      if (NextIndex) {
        IndexRainbow = 1;
      }
      break;
    case 1:
      Red   = 255;
      Green = 255;
      Blue  = 0;
      NextIndex = FadeColor();
      if (NextIndex) {
        IndexRainbow = 2;
      }
      break;
    case 2:
      Red   = 0;
      Green = 255;
      Blue  = 0;
      NextIndex = FadeColor();
      if (NextIndex) {
        IndexRainbow = 3;
      }
      break;
    case 3:
      Red   = 0;
      Green = 255;
      Blue  = 255;
      NextIndex = FadeColor();
      if (NextIndex) {
        IndexRainbow = 4;
      }
      break;
    case 4:
      Red   = 0;
      Green = 0;
      Blue  = 255;
      NextIndex = FadeColor();
      if (NextIndex) {
        IndexRainbow = 5;
      }
      break;
    case 5:
      Red   = 255;
      Green = 0;
      Blue  = 255;
      NextIndex = FadeColor();
      if (NextIndex) {
        IndexRainbow = 0;
      }
      break;
  }
  //Fade Brightness
  FadeBrightness();
}


//--------------------------- Rainbow Color ---------------------------//
void ColorPickerRandomSync() {

  //Fade Brightness
  FadeBrightness();
}


//--------------------------- Sync Parameter ---------------------------//
void syncParameter() {
  //Syncs the Parameter from MQTT. Depending on the Effect / Mode some Parameter will get ignored
  Power             = mqtt_Power;
  RandomColor       = mqtt_RandomColor;
  RainbowColor      = mqtt_RainbowColor;
  RandomColorSync   = mqtt_RandomColorSync;
  //Red             = mqtt_Red;   //
  //Green           = mqtt_Green; //Gets set in Color Methodes
  //Blue            = mqtt_Blue;  //
  Brightness        = mqtt_Brightness;
  RandomEffect      = mqtt_RandomEffect;
  EffectSpeed       = mqtt_EffectSpeed;
  FadeSpeed         = mqtt_FadeSpeed;
  EffectNumber      = mqtt_EffectNumber;
  EffectDirection   = mqtt_EffectDirection;
}

//--------------------------- Init MQTT Parameter ---------------------------//
void InitMqttParameter() {
  //Initializing the MQTT LED Ball parameters will later be overwritten
  mqtt_Power            = false;
  mqtt_RandomColor      = false;
  mqtt_RainbowColor     = false;
  mqtt_RandomColorSync  = false;
  mqtt_Red              = 0;
  mqtt_Green            = 128;
  mqtt_Blue             = 255;
  mqtt_Brightness       = 200;
  mqtt_RandomEffect     = false;
  mqtt_EffectSpeed      = 10;
  mqtt_FadeSpeed        = 10;
  mqtt_EffectNumber     = 0;
  mqtt_EffectDirection  = 8;
}

//--------------------------- Init Effect Arrays ---------------------------//
void InitEffectArrays() {

}

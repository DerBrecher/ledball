//#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>

//Include Secret Header
#define LedBall1      //Define used MQTT Parameters
#include <secrets.h>


//for Serial print Startup Info
#define Name        "Led Ball Controller"
#define Programmer  "Nico Weidenfeller"
#define Created     "21.02.2019"
#define LastModifed "28.05.2019"
#define Version     "1.1.11"

/*
  Name          :   Led Ball Controller
  Programmer    :   Nico Weidenfeller
  Created       :   21.02.2019
  Last Modifed  :   28.05.2019
  Version       :   1.1.11
  Description   :   Controller for a 400-led Disco Ball (size can be changed with the Resolution) with a Resolution of 16 * 25

  ToDoList      :   =>
                    - When the direction is used, the speed of the effects must be adjusted according to the direction. Effect speeds are time based and not pixel lenght based
                    - When gamma8 correction is used fix fade to new color with gamma8 correction
                    - Delete Init MQTT Paramter. Gets read from the EEPROM

  Bugs          :   - Color Picker goes to white after time.

  Optimize      :   1. Put MQTT Paths in Secret Header into an Array and add an Array for the Recived Data

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
                    Version 1.1.7
                      Added RandomColorSync and added Direction Option to some Effects. Random Color Sync only Supports some of the Effects. If the Effect not supports it, the controller will automatically use the normal color picker
                    Version 1.1.8
                      Added Heartbeat every 5sec. Added Bounce, Flash Effects.
                    Version 1.1.9
                      Moved MQTT Settings and Paths to Secret Header for future use. Added some New Effects and added Effect Message to Effect_Number. Fixed bug with Color Picker Random Color Sync.
                    Version 1.1.10
                      Added new Color Picker "Filter Color Multi". Supports Normal Fade to Color and Color Sync. Added saving the Settings of the Ball to the EEPROM of the ESP and loading it on the startup
                    Version 1.1.11
                      Finished implementation of MQTT Paths etc for "Filter Color Multi"

  EffectList    :   1. fadeall()              => Fades all pixels to black by an nscale8 number
                    2. black()                => Makes all LEDs black (no brightness change)
                    3. ShowMatrix()           => Pushes the Matrix to the LED Ball and shows it
                    4. FadeColor()            => Fades actual Color to new Color
                    5. FadeBrightness()       => Fades actual Brightness to new Brightness
                    7. fillSoild()            => Fills the Matrix with a choosen Color
                    8. ShiftArrayToLeft()     => Shifts the Rows of the Matrix by 1 to the Left
                    9: ShiftArrayToRight()    => Shifts the Rows of the Matrix by 1 to the Right

  Main Tap Info :   Owns the status machines of the LED ball and initialize the programm
*/

//*************************************************************************************************//
//--------------------------------------------- SHARED --------------------------------------------//
//*************************************************************************************************//

//Developer mode skips some initializations and blocks startup effects for shorter Startup time.
boolean DevMode = true;
boolean DevFilter = true;

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
String LedBallName = "LED_Ball_1";

//--- Main State Machine ---//
int MainState = 0;                  //Status of the Main State Machine
int MainStateMemory = 0;            //Rember last Main State
#define FPS 60                      //FPS with which the LED Matrix get pushed out
unsigned long PrevMillis_PublishMatrix = 0;

//--- Light State Machine ---//
int LightState = 0;                 //Status of the Light State Machine

//--- HeartBeat ---//
unsigned long HeartBeatCounter = 0;

//------------ EEPROM Settings ------------//
//Actual Size 26 Bytes. Date 21.04.2019
int EEPROM_Size = 32; //Size for the saving of the Settings in the EEPROM (BYTE)
boolean EEPROM_NewDataToSave = false;
boolean EEPROM_EspNewBoot = true;
//------------ End EEPROM Settings ------------//

//Normal Color

//Rainbow Color
int IndexRainbow = 0;

//Random Color
boolean NextIndex = false;
int IndexRandomColor = 0;
int LastIndexRandomColor = 0;

//Random Color Sync
int IndexRandomColorSync = 0;
int LastIndexRandomColorSync = 0;
boolean memEffectFinsihed = false;
boolean NextColor = false;
boolean SendNotSupported = false;
boolean SendOnlySupported = false;

//Filter Color Multi
boolean Filter1Active = false;
boolean Filter2Active = false;
boolean Filter3Active = false;
boolean Filter4Active = false;

//Color Control State
int ColorControl = 0;

//--- Defines for Debugs ---//
//Will be Serial printed in the Information Tab
#define DEBUG_MAIN_STATE    //Prints Main State
#define DEBUG_LIGHT_STATE   //Prints Light State
#define DEBUG_COLOR         //Prints the Color Control mode
#define DEBUG_EFFECTS       //Prints the used Effect
#define DEBUG_NETWORK       //Prints MQTT Parameter

//LED Ball Control Parameter
//Truns the Ball ON / OFF
boolean Power;            //On / Off

//Controls the Color shown on the Ball
boolean RandomColor;      //On / Off
boolean RainbowColor;     //On / Off
boolean RandomColorSync;  //On / Off
boolean FilterColorMulti; //On / Off
byte Filter1Red;       //0 - 255
byte Filter1Green;     //0 - 255
byte Filter1Blue;      //0 - 255
byte Filter2Red;       //0 - 255
byte Filter2Green;     //0 - 255
byte Filter2Blue;      //0 - 255
byte Filter3Red;       //0 - 255
byte Filter3Green;     //0 - 255
byte Filter3Blue;      //0 - 255
byte Filter4Red;       //0 - 255
byte Filter4Green;     //0 - 255
byte Filter4Blue;      //0 - 255
byte Red;              //0 - 255
byte Green;            //0 - 255
byte Blue;             //0 - 255

//Controls the Brightness of the Ball
byte Brightness;       //0 - 255

//Controls the Effect show on the Ball
boolean RandomEffect;  //On / Off
byte EffectSpeed;      //0 - 100
byte FadeSpeed;        //0 - 255
byte EffectNumber;     //0 - 255
byte EffectDirection;  // 8 == Up // 6 == Right // 2 == Down // 4 == Left

//Secret Control for the Random Effect
boolean AllowFade = false;   //On / Off

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
//----HeartBeat----//
unsigned long PrevMillis_HeartBeat            = 0;


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
//----HeartBeat----//
unsigned long TimeOut_HeartBeat               = 5000;   // 5.00 Seconds


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

//-----DiscoBall
int NumberPixels = 15;

//-----Rave
boolean InitRave = true;

//-----RingRung
int PosXEffectRingRun = 0;
int PosYEffectRingRun = 0;

//-----DiscoField
int CounterNewFieldGen = 0;

//-----Equalizer
int RandomPosYEqualizer[matrix_x];

//-----SingleBounce
boolean SwapSingleBounce        = false;
int PosYEffectSingleBounce  = 0;
int PosXEffectSingleBounce  = 0;

//-----DoubleBounce
boolean SwapDoubleBounce        = false;
int PosYEffectDoubleBounce  = 0;
int PosXEffectDoubleBounce  = 0;

//-----FullFlash

//----HalfFlash
boolean SwapHalfFlash = false;
boolean nextHalfFlash;

//----QuarterFlash
int FlashSection = 0;
boolean nextQuarterFlash;

//----EighthFlash
int XFlashSection = 0;
int YFlashSection = 0;
boolean nextEighthFlash;

//----Circus
boolean NextLine;
boolean FadeFinishedCircus;

//----Matrix
boolean SwapMatrix;
int YMatrixDirection = 0;

//----LoopSnake
boolean SwapLoopSnake;
int XLoopSnakeDirection = 0;
int YLoopSnakeDirection = 0;

//----HappyBirthday
int HappyBirthdayPosX = 0;
int Happy_Birthday_Array[12][100] = {
  { 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1 },
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0},
  { 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0},
  { 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  { 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}
};

//----HappNewYear


//----SpiralSnake
int XSpiralSnakeDirection = 0;
int YSpiralSnakeDirection = 0;

//----WaveRefresh
int XPosWaveRefresh = 0;
int YPosWaveRefresh = 0;

//----Rotor
int XPositionRotor = 0;

//----Flash
int SwapFlash = false;

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
boolean mem_mqtt_FilterColorMulti;
uint8_t mem_mqtt_Filter1Red;
uint8_t mem_mqtt_Filter1Green;
uint8_t mem_mqtt_Filter1Blue;
uint8_t mem_mqtt_Filter2Red;
uint8_t mem_mqtt_Filter2Green;
uint8_t mem_mqtt_Filter2Blue;
uint8_t mem_mqtt_Filter3Red;
uint8_t mem_mqtt_Filter3Green;
uint8_t mem_mqtt_Filter3Blue;
uint8_t mem_mqtt_Filter4Red;
uint8_t mem_mqtt_Filter4Green;
uint8_t mem_mqtt_Filter4Blue;
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

//---- LED Ball Control Parameter from MQTT ----//
/*
   Mirror image of the real parameters. Are only used to
   prevent confusion of origin (personal programming preferenc)
*/
boolean mqtt_Power;

boolean mqtt_RandomColor;
boolean mqtt_RainbowColor;
boolean mqtt_RandomColorSync;
boolean mqtt_FilterColorMulti;
byte    mqtt_Filter1Red;
byte    mqtt_Filter1Green;
byte    mqtt_Filter1Blue;
byte    mqtt_Filter2Red;
byte    mqtt_Filter2Green;
byte    mqtt_Filter2Blue;
byte    mqtt_Filter3Red;
byte    mqtt_Filter3Green;
byte    mqtt_Filter3Blue;
byte    mqtt_Filter4Red;
byte    mqtt_Filter4Green;
byte    mqtt_Filter4Blue;
byte    mqtt_Red;
byte    mqtt_Green;
byte    mqtt_Blue;

byte    mqtt_Brightness;

boolean mqtt_RandomEffect;
byte    mqtt_EffectSpeed;
byte    mqtt_FadeSpeed;
byte    mqtt_EffectNumber;
byte    mqtt_EffectDirection;


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

  Serial.println("Initialize EEPROM");
  EEPROM.begin(EEPROM_Size);
  Serial.println("Finihsed Initialization EEPROM");

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
  //--HeartBeat--//
  HeartBeat();

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
      MainState = 25;
      break;

    case 25: //Load Settings from EEPROM
      if (mqtt_Client.connected() and WiFi.status() == WL_CONNECTED) {
        LoadSettings();
        MainState = 30;
      }
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
      //Saves New Parameter Changes to the EEPROM
      SaveSettings();
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
  if (not RandomColor && not RainbowColor && not RandomColorSync && not FilterColorMulti) {
    //Normal Color
    ColorPickerNormal();
    ColorControl = 0;
  }
  if (RandomColor && not RainbowColor && not RandomColorSync && not FilterColorMulti) {
    //Random Color
    ColorPickerRandom();
    ColorControl = 1;
  }
  if (not RandomColor && RainbowColor && not RandomColorSync && not FilterColorMulti) {
    //Rainbow Color
    ColorPickerRainbow();
    ColorControl = 2;
  }
  if (not RandomColor && not RainbowColor &&  RandomColorSync && not FilterColorMulti) {
    //Random Color Sync
    ColorPickerRandomSync(NextColor);
    //Reset NextColor
    NextColor = false;
    ColorControl = 3;
  }
  if (not RandomColor && not RainbowColor && not RandomColorSync && FilterColorMulti) {
    //Filter Color Multi
    ColorPickerFilterColorMulti(NextColor);
    //Reset NextColor
    ColorControl = 4;
  }

  //Decides wich Effect is used
  if (Power) {
    if (RandomEffect) {
      LightState = 10;
    } else {
      LightState = 20;
    }
  } else {
    LightState = 30;
  }

  //Light State machine
  switch (LightState) {

    case 10: //Random Effect Picker
      RandomEffectPicker();
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

        case 8: //SingleBounce
          SingleBounce();
          break;

        case 9: //DoubleBounce
          DoubleBounce();
          break;

        case 10: //FullFlash
          FullFlash();
          break;

        case 11: //HalfFlash
          HalfFlash();
          break;

        case 12: //QuarterFlash
          QuarterFlash();
          break;

        case 13: //EighthFlash
          EighthFlash();
          break;

        case 14: //Circus
          Circus();
          break;

        case 15: //Matrix
          Matrix();
          break;

        case 16: //LoopSnake
          LoopSnake();
          break;

        case 17: //HappyBirthday
          //Only Works with Matrix of 16 * 25
          if (matrix_x == 16 && matrix_y == 25) {
            HappyBirthday();
          } else {
            GeneralErrorEffect();
          }
          break;

        case 18: //HappyNewYear
          //Only Works with Matrix of 16 * 25
          if (matrix_x == 16 && matrix_y == 25) {
            HappyNewYear();
          } else {
            GeneralErrorEffect();
          }
          break;

        case 19: //SpiralSnake
          SpiralSnake();
          break;

        case 20: //WaveRefresh
          WaveRefresh();
          break;

        case 21: //Rotor
          Rotor();
          break;

        case 22: //Flash
          Flash();
          break;

        default: //Effect Status Effect Error
          GeneralErrorEffect();
          break;

      }
      //Reset Rave Init if another Effect is called
      if (EffectNumber != 6) {
        InitRave = true;
      }

      break;

    case 30: //No Power State
      fadeall(230);
      break;

    default: //Default fade all
      fadeall(230);
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


//--------------------------- Random Color Sync ---------------------------//
void ColorPickerRandomSync(boolean EffectFinsihed) {
  //!Warning! this Color Picker is not Supported by all Effects//

  //Check for a rising edge at the var EffectFinished
  if (not memEffectFinsihed and EffectFinsihed) {
    //Create New Color

    while (true) {
      IndexRandomColorSync = int(random(0, 5));
      if (IndexRandomColorSync != LastIndexRandomColorSync) {
        break;
      }
    }
    LastIndexRandomColorSync = IndexRandomColorSync;

    switch (IndexRandomColorSync) {

      case 0:
        actualColorRed   = 255;
        actualColorGreen = 0;
        actualColorBlue  = 0;
        break;

      case 1:
        actualColorRed   = 255;
        actualColorGreen = 255;
        actualColorBlue  = 0;
        break;

      case 2:
        actualColorRed   = 0;
        actualColorGreen = 255;
        actualColorBlue  = 0;
        break;

      case 3:
        actualColorRed   = 0;
        actualColorGreen = 255;
        actualColorBlue  = 255;
        break;

      case 4:
        actualColorRed   = 0;
        actualColorGreen = 0;
        actualColorBlue  = 255;
        break;

      case 5:
        actualColorRed   = 255;
        actualColorGreen = 0;
        actualColorBlue  = 255;
        break;

      default:
        actualColorRed   = 0;
        actualColorGreen = 0;
        actualColorBlue  = 0;
        break;

    }
  }

  //Fade Brightness
  FadeBrightness();

  memEffectFinsihed = EffectFinsihed;
}

//--------------------------- Random Color Sync ---------------------------//
void ColorPickerFilterColorMulti(boolean EffectFinsihed) {


}

//--------------------------- Reset Color Picker to Normal ---------------------------//
void ColorPickerRandomSyncNotSupported() {
  if (SendNotSupported) {
    //Reset MQTT Switch
    mqtt_Client.publish(mqtt_command_RandomColorSync, "0"); //Turn of Random Color Sync
    SendNotSupported = false;
  }
  //Reset Light Control switch
  RandomColorSync = false;
  mqtt_RandomColorSync = false;
}

//------------------------- Set Color Picker to Random Color Sync -------------------------//
void ColorPickerRandomSyncOnlySupported() {
  if (SendOnlySupported) {
    //Reset MQTT Switch
    mqtt_Client.publish(mqtt_command_RandomColorSync, "1"); //Turn on Random Color Sync
    mqtt_Client.publish(mqtt_command_RandomColor, "0"); //Turn of Random Color
    mqtt_Client.publish(mqtt_command_RainbowColor, "0"); //Turn of Rainbow
    SendOnlySupported = false;
  }
  //Reset Light Control switch
  RandomColorSync = true;
  RandomColor     = false;
  RainbowColor    = false;
  mqtt_RandomColorSync = true;
  mqtt_RandomColor     = false;
  mqtt_RainbowColor    = false;
}


//--------------------------- Sync Parameter ---------------------------//
void syncParameter() {
  //Syncs the Parameter from MQTT. Depending on the Effect / Mode some Parameter will get ignored
  Power             = mqtt_Power;
  RandomColor       = mqtt_RandomColor;
  RainbowColor      = mqtt_RainbowColor;
  RandomColorSync   = mqtt_RandomColorSync;
  FilterColorMulti  = mqtt_FilterColorMulti;
  Filter1Red        = mqtt_Filter1Red;
  Filter1Green      = mqtt_Filter1Green;
  Filter1Blue       = mqtt_Filter1Blue;
  Filter2Red        = mqtt_Filter2Red;
  Filter2Green      = mqtt_Filter2Green;
  Filter2Blue       = mqtt_Filter2Blue;
  Filter3Red        = mqtt_Filter3Red;
  Filter3Green      = mqtt_Filter3Green;
  Filter3Blue       = mqtt_Filter3Blue;
  Filter4Red        = mqtt_Filter4Red;
  Filter4Green      = mqtt_Filter4Green;
  Filter4Blue       = mqtt_Filter4Blue;
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
  mqtt_FilterColorMulti = false;
  mqtt_Filter1Red       = 0;
  mqtt_Filter1Green     = 0;
  mqtt_Filter1Blue      = 0;
  mqtt_Filter2Red       = 0;
  mqtt_Filter2Green     = 0;
  mqtt_Filter2Blue      = 0;
  mqtt_Filter3Red       = 0;
  mqtt_Filter3Green     = 0;
  mqtt_Filter3Blue      = 0;
  mqtt_Filter4Red       = 0;
  mqtt_Filter4Green     = 0;
  mqtt_Filter4Blue      = 0;
  mqtt_Red              = 0;
  mqtt_Green            = 128;
  mqtt_Blue             = 255;
  mqtt_Brightness       = 200;
  mqtt_RandomEffect     = false;
  mqtt_EffectSpeed      = 10;
  mqtt_FadeSpeed        = 10;
  mqtt_EffectNumber     = 0;
  mqtt_EffectDirection  = 8;

  //Dev Mode for Filter
  if (DevFilter) {
    //Filter 1
    mqtt_Filter1Red       = 255;
    mqtt_Filter1Green     = 0;
    mqtt_Filter1Blue      = 0;
    Filter1Active         = true;
    //Filter 2
    mqtt_Filter2Red       = 255;
    mqtt_Filter2Green     = 128;
    mqtt_Filter2Blue      = 0;
    Filter2Active         = true;
    //Filter 3
    mqtt_Filter3Red       = 0;
    mqtt_Filter3Green     = 0;
    mqtt_Filter3Blue      = 0;
    Filter3Active         = false;
    //Filter 4
    mqtt_Filter4Red       = 0;
    mqtt_Filter4Green     = 0;
    mqtt_Filter4Blue      = 0;
    Filter4Active         = false;
  }
}

//--------------------------- Init Effect Arrays ---------------------------//
void InitEffectArrays() {

}

//--------------------------- Save Settings ---------------------------//
void SaveSettings() {
  if (EEPROM_NewDataToSave) {
    //Save Settings to the EEPROM
    EEPROM.write(0 , Power);
    EEPROM.write(1 , RandomColor);
    EEPROM.write(2 , RainbowColor);
    EEPROM.write(3 , RandomColorSync);
    EEPROM.write(4 , FilterColorMulti);
    EEPROM.write(5 , Filter1Red);
    EEPROM.write(6 , Filter1Green);
    EEPROM.write(7 , Filter1Blue);
    EEPROM.write(8 , Filter2Red);
    EEPROM.write(9 , Filter2Green);
    EEPROM.write(10, Filter2Blue);
    EEPROM.write(11, Filter3Red);
    EEPROM.write(12, Filter3Green);
    EEPROM.write(13, Filter3Blue);
    EEPROM.write(14, Filter4Red);
    EEPROM.write(15, Filter4Green);
    EEPROM.write(16, Filter4Blue);
    EEPROM.write(17, Red);
    EEPROM.write(18, Green);
    EEPROM.write(19, Blue);
    EEPROM.write(20, Brightness);
    EEPROM.write(21, RandomEffect);
    EEPROM.write(22, EffectSpeed);
    EEPROM.write(23, FadeSpeed);
    EEPROM.write(24, EffectNumber);
    EEPROM.write(25, EffectDirection);

    EEPROM.commit();
    delay(10);
    EEPROM_NewDataToSave = false;
  }
}

//--------------------------- Load Settings ---------------------------//
void LoadSettings() {
  if (EEPROM_EspNewBoot) {
    //Load Settings from EEPROM
    mqtt_Power             = EEPROM.read(0);
    mqtt_RandomColor       = EEPROM.read(1);
    mqtt_RainbowColor      = EEPROM.read(2);
    mqtt_RandomColorSync   = EEPROM.read(3);
    mqtt_FilterColorMulti  = EEPROM.read(4);
    mqtt_Filter1Red        = EEPROM.read(5);
    mqtt_Filter1Green      = EEPROM.read(6);
    mqtt_Filter1Blue       = EEPROM.read(7);
    mqtt_Filter2Red        = EEPROM.read(8);
    mqtt_Filter2Green      = EEPROM.read(9);
    mqtt_Filter2Blue       = EEPROM.read(10);
    mqtt_Filter3Red        = EEPROM.read(11);
    mqtt_Filter3Green      = EEPROM.read(12);
    mqtt_Filter3Blue       = EEPROM.read(13);
    mqtt_Filter4Red        = EEPROM.read(14);
    mqtt_Filter4Green      = EEPROM.read(15);
    mqtt_Filter4Blue       = EEPROM.read(16);
    mqtt_Red               = EEPROM.read(17);
    mqtt_Green             = EEPROM.read(18);
    mqtt_Blue              = EEPROM.read(19);
    mqtt_Brightness        = EEPROM.read(20);
    mqtt_RandomEffect      = EEPROM.read(21);
    mqtt_EffectSpeed       = EEPROM.read(22);
    mqtt_FadeSpeed         = EEPROM.read(23);
    mqtt_EffectNumber      = EEPROM.read(24);
    mqtt_EffectDirection   = EEPROM.read(25);

    //Publish Settings to MQTT
    //Send last Power state
    if (mqtt_Power) {
      mqtt_Client.publish(mqtt_state_Power, "ON");
    } else {
      mqtt_Client.publish(mqtt_state_Power, "OFF");
    }
    delay(10);

    char message[16];

    //Color Picker
    sprintf(message, "%d", mqtt_RandomColor);
    mqtt_Client.publish(mqtt_state_RandomColor, message);
    delay(10);
    sprintf(message, "%d", mqtt_RainbowColor);
    mqtt_Client.publish(mqtt_state_RainbowColor, message);
    delay(10);
    sprintf(message, "%d", mqtt_RandomColorSync);
    mqtt_Client.publish(mqtt_state_RandomColorSync, message);
    delay(10);
    sprintf(message, "%d", mqtt_FilterColorMulti);
    mqtt_Client.publish(mqtt_state_FilterColorMulti, message);
    delay(10);

    //Filter
    sprintf(message, "%d,%d,%d", mqtt_Filter1Red, mqtt_Filter1Green, mqtt_Filter1Blue);
    mqtt_Client.publish(mqtt_state_Filter1, message);
    delay(10);
    sprintf(message, "%d,%d,%d", mqtt_Filter2Red, mqtt_Filter2Green, mqtt_Filter2Blue);
    mqtt_Client.publish(mqtt_state_Filter2, message);
    delay(10);
    sprintf(message, "%d,%d,%d", mqtt_Filter3Red, mqtt_Filter3Green, mqtt_Filter3Blue);
    mqtt_Client.publish(mqtt_state_Filter3, message);
    delay(10);
    sprintf(message, "%d,%d,%d", mqtt_Filter4Red, mqtt_Filter4Green, mqtt_Filter4Blue);
    mqtt_Client.publish(mqtt_state_Filter4, message);
    delay(10);

    //Color
    sprintf(message, "%d,%d,%d", mqtt_Red, mqtt_Green, mqtt_Blue);
    mqtt_Client.publish(mqtt_state_Color, message);
    delay(10);

    //Others
    sprintf(message, "%d", mqtt_Brightness);
    mqtt_Client.publish(mqtt_state_Brightness, message);
    delay(10);
    sprintf(message, "%d", mqtt_RandomEffect);
    mqtt_Client.publish(mqtt_state_RandomEffect, message);
    delay(10);
    sprintf(message, "%d", mqtt_EffectSpeed);
    mqtt_Client.publish(mqtt_state_EffectSpeed, message);
    delay(10);
    sprintf(message, "%d", mqtt_FadeSpeed);
    mqtt_Client.publish(mqtt_state_FadeSpeed, message);
    delay(10);
    sprintf(message, "%d", mqtt_EffectNumber);
    mqtt_Client.publish(mqtt_state_EffectNumber, message);
    delay(10);
    sprintf(message, "%d", mqtt_EffectDirection);
    mqtt_Client.publish(mqtt_state_EffectDirection, message);
    delay(10);

    EEPROM_EspNewBoot = false;
  }
}

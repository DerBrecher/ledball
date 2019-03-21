/*
  Information about Tap : Controls Wifi Connection and receives and process Commands
*/

//Variable section
//Network Parameter
WiFiClient    wifiMqtt;
PubSubClient  mqtt_Client(wifiMqtt);
#define mqtt_server       "192.168.99.23"
#define mqtt_username     "pi"
#define mqtt_password     "raspberry"
#define mqtt_port         1883
#define mqtt_client_name  "MumLedBallControllerTest2"

//------------------------------------- Mqtt Paths --------------------------------------//
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

//---- LED Ball Control Parameter from MQTT ----//
/*
 * Mirror image of the real parameters. Are only used to 
 * prevent confusion of origin (personal programming preferenc)
 */
boolean mqtt_Power;        

boolean mqtt_RandomColor;      
boolean mqtt_RainbowColor      
boolean mqtt_RandomColorSync  
uint8_t mqtt_Red;              
uint8_t mqtt_Green;           
uint8_t mqtt_Blue;            

uint8_t mqtt_Brightness;      

boolean mqtt_RandomEffect;   
uint8_t mqtt_EffectSpeed;     
uint8_t mqtt_FadeSpeed;        
uint8_t mqtt_EffectNumber;     
uint8_t mqtt_EffectDirection; 

//------------------------------------- WiFi Control -------------------------------------//
void wifi() {
  delay(10);
  //Start Wifi Connection
  WiFi.begin(SSIDWZ, WPA2WZ);
  delay(10);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    NoWifiCounter++;
    Serial.print("No WiFi : ");
    Serial.println(int(NoWifiCounter));
  }

}


//------------------------------------- MQTT Control -------------------------------------//
void mqtt() {
  delay(50);
  while (!mqtt_Client.connected()) {
    if (mqtt_Client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
      mqtt_Client.subscribe( mqtt_LED_setcolor);
      mqtt_Client.subscribe( mqtt_LED_setbrightness);
      mqtt_Client.subscribe( mqtt_LED_setstatus);
      mqtt_Client.subscribe( mqtt_LED_FadeSpeed);
      mqtt_Client.subscribe( mqtt_LED_EffectSpeed);
      mqtt_Client.subscribe( mqtt_LED_seteffect);
      mqtt_Client.subscribe( mqtt_LED_Random_Effect);
      mqtt_Client.subscribe( mqtt_LED_Random_Color);
      mqtt_Client.subscribe( mqtt_LED_RainbowActive);
    } else {
      delay(500);
    }
  }
}

//------------------------------------- MQTT Callback -------------------------------------//
void callback(char* topic, byte * payload, unsigned int length) {
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  //On / Off
  if (String(mqtt_LED_setstatus).equals(topic)) {
    if (strcmp(message, "ON") == 0) {
      Power = true;
      mqtt_Client.publish(mqtt_LED_getstatus, "ON");
    }
    else {
      Power = false;
      mqtt_Client.publish(mqtt_LED_getstatus, "OFF");
    }
  }

  //Color
  if (String(mqtt_LED_setcolor).equals(topic)) {
    char* red = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue = strtok(NULL, ",");
    Color_Red = atoi(red);
    Color_Green = atoi(green);
    Color_Blue = atoi(blue);
    char temp_buff[12];
    snprintf(temp_buff, 12, "%d,%d,%d", Color_Red, Color_Green, Color_Blue);
    mqtt_Client.publish(mqtt_LED_getcolor, temp_buff);
  }

  //Brightness
  if (String(mqtt_LED_setbrightness).equals(topic)) {
    Brightness = atoi(message);
    char temp_buff[3];
    itoa(Brightness, temp_buff, 10);
    mqtt_Client.publish(mqtt_LED_getbrightness, temp_buff);
  }

  //Effect
  if (String(mqtt_LED_seteffect).equals(topic)) {
    Effect_Number = atoi(message);
    char temp_buff[3];
    itoa(Effect_Number, temp_buff, 10);
    mqtt_Client.publish(mqtt_LED_geteffect, temp_buff);
  }

  //Effect Speed
  if (String(mqtt_LED_EffectSpeed).equals(topic)) {
    Effect_Speed = atoi(message);
  }

  //Fade Speed
  if (String(mqtt_LED_FadeSpeed).equals(topic)) {
    Fade_Speed = atoi(message);
  }

  //Random Color
  if (String(mqtt_LED_Random_Color).equals(topic)) {
    if (atoi(message) == 1) {
      Random_Color = true;
    }
    if (atoi(message) == 0) {
      Random_Color = false;
    }
  }

  //Rainbow Active
  if (String(mqtt_LED_RainbowActive).equals(topic)) {
    if (atoi(message) == 1) {
      Rainbow_Active = true;
    }
    if (atoi(message) == 0) {
      Rainbow_Active = false;
    }
  }

  //Random Effect
  if (String(mqtt_LED_Random_Effect).equals(topic)) {
    if (atoi(message) == 1) {
      Random_Effect = true;
    }
    if (atoi(message) == 0) {
      Random_Effect = false;
    }
  }

#ifdef DEBUG_NETWORK
  Serial.println("/--------Network Parameter--------/");
  Serial.print("   Power         :");
  Serial.println(Power);
  Serial.print("   Color Red     :");
  Serial.println(Color_Red);
  Serial.print("   Color Green   :");
  Serial.println(Color_Green);
  Serial.print("   Color Blue    :");
  Serial.println(Color_Blue);
  Serial.print("   Brightness    :");
  Serial.println(Brightness);
  Serial.print("   Effect Number :");
  Serial.println(Effect_Number);
  Serial.print("   Effect Speed  :");
  Serial.println(Effect_Speed);
  Serial.print("   Random Color  :");
  Serial.println(Random_Color);
  Serial.print("   Random Effect :");
  Serial.println(Random_Effect);
  Serial.println("/---------------------------------/");
#endif

}
//-------------------------------------End Callback--------------------------------------//

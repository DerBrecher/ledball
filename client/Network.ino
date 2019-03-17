//-------------------------------------Reconnect Wifi------------------------------------//
void reconnect_wifi() {
  delay(10);
  //Start Wifi Connection
  WiFi.begin(wifi_ssid, wifi_password);
  delay(10);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    NoWifiCounter++;
    Serial.print("No WiFi : ");
    Serial.println(int(NoWifiCounter));
  }

}
//----------------------------------End Reconnect Wifi-----------------------------------//

//---------------------------------Reconnect MqttClient----------------------------------//
void reconnect_MqttClient() {
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
//-------------------------------End Reconnect MqttClient--------------------------------//

//---------------------------------------Callback----------------------------------------//
void callback(char* topic, byte * payload, unsigned int length) {
  /*
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
  */
}
//-------------------------------------End Callback--------------------------------------//

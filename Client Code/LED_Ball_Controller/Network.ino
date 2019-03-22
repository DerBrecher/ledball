/*
  Information about Tap : Controls Wifi Connection and receives and process Commands
*/

//------------------------------------- WiFi Control -------------------------------------//
void wifi() {
  delay(1);
  //Start Wifi Connection
  if (StartWifiConnection) {
    WiFi.begin(SSIDWZ, WPA2WZ);
    StartWifiConnection = false;
  }
  delay(1);

  //Check if WiFi is Connected
  if (WiFi.status() == WL_CONNECTED) {
    //Reset
    StartWifiConnection = true;
    NoWifiCounter = 0;
  }
  delay(1);

  //Count in 300ms Step if no WiFi is Connected.
  unsigned long CurMillis_NoWiFiCounter = millis();
  if (CurMillis_NoWiFiCounter - PrevMillis_NoWiFiCounter >= TimeOut_NoWiFiCounter) {
    PrevMillis_NoWiFiCounter = CurMillis_NoWiFiCounter;
    NoWifiCounter++;
    Serial.print("No WiFi : ");
    Serial.println(int(NoWifiCounter));
  }

  //Restart ESP after the Counter hits 10
  if (int(NoWifiCounter) >= 10) {
    ESP.restart();
  }

}

//------------------------------------- MQTT Control -------------------------------------//
void mqtt() {
  delay(50);
  //Try to Reconnect to MQTT and subscribe to the Channels
  while (!mqtt_Client.connected()) {
    if (mqtt_Client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
      mqtt_Client.subscribe( mqtt_state_Power);
      mqtt_Client.subscribe( mqtt_state_RandomColor);
      mqtt_Client.subscribe( mqtt_state_RainbowColor);
      mqtt_Client.subscribe( mqtt_state_RandomColorSync);
      mqtt_Client.subscribe( mqtt_state_Color);
      mqtt_Client.subscribe( mqtt_state_Brightness);
      mqtt_Client.subscribe( mqtt_state_RandomEffect);
      mqtt_Client.subscribe( mqtt_state_EffectSpeed);
      mqtt_Client.subscribe( mqtt_state_FadeSpeed);
      mqtt_Client.subscribe( mqtt_state_EffectNumber);
      mqtt_Client.subscribe( mqtt_state_EffectDirection);
    } else {
      delay(500);
    }
  }
}

//------------------------------------- MQTT Callback -------------------------------------//
void callback(char* topic, byte * payload, unsigned int length) {

  //Set for Serial.print in Information Tab
  NewData = true;

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  //------------------- Parameter [mqtt_Power] -------------------//
  if (String(mqtt_state_Power).equals(topic)) {
    if (strcmp(message, "ON") == 0) {
      mqtt_Power = true;
      mqtt_Client.publish(mqtt_command_Power, "ON");
    }
    else {
      mqtt_Power = false;
      mqtt_Client.publish(mqtt_command_Power, "OFF");
    }
  }

  //------------------- Parameter [mqtt_RandomColor] -------------------//
  if (String(mqtt_state_RandomColor).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColor = true;
    } else {
      mqtt_RandomColor = false;
    }
    mqtt_Client.publish(mqtt_command_RandomColor, message);
  }

  //------------------- Parameter [mqtt_RainbowColor] -------------------//
  if (String(mqtt_state_RainbowColor).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RainbowColor = true;
    } else {
      mqtt_RainbowColor = false;
    }
    mqtt_Client.publish(mqtt_command_RainbowColor, message);
  }

  //------------------- Parameter [mqtt_RandomColorSync] -------------------//
  if (String(mqtt_state_RandomColorSync).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColorSync = true;
    } else {
      mqtt_RandomColorSync = false;
    }
    mqtt_Client.publish(mqtt_command_RandomColorSync, message);
  }

  //------------------- Parameter [mqtt_Red, mqtt_Green, mqtt_Blue] -------------------//
  if (String(mqtt_state_Color).equals(topic)) {
    char* red   = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue  = strtok(NULL, ",");

    mqtt_Red    = Check8BitBoundaries(atoi(red));
    mqtt_Green  = Check8BitBoundaries(atoi(green));
    mqtt_Blue   = Check8BitBoundaries(atoi(blue));

    mqtt_Client.publish(mqtt_command_Color, message);
  }

  //------------------- Parameter [mqtt_Brightness] -------------------//
  if (String(mqtt_state_Brightness).equals(topic)) {
    mqtt_Brightness = Check8BitBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_command_Brightness, message);
  }

  //------------------- Parameter [mqtt_RandomEffect] -------------------//
  if (String(mqtt_state_RandomEffect).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomEffect = true;
    } else {
      mqtt_RandomEffect = false;
    }
    mqtt_Client.publish(mqtt_command_RandomEffect, message);
  }

  //------------------- Parameter [mqtt_EffectSpeed] -------------------//
  if (String(mqtt_state_EffectSpeed).equals(topic)) {
    mqtt_EffectSpeed = Check8BitBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_command_EffectSpeed, message);
  }

  //------------------- Parameter [mqtt_FadeSpeed] -------------------//
  if (String(mqtt_state_FadeSpeed).equals(topic)) {
    mqtt_FadeSpeed = Check8BitBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_command_FadeSpeed, message);
  }

  //------------------- Parameter [mqtt_EffectNumber] -------------------//
  if (String(mqtt_state_EffectNumber).equals(topic)) {
    mqtt_EffectNumber = Check8BitBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_command_EffectNumber, message);
  }


  //------------------- Parameter [mqtt_EffectDirection] -------------------//
  if (String(mqtt_state_EffectDirection).equals(topic)) {
    mqtt_EffectDirection = CheckDirectionBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_command_EffectDirection, message);
  }

}

//------------------------------------- Boundarie Check --------------------------------------//
int Check8BitBoundaries(int x) {
  if (x < 0) {
    return 0;
  } else if (x > 255) {
    return 255;
  } else {
    return x;
  }
}

int Check1BitBoundaries(int x) {
  if (x < 0) {
    return 0;
  } else if (x > 1) {
    return 1;
  } else {
    return x;
  }
}

int CheckDirectionBoundaries(int x) {
  if (x != 8 && x != 6 && x != 2 && x != 4) {
    return 8;
  } else {
    return x;
  }
}

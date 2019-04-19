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

  //Try to Reconnect to MQTT and subscribe to the Channels
  if (!mqtt_Client.connected() and WiFi.status() == WL_CONNECTED) {
    if (mqtt_Client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
      Serial.println("Start channel subscription");
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
      Serial.println("Finished channel subscription");
    } else {
      Serial.println("No MQTT subscribtion possible");
    }
  }

}

//------------------------------------- MQTT Callback -------------------------------------//
void callback(char* topic, byte * payload, unsigned int length) {

  //Set for Serial.print in Information Tab
  NewData = true;
  char NullBuffer[1] = {'0'};

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
      mqtt_RandomColor     = true;
      mqtt_RainbowColor    = false;
      mqtt_RandomColorSync = false;
    } else {
      mqtt_RandomColor     = false;
      mqtt_RainbowColor    = false;
      mqtt_RandomColorSync = false;
    }

    mqtt_Client.publish(mqtt_command_RandomColor, message);         //State of Random Color
    mqtt_Client.publish(mqtt_command_RainbowColor, "0");     //Turn of Rainbow Color
    mqtt_Client.publish(mqtt_command_RandomColorSync, "0");  //Turn of Random Color Sync

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_RainbowColor] -------------------//
  if (String(mqtt_state_RainbowColor).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColor     = false;
      mqtt_RainbowColor    = true;
      mqtt_RandomColorSync = false;
    } else {
      mqtt_RandomColor     = false;
      mqtt_RainbowColor    = false;
      mqtt_RandomColorSync = false;
    }
    mqtt_Client.publish(mqtt_command_RandomColor, "0");      //Turn of Random Color
    mqtt_Client.publish(mqtt_command_RainbowColor, message);        //State of Rainbow Color
    mqtt_Client.publish(mqtt_command_RandomColorSync, "0");  //Turn of Random Color Sync

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_RandomColorSync] -------------------//
  if (String(mqtt_state_RandomColorSync).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColor     = false;
      mqtt_RainbowColor    = false;
      mqtt_RandomColorSync = true;
    } else {
      mqtt_RandomColor     = false;
      mqtt_RainbowColor    = false;
      mqtt_RandomColorSync = false;
    }
    mqtt_Client.publish(mqtt_command_RandomColor, "0");  //Turn of Random Color
    mqtt_Client.publish(mqtt_command_RainbowColor, "0"); //Turn of Rainbow Color
    mqtt_Client.publish(mqtt_command_RandomColorSync, message); //State of Random Color Sync

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_Red, mqtt_Green, mqtt_Blue] -------------------//
  if (String(mqtt_state_Color).equals(topic)) {
    char* red   = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue  = strtok(NULL, ",");

    mqtt_Red    = Check8BitBoundaries(atoi(red));
    mqtt_Green  = Check8BitBoundaries(atoi(green));
    mqtt_Blue   = Check8BitBoundaries(atoi(blue));

    mqtt_RandomColor     = false;
    mqtt_RainbowColor    = false;
    mqtt_RandomColorSync = false;

    mqtt_Client.publish(mqtt_command_RandomColor, "0");     //Turn of Random Color
    mqtt_Client.publish(mqtt_command_RainbowColor, "0");    //Turn of Rainbow Color
    mqtt_Client.publish(mqtt_command_RandomColorSync, "0"); //Turn of Random Color Sync

    mqtt_Client.publish(mqtt_command_Color, message);

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
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

    //Check if Message is a Number or a Text
    if (!isdigit(message[0])) {
      mqtt_EffectNumber = MessageToEffectNumber(message);
    } else {
      mqtt_EffectNumber = Check8BitBoundaries(atoi(message));
    }

    mqtt_Client.publish(mqtt_command_EffectNumber, message);
  }

  //------------------- Parameter [mqtt_EffectDirection] -------------------//
  if (String(mqtt_state_EffectDirection).equals(topic)) {
    int TempDirection;
    if (strcmp(message, "Oben") == 0) {
      TempDirection = 8;
    } else if (strcmp(message, "Links") == 0) {
      TempDirection = 4;
    } else if (strcmp(message, "Unten") == 0) {
      TempDirection = 2;
    } else if (strcmp(message, "Rechts") == 0) {
      TempDirection = 6;
    } else {
      TempDirection = 8;
    }
    mqtt_EffectDirection = CheckDirectionBoundaries(TempDirection);
    mqtt_Client.publish(mqtt_command_EffectDirection, message);

    //Needed for Rave
    InitRave = true;
  }
}

//------------------------------- MQTT MessageToEffectNumber --------------------------------//
uint8_t MessageToEffectNumber(char* message) {
  //Converts Name of the Effect to the EffectNumber
  if (strcmp(message, "black") == 0) {
    return 0;
  } else if (strcmp(message, "fillSolid") == 0) {
    return 1;
  } else if (strcmp(message, "RainDrop") == 0) {
    return 2;
  } else if (strcmp(message, "RingRun") == 0) {
    return 3;
  } else if (strcmp(message, "DiscoBall") == 0) {
    return 4;
  } else if (strcmp(message, "DiscoField") == 0) {
    return 5;
  } else if (strcmp(message, "Rave") == 0) {
    return 6;
  } else if (strcmp(message, "Equalizer") == 0) {
    return 7;
  } else if (strcmp(message, "SingleBounce") == 0) {
    return 8;
  } else if (strcmp(message, "DoubleBounce") == 0) {
    return 9;
  } else if (strcmp(message, "FullFlash") == 0) {
    return 10;
  } else if (strcmp(message, "HalfFlash") == 0) {
    return 11;
  } else if (strcmp(message, "QuarterFlash") == 0) {
    return 12;
  } else if (strcmp(message, "EighthFlash") == 0) {
    return 13;
  } else if (strcmp(message, "Circus") == 0) {
    return 14;
  } else if (strcmp(message, "Matrix") == 0) {
    return 15;
  } else if (strcmp(message, "LoopSnake") == 0) {
    return 16;
  } else if (strcmp(message, "HappyBirthday") == 0) {
    return 17;
  } else if (strcmp(message, "HappyNewYear") == 0) {
    return 18;
  } else if (strcmp(message, "SpiralSnake") == 0) {
    return 19;
  } else if (strcmp(message, "WaveRefresh") == 0) {
    return 20;
  } else if (strcmp(message, "Rotor") == 0) {
    return 21;
  } else if (strcmp(message, "Flash") == 0) {
    return 22;
  } else {
    return 0;
  }

}

//------------------------------------- MQTT Heartbeat -------------------------------------//
void HeartBeat() {
  unsigned long CurMillis_HeartBeat = millis();
  if (CurMillis_HeartBeat - PrevMillis_HeartBeat >= TimeOut_HeartBeat) {
    PrevMillis_HeartBeat = CurMillis_HeartBeat;
    HeartBeatCounter++;
    String FinalMessage = LedBallName + "," + String(HeartBeatCounter);
    char message[30];
    FinalMessage.toCharArray(message, sizeof(message));
    mqtt_Client.publish(mqtt_heartbeat, message);
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

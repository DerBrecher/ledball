/*
  Information about Tap : Controls Wifi Connection and receives and process Commands
*/

//------------------------------------- WiFi Control -------------------------------------//
void wifi() {

  delay(1);
  //Start Wifi Connection
  if (StartWifiConnection) {
    WiFi.mode(WIFI_STA);
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
  if (int(NoWifiCounter) >= 30) {
    ESP.restart();
  }

}

//------------------------------------- MQTT Control -------------------------------------//
void mqtt() {

  //Try to Reconnect to MQTT and subscribe to the Channels
  if (mqtt_Client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
    Serial.println("Start channel subscription");
    mqtt_Client.subscribe( mqtt_command_Power);
    mqtt_Client.subscribe( mqtt_command_RandomColor);
    mqtt_Client.subscribe( mqtt_command_RainbowColor);
    mqtt_Client.subscribe( mqtt_command_RandomColorSync);
    mqtt_Client.subscribe( mqtt_command_FilterColorMulti);
    mqtt_Client.subscribe( mqtt_command_Filter1_Color);
    mqtt_Client.subscribe( mqtt_command_Filter2_Color);
    mqtt_Client.subscribe( mqtt_command_Filter3_Color);
    mqtt_Client.subscribe( mqtt_command_Filter4_Color);
    mqtt_Client.subscribe( mqtt_command_Color);
    mqtt_Client.subscribe( mqtt_command_Brightness);
    mqtt_Client.subscribe( mqtt_command_RandomEffect);
    mqtt_Client.subscribe( mqtt_command_EffectSpeed);
    mqtt_Client.subscribe( mqtt_command_FadeSpeed);
    mqtt_Client.subscribe( mqtt_command_EffectNumber);
    mqtt_Client.subscribe( mqtt_command_EffectDirection);
    mqtt_Client.subscribe( mqtt_command_RandomEffectPower);
    //---- Not Used Parameter ----//
    mqtt_Client.subscribe( mqtt_command_Filter1);
    mqtt_Client.subscribe( mqtt_command_Filter2);
    mqtt_Client.subscribe( mqtt_command_Filter3);
    mqtt_Client.subscribe( mqtt_command_Filter4);
    Serial.println("Finished channel subscription");
  } else {
    Serial.println("No MQTT subscribtion possible");
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
  if (String(mqtt_command_Power).equals(topic)) {
    if (atoi(message)) {
      mqtt_Power = true;
      mqtt_Client.publish(mqtt_state_Power, "1");
    }
    else {
      mqtt_Power = false;
      mqtt_Client.publish(mqtt_state_Power, "0");
    }
  }

  //------------------- Parameter [mqtt_RandomColor] -------------------//
  if (String(mqtt_command_RandomColor).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColor      = true;
      mqtt_RainbowColor     = false;
      mqtt_RandomColorSync  = false;
      mqtt_FilterColorMulti = false;
    } else {
      mqtt_RandomColor      = false;
      mqtt_RainbowColor     = false;
      mqtt_RandomColorSync  = false;
      mqtt_FilterColorMulti = false;
    }

    mqtt_Client.publish(mqtt_state_RandomColor, message);         //State of Random Color
    mqtt_Client.publish(mqtt_state_RainbowColor, "0");     //Turn of Rainbow Color
    mqtt_Client.publish(mqtt_state_RandomColorSync, "0");  //Turn of Random Color Sync
    mqtt_Client.publish(mqtt_state_FilterColorMulti, "0"); //Turn of Filter Color Multi

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_RainbowColor] -------------------//
  if (String(mqtt_command_RainbowColor).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColor      = false;
      mqtt_RainbowColor     = true;
      mqtt_RandomColorSync  = false;
      mqtt_FilterColorMulti = false;
    } else {
      mqtt_RandomColor      = false;
      mqtt_RainbowColor     = false;
      mqtt_RandomColorSync  = false;
      mqtt_FilterColorMulti = false;
    }
    mqtt_Client.publish(mqtt_state_RandomColor, "0");      //Turn of Random Color
    mqtt_Client.publish(mqtt_state_RainbowColor, message);        //State of Rainbow Color
    mqtt_Client.publish(mqtt_state_RandomColorSync, "0");  //Turn of Random Color Sync
    mqtt_Client.publish(mqtt_state_FilterColorMulti, "0"); //Turn of Filter Color Multi

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_RandomColorSync] -------------------//
  if (String(mqtt_command_RandomColorSync).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColor      = false;
      mqtt_RainbowColor     = false;
      mqtt_RandomColorSync  = true;
      mqtt_FilterColorMulti = false;
    } else {
      mqtt_RandomColor      = false;
      mqtt_RainbowColor     = false;
      mqtt_RandomColorSync  = false;
      mqtt_FilterColorMulti = false;
    }
    mqtt_Client.publish(mqtt_state_RandomColor, "0");  //Turn of Random Color
    mqtt_Client.publish(mqtt_state_RainbowColor, "0"); //Turn of Rainbow Color
    mqtt_Client.publish(mqtt_state_RandomColorSync, message); //State of Random Color Sync
    mqtt_Client.publish(mqtt_state_FilterColorMulti, "0"); //Turn of Filter Color Multi

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_state_FilterColorMulti] -------------------//
  if (String(mqtt_command_FilterColorMulti).equals(topic)) {
    int temp_1Bit = Check1BitBoundaries(atoi(message));
    if (temp_1Bit) {
      mqtt_RandomColor      = false;
      mqtt_RainbowColor     = false;
      mqtt_RandomColorSync  = false;
      mqtt_FilterColorMulti = true;
    } else {
      mqtt_RandomColor      = false;
      mqtt_RainbowColor     = false;
      mqtt_RandomColorSync  = false;
      mqtt_FilterColorMulti = false;
    }
    mqtt_Client.publish(mqtt_state_RandomColor, "0");  //Turn of Random Color
    mqtt_Client.publish(mqtt_state_RainbowColor, "0"); //Turn of Rainbow Color
    mqtt_Client.publish(mqtt_state_RandomColorSync, "0"); //Turn of Random Color Sync
    mqtt_Client.publish(mqtt_state_FilterColorMulti, message); //State of Filter Color Multi

    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_Filter1Red, mqtt_Filter1Green, mqtt_Filter1Blue] -------------------//
  if (String(mqtt_command_Filter1_Color).equals(topic)) {
    mqtt_Client.publish(mqtt_state_Filter1_Color, message);
    char* red   = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue  = strtok(NULL, ",");

    //Check if Data has changed from Last time (for information print)
    if (mqtt_Filter4Red != Check8BitBoundaries(atoi(red)) || mqtt_Filter4Green != Check8BitBoundaries(atoi(green)) || mqtt_Filter4Blue != Check8BitBoundaries(atoi(blue))) {
      LightDataChange = true;
    }

    mqtt_Filter1Red    = Check8BitBoundaries(atoi(red));
    mqtt_Filter1Green  = Check8BitBoundaries(atoi(green));
    mqtt_Filter1Blue   = Check8BitBoundaries(atoi(blue));

  }

  //------------------- Parameter [mqtt_Filter2Red, mqtt_Filter2Green, mqtt_Filter2Blue] -------------------//
  if (String(mqtt_command_Filter2_Color).equals(topic)) {
    mqtt_Client.publish(mqtt_state_Filter2_Color, message);
    char* red   = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue  = strtok(NULL, ",");

    //Check if Data has changed from Last time (for information print)
    if (mqtt_Filter4Red != Check8BitBoundaries(atoi(red)) || mqtt_Filter4Green != Check8BitBoundaries(atoi(green)) || mqtt_Filter4Blue != Check8BitBoundaries(atoi(blue))) {
      LightDataChange = true;
    }

    mqtt_Filter2Red    = Check8BitBoundaries(atoi(red));
    mqtt_Filter2Green  = Check8BitBoundaries(atoi(green));
    mqtt_Filter2Blue   = Check8BitBoundaries(atoi(blue));

  }

  //------------------- Parameter [mqtt_Filter3Red, mqtt_Filter3Green, mqtt_Filter3Blue] -------------------//
  if (String(mqtt_command_Filter3_Color).equals(topic)) {
    mqtt_Client.publish(mqtt_state_Filter3_Color, message);
    char* red   = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue  = strtok(NULL, ",");

    //Check if Data has changed from Last time (for information print)
    if (mqtt_Filter4Red != Check8BitBoundaries(atoi(red)) || mqtt_Filter4Green != Check8BitBoundaries(atoi(green)) || mqtt_Filter4Blue != Check8BitBoundaries(atoi(blue))) {
      LightDataChange = true;
    }

    mqtt_Filter3Red    = Check8BitBoundaries(atoi(red));
    mqtt_Filter3Green  = Check8BitBoundaries(atoi(green));
    mqtt_Filter3Blue   = Check8BitBoundaries(atoi(blue));

  }

  //------------------- Parameter [mqtt_Filter4Red, mqtt_Filter4Green, mqtt_Filter4Blue] -------------------//
  if (String(mqtt_command_Filter4_Color).equals(topic)) {
    mqtt_Client.publish(mqtt_state_Filter4_Color, message);
    char* red   = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue  = strtok(NULL, ",");

    //Check if Data has changed from Last time (for information print)
    if (mqtt_Filter4Red != Check8BitBoundaries(atoi(red)) || mqtt_Filter4Green != Check8BitBoundaries(atoi(green)) || mqtt_Filter4Blue != Check8BitBoundaries(atoi(blue))) {
      LightDataChange = true;
    }

    mqtt_Filter4Red    = Check8BitBoundaries(atoi(red));
    mqtt_Filter4Green  = Check8BitBoundaries(atoi(green));
    mqtt_Filter4Blue   = Check8BitBoundaries(atoi(blue));

  }

  //------------------- Parameter [mqtt_Red, mqtt_Green, mqtt_Blue] -------------------//
  if (String(mqtt_command_Color).equals(topic)) {
    mqtt_Client.publish(mqtt_state_Color, message);
    char* red   = strtok(message, ",");
    char* green = strtok(NULL, ",");
    char* blue  = strtok(NULL, ",");

    mqtt_Red    = Check8BitBoundaries(atoi(red));
    mqtt_Green  = Check8BitBoundaries(atoi(green));
    mqtt_Blue   = Check8BitBoundaries(atoi(blue));

    mqtt_RandomColor      = false;
    mqtt_RainbowColor     = false;
    mqtt_RandomColorSync  = false;
    mqtt_FilterColorMulti = false;

    mqtt_Client.publish(mqtt_state_RandomColor, "0");     //Turn of Random Color
    mqtt_Client.publish(mqtt_state_RainbowColor, "0");    //Turn of Rainbow Color
    mqtt_Client.publish(mqtt_state_RandomColorSync, "0"); //Turn of Random Color Sync
    mqtt_Client.publish(mqtt_state_FilterColorMulti, "0"); //Turn of Filter Color Multi


    //For Random Color Sync
    SendNotSupported = true;
    SendOnlySupported = true;
  }

  //------------------- Parameter [mqtt_Brightness] -------------------//
  if (String(mqtt_command_Brightness).equals(topic)) {
    mqtt_Brightness = Check8BitBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_state_Brightness, message);
  }

  //------------------- Parameter [mqtt_RandomEffect] -------------------//
  if (String(mqtt_command_RandomEffect).equals(topic)) {
    if (atoi(message)) {
      mqtt_RandomEffect = true;
      mqtt_Client.publish(mqtt_state_RandomEffect, "1");
    } else {
      mqtt_RandomEffect = false;
      mqtt_Client.publish(mqtt_state_RandomEffect, "0");
    }

  }

  //------------------- Parameter [mqtt_RandomEffectPower] -------------------//
  if (String(mqtt_command_RandomEffectPower).equals(topic)) {
    mqtt_RandomEffectPower = Check8BitBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_state_RandomEffectPower, message);
  }

  //------------------- Parameter [mqtt_EffectSpeed] -------------------//
  if (String(mqtt_command_EffectSpeed).equals(topic)) {
    mqtt_EffectSpeed = Check8BitBoundaries(atoi(message));
    mqtt_Client.publish(mqtt_state_EffectSpeed, message);
  }

  //------------------- Parameter [mqtt_FadeSpeed] -------------------//
  if (String(mqtt_command_FadeSpeed).equals(topic)) {
    mqtt_FadeSpeed = Check8BitBoundaries(atoi(message));
    //Fade Speed cant be 255 or the Fade will crash
    if (mqtt_FadeSpeed == 255) {
      mqtt_FadeSpeed - 1;
    }
    mqtt_Client.publish(mqtt_state_FadeSpeed, message);
  }

  //------------------- Parameter [mqtt_EffectNumber] -------------------//
  if (String(mqtt_command_EffectNumber).equals(topic)) {

    //Check if Message is a Number or a Text
    if (!isdigit(message[0])) {
      mqtt_EffectNumber = MessageToEffectNumber(message);
    } else {
      mqtt_EffectNumber = Check8BitBoundaries(atoi(message));
    }

    mqtt_Client.publish(mqtt_state_EffectNumber, message);
  }

  //------------------- Parameter [mqtt_EffectDirection] -------------------//
  if (String(mqtt_command_EffectDirection).equals(topic)) {
    int TempDirection;
    if (strcmp(message, "8") == 0) {
      TempDirection = 8;
    } else if (strcmp(message, "4") == 0) {
      TempDirection = 4;
    } else if (strcmp(message, "2") == 0) {
      TempDirection = 2;
    } else if (strcmp(message, "6") == 0) {
      TempDirection = 6;
    } else {
      TempDirection = 8;
    }
    mqtt_EffectDirection = CheckDirectionBoundaries(TempDirection);
    mqtt_Client.publish(mqtt_state_EffectDirection, message);

    //Needed for Rave
    InitRave = true;
  }


  //------------------- Filter On / OFF Not used -------------------//
  if (String(mqtt_command_Filter1).equals(topic)) {
    LightDataChange = true;
    if (atoi(message)) {
      mqtt_Client.publish(mqtt_state_Filter1, "1");
      Filter1Active = true;
    }
    else {
      mqtt_Client.publish(mqtt_state_Filter1, "0");
      Filter1Active = false;
    }
  }

  //------------------- Filter On / OFF Not used -------------------//
  if (String(mqtt_command_Filter2).equals(topic)) {
    LightDataChange = true;
    if (atoi(message)) {
      mqtt_Client.publish(mqtt_state_Filter2, "1");
      Filter2Active = true;
    }
    else {
      mqtt_Client.publish(mqtt_state_Filter2, "0");
      Filter2Active = false;
    }
  }

  //------------------- Filter On / OFF Not used -------------------//
  if (String(mqtt_command_Filter3).equals(topic)) {
    LightDataChange = true;
    if (atoi(message)) {
      mqtt_Client.publish(mqtt_state_Filter3, "1");
      Filter3Active = true;
    }
    else {
      mqtt_Client.publish(mqtt_state_Filter3, "0");
      Filter3Active = false;
    }
  }

  //------------------- Filter On / OFF Not used -------------------//
  if (String(mqtt_command_Filter4).equals(topic)) {
    LightDataChange = true;
    if (atoi(message)) {
      mqtt_Client.publish(mqtt_state_Filter4, "1");
      Filter4Active = true;
    }
    else {
      mqtt_Client.publish(mqtt_state_Filter4, "0");
      Filter4Active = false;
    }
  }
}

//------------------------------- MQTT MessageToEffectNumber --------------------------------//
uint8_t MessageToEffectNumber(char* message) {
  //Deactivate RandomEffect if Manuel Effect is selected
  mqtt_RandomEffect = false;
  mqtt_Client.publish(mqtt_state_RandomEffect, "0");

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
  } else if (strcmp(message, "HalfFlash") == 0) {
    return 10;
  } else if (strcmp(message, "QuarterFlash") == 0) {
    return 11;
  } else if (strcmp(message, "EighthFlash") == 0) {
    return 12;
  } else if (strcmp(message, "Matrix") == 0) {
    return 13;
  } else if (strcmp(message, "WaveRefresh") == 0) {
    return 14;
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

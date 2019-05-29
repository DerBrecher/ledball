/*
  Information about Tap : Dedicated for helpful animation and data analysis
*/

void printer() {

#ifdef DEBUG_NETWORK
  //Check if New Data is available and its different from the last Data
  if (NewData and (
        mqtt_Power                != mem_mqtt_Power or
        mem_mqtt_RandomColor      != mqtt_RandomColor or
        mem_mqtt_RainbowColor     != mqtt_RainbowColor or
        mem_mqtt_RandomColorSync  != mqtt_RandomColorSync or
        mem_mqtt_FilterColorMulti != mqtt_FilterColorMulti or
        mem_mqtt_Filter1Red       != mqtt_Filter1Red or
        mem_mqtt_Filter1Green     != mqtt_Filter1Green or
        mem_mqtt_Filter1Blue      != mqtt_Filter1Blue or
        mem_mqtt_Filter2Red       != mqtt_Filter2Red or
        mem_mqtt_Filter2Green     != mqtt_Filter2Green or
        mem_mqtt_Filter2Blue      != mqtt_Filter2Blue or
        mem_mqtt_Filter3Red       != mqtt_Filter3Red or
        mem_mqtt_Filter3Green     != mqtt_Filter3Green or
        mem_mqtt_Filter3Blue      != mqtt_Filter3Blue or
        mem_mqtt_Filter4Red       != mqtt_Filter4Red or
        mem_mqtt_Filter4Green     != mqtt_Filter4Green or
        mem_mqtt_Filter4Blue      != mqtt_Filter4Blue or
        mem_mqtt_Red              != mqtt_Red or
        mem_mqtt_Green            != mqtt_Green or
        mem_mqtt_Blue             != mqtt_Blue or
        mem_mqtt_Brightness       != mqtt_Brightness or
        mem_mqtt_RandomEffect     != mqtt_RandomEffect or
        mem_mqtt_EffectSpeed      != mqtt_EffectSpeed or
        mem_mqtt_FadeSpeed        != mqtt_FadeSpeed or
        mem_mqtt_EffectNumber     != mqtt_EffectNumber or
        mem_mqtt_EffectDirection  != mqtt_EffectDirection)) {
    Serial.println("/--------Network Parameter--------/");
    //Power
    Serial.print("  Power              : ");
    Serial.println(mqtt_Power);
    //Color Picker
    Serial.print("  RandomColor        : ");
    Serial.println(mqtt_RandomColor);
    Serial.print("  Rainbow Color      : ");
    Serial.println(mqtt_RainbowColor);
    Serial.print("  Random Color Sync  : ");
    Serial.println(mqtt_RandomColorSync);
    Serial.print("  Filter Color Multi : ");
    Serial.println(mqtt_FilterColorMulti);
    //Filter 1
    Serial.print("  Filter 1 Red       : ");
    Serial.println(mqtt_Filter1Red);
    Serial.print("  Filter 1 Green     : ");
    Serial.println(mqtt_Filter1Green);
    Serial.print("  Filter 1 Blue      : ");
    Serial.println(mqtt_Filter1Blue);
    //Filter 2
    Serial.print("  Filter 2 Red       : ");
    Serial.println(mqtt_Filter2Red);
    Serial.print("  Filter 2 Green     : ");
    Serial.println(mqtt_Filter2Green);
    Serial.print("  Filter 2 Blue      : ");
    Serial.println(mqtt_Filter2Blue);
    //Filter 3
    Serial.print("  Filter 3 Red       : ");
    Serial.println(mqtt_Filter3Red);
    Serial.print("  Filter 3 Green     : ");
    Serial.println(mqtt_Filter3Green);
    Serial.print("  Filter 3 Blue      : ");
    Serial.println(mqtt_Filter3Blue);
    //Filter 4
    Serial.print("  Filter 4 Red       : ");
    Serial.println(mqtt_Filter4Red);
    Serial.print("  Filter 4 Green     : ");
    Serial.println(mqtt_Filter4Green);
    Serial.print("  Filter 4 Blue      : ");
    Serial.println(mqtt_Filter4Blue);
    //Color Normal
    Serial.print("  Color Red          : ");
    Serial.println(mqtt_Red);
    Serial.print("  Color Green        : ");
    Serial.println(mqtt_Green);
    Serial.print("  Color Blue         : ");
    Serial.println(mqtt_Blue);
    //Brightness
    Serial.print("  Brightness         : ");
    Serial.println(mqtt_Brightness);
    //Random Effect
    Serial.print("  Random Effect      : ");
    Serial.println(mqtt_RandomEffect);
    //Effect Speed
    Serial.print("  Effect Speed       : ");
    Serial.println(mqtt_EffectSpeed);
    //Fade Speed
    Serial.print("  Fade Speed         : ");
    Serial.println(mqtt_FadeSpeed);
    //Effect Number
    Serial.print("  Effect Number      : ");
    Serial.println(mqtt_EffectNumber);
    //Effect Direction
    Serial.print("  Effect Direction   : ");
    Serial.println(mqtt_EffectDirection);
    Serial.println("/---------------------------------/");
    //Reset
    NewData = false;

    mem_mqtt_Power            = mqtt_Power;

    mem_mqtt_RandomColor      = mqtt_RandomColor;
    mem_mqtt_RainbowColor     = mqtt_RainbowColor;
    mem_mqtt_RandomColorSync  = mqtt_RandomColorSync;
    mem_mqtt_FilterColorMulti = mqtt_FilterColorMulti;

    mem_mqtt_Filter1Red       = mqtt_Filter1Red;
    mem_mqtt_Filter1Green     = mqtt_Filter1Green;
    mem_mqtt_Filter1Blue      = mqtt_Filter1Blue;

    mem_mqtt_Filter2Red       = mqtt_Filter2Red;
    mem_mqtt_Filter2Green     = mqtt_Filter2Green;
    mem_mqtt_Filter2Blue      = mqtt_Filter2Blue;

    mem_mqtt_Filter3Red       = mqtt_Filter3Red;
    mem_mqtt_Filter3Green     = mqtt_Filter3Green;
    mem_mqtt_Filter3Blue      = mqtt_Filter3Blue;

    mem_mqtt_Filter4Red       = mqtt_Filter4Red;
    mem_mqtt_Filter4Green     = mqtt_Filter4Green;
    mem_mqtt_Filter4Blue      = mqtt_Filter4Blue;

    mem_mqtt_Red              = mqtt_Red;
    mem_mqtt_Green            = mqtt_Green;
    mem_mqtt_Blue             = mqtt_Blue;

    mem_mqtt_Brightness       = mqtt_Brightness;

    mem_mqtt_RandomEffect     = mqtt_RandomEffect;
    mem_mqtt_EffectSpeed      = mqtt_EffectSpeed;
    mem_mqtt_FadeSpeed        = mqtt_FadeSpeed;
    mem_mqtt_EffectNumber     = mqtt_EffectNumber;
    mem_mqtt_EffectDirection  = mqtt_EffectDirection;
  }
#endif


#ifdef DEBUG_MAIN_STATE
  //Prints the state of the main state machine if it changes
  if (mem_MainState != MainState) {
    Serial.println("/------------Main State-----------/");
    Serial.print("  Main State       : ");
    Serial.println(MainState);
    Serial.println("/---------------------------------/");
  }
  //Reset
  mem_MainState = MainState;
#endif


#ifdef DEBUG_LIGHT_STATE
  //Prints the state of the light state machine if it changes
  if (mem_LightState != LightState) {
    Serial.println("/-----------Light State-----------/");
    Serial.print("  Light State      : ");
    Serial.println(LightState);
    Serial.println("/---------------------------------/");
  }
  //Reset
  mem_LightState = LightState;
#endif


#ifdef DEBUG_COLOR
  //Prints the state of the Color Control
  if ((mem_ColorControl != ColorControl) or (LightDataChange && ColorControl == 4)) {
    Serial.println("/--------------Color--------------/");
    Serial.print("  Color Picker     : ");
    switch (ColorControl) {
      case 0 : Serial.println("Normal");
        break;
      case 1 : Serial.println("Random");
        break;
      case 2 : Serial.println("Rainbow");
        break;
      case 3 : Serial.println("Random Sync");
        break;
      case 4 : Serial.println("Filter Multi");
        //Filter 1
        if (Filter1Active) {
          Serial.print("      Filter 1     : ");
          Serial.print(Filter1Red);
          Serial.print(",");
          Serial.print(Filter1Green);
          Serial.print(",");
          Serial.print(Filter1Blue);
          Serial.println("");
        } else {
          Serial.print("      Filter 1     : ");
          Serial.println("OFF");
        }
        //Filter 2
        if (Filter2Active) {
          Serial.print("      Filter 2     : ");
          Serial.print(Filter2Red);
          Serial.print(",");
          Serial.print(Filter2Green);
          Serial.print(",");
          Serial.print(Filter2Blue);
          Serial.println("");
        } else {
          Serial.print("      Filter 2     : ");
          Serial.println("OFF");
        }
        //Filter 3
        if (Filter3Active) {
          Serial.print("      Filter 3     : ");
          Serial.print(Filter3Red);
          Serial.print(",");
          Serial.print(Filter3Green);
          Serial.print(",");
          Serial.print(Filter3Blue);
          Serial.println("");
        } else {
          Serial.print("      Filter 3     : ");
          Serial.println("OFF");
        }
        //Filter 4
        if (Filter4Active) {
          Serial.print("      Filter 4     : ");
          Serial.print(Filter4Red);
          Serial.print(",");
          Serial.print(Filter4Green);
          Serial.print(",");
          Serial.print(Filter4Blue);
          Serial.println("");
        } else {
          Serial.print("      Filter 4     : ");
          Serial.println("OFF");
        }
        break;
    }
    Serial.println("/---------------------------------/");
  }
  //Reset
  LightDataChange = false;
  mem_ColorControl = ColorControl;
#endif


#ifdef DEBUG_EFFECTS
  //Prints the current effect
  if (mem_EffectNumber != EffectNumber) {
    Serial.println("/--------------Effect-------------/");
    Serial.print("  Effect           : ");
    switch (EffectNumber) {
      case 0 : Serial.println("Black");
        break;
      case 1 : Serial.println("Fill Solid");
        break;
      case 2 : Serial.println("Rain Drop");
        break;
      case 3 : Serial.println("Ring Run");
        break;
      case 4 : Serial.println("Disco Ball");
        break;
      case 5 : Serial.println("Disco Field");
        break;
      case 6 : Serial.println("Rave");
        break;
      case 7 : Serial.println("Equalizer");
        break;
      case 8 : Serial.println("Single Bounce");
        break;
      case 9: Serial.println("Double Bounce");
        break;
      case 10: Serial.println("Full Flash");
        break;
      case 11: Serial.println("Half Flash");
        break;
      case 12: Serial.println("QuarterFlash");
        break;
      case 13: Serial.println("EighthFlash");
        break;
      case 14: Serial.println("Circus");
        break;
      case 15: Serial.println("Matrix");
        break;
      case 16: Serial.println("LoopSnake");
        break;
      case 17: Serial.println("HappyBirthday");
        break;
      case 18: Serial.println("HappyNewYear");
        break;
      case 19: Serial.println("SpiralSnake");
        break;
      case 20: Serial.println("WaveRefresh");
        break;
      case 21: Serial.println("Rotor");
        break;
      case 22: Serial.println("Flash");
        break;

      default: Serial.println("General Error Effect");
        break;
    }
    Serial.println("/---------------------------------/");
  }
  //Reset
  mem_EffectNumber = EffectNumber;
#endif

}

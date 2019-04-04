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
    Serial.print("  Power            : ");
    Serial.println(mqtt_Power);
    Serial.print("  RandomColor      : ");
    Serial.println(mqtt_RandomColor);
    Serial.print("  Rainbow Color    : ");
    Serial.println(mqtt_RainbowColor);
    Serial.print("  Random ColorSync : ");
    Serial.println(mqtt_RandomColorSync);
    Serial.print("  Color Red        : ");
    Serial.println(mqtt_Red);
    Serial.print("  Color Green      : ");
    Serial.println(mqtt_Green);
    Serial.print("  Color Blue       : ");
    Serial.println(mqtt_Blue);
    Serial.print("  Brightness       : ");
    Serial.println(mqtt_Brightness);
    Serial.print("  Random Effect    : ");
    Serial.println(mqtt_RandomEffect);
    Serial.print("  Effect Speed     : ");
    Serial.println(mqtt_EffectSpeed);
    Serial.print("  Fade Speed       : ");
    Serial.println(mqtt_FadeSpeed);
    Serial.print("  Effect Number    : ");
    Serial.println(mqtt_EffectNumber);
    Serial.print("  Effect Direction : ");
    Serial.println(mqtt_EffectDirection);
    Serial.println("/---------------------------------/");
    //Reset
    NewData = false;

    mem_mqtt_Power            = mqtt_Power;

    mem_mqtt_RandomColor      = mqtt_RandomColor;
    mem_mqtt_RainbowColor     = mqtt_RainbowColor;
    mem_mqtt_RandomColorSync  = mqtt_RandomColorSync;
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
  if (mem_ColorControl != ColorControl) {
    Serial.println("/--------------Color--------------/");
    Serial.print("  Color            : ");
    switch (ColorControl) {
      case 0 : Serial.println("Color Picker Normal");
        break;
      case 1 : Serial.println("Color Picker Random");
        break;
      case 2 : Serial.println("Color Picker Rainbow");
        break;
      case 3 : Serial.println("Color Picker Random Sync");
        break;
    }
    Serial.println("/---------------------------------/");
  }
  //Reset
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
      case 8 : Serial.println("");
        break;
      case 9: Serial.println("");
        break;
      case 10: Serial.println("");
        break;
      case 11: Serial.println("");
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

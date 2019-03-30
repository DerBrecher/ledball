/*
  Information about Tap : Dedicated for helpful animation and data analysis
*/

void printer() {

  //--------------------- Section from Network Tab ---------------------//
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
    Serial.print("  Power            :");
    Serial.println(mqtt_Power);
    Serial.print("  RandomColor      :");
    Serial.println(mqtt_RandomColor);
    Serial.print("  Rainbow Color    :");
    Serial.println(mqtt_RainbowColor);
    Serial.print("  Random ColorSync :");
    Serial.println(mqtt_RandomColorSync);
    Serial.print("  Color Red        :");
    Serial.println(mqtt_Red);
    Serial.print("  Color Green      :");
    Serial.println(mqtt_Green);
    Serial.print("  Color Blue       :");
    Serial.println(mqtt_Blue);
    Serial.print("  Brightness       :");
    Serial.println(mqtt_Brightness);
    Serial.print("  Random Effect    :");
    Serial.println(mqtt_RandomEffect);
    Serial.print("  Effect Speed     :");
    Serial.println(mqtt_EffectSpeed);
    Serial.print("  Fade Speed       :");
    Serial.println(mqtt_FadeSpeed);
    Serial.print("  Effect Number    :");
    Serial.println(mqtt_EffectNumber);
    Serial.print("  Effect Direction :");
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

}

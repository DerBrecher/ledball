/*
  Information about Tap : Dedicated for helpful animation and data analysis
*/

void printer() {

  //--------------------- Section from Network Tab ---------------------//
#ifdef DEBUG_NETWORK
  if (NewData) {
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
  }
#endif

}

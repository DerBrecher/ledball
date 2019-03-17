

void jsonHandler() {
  unsigned long CurMillis_JSONPolling = millis();
  if (CurMillis_JSONPolling - PrevMillis_JSONPolling > POLLINGRATE ) {
    PrevMillis_JSONPolling = CurMillis_JSONPolling;
    HTTPClient http;
    http.begin(LEDSERVERIP, LEDSERVERPORT, LEDSTATEURI);
    int httpCode = http.GET();

    Serial.print("Http Code is: ");
    Serial.println(httpCode);
    if (httpCode == 200) {
      //Serial.print("Paylout is: ");
      //Serial.println(http.getString());
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, http.getString());
      Power = doc["state"];
      Serial.print("Power is: ");
      Serial.println(Power);
      Color_Red = doc["color"]["red"];
      Serial.print("Red is: ");
      Serial.println(Color_Red);
      Color_Green = doc["color"]["green"];
      Color_Blue = doc["color"]["blue"];
      Brightness = doc["brightness"];
      Serial.print("Brightness is: ");
      Serial.println(Brightness);
      Effect_Number = doc["effectNo"];
      Serial.print("Effect_Number is: ");
      Serial.println(Effect_Number);
      Effect_Speed = doc["effectSpeed"];
      Fade_Speed = doc["fadeSpeed"];
      Random_Color = doc["randomColor"];
      Rainbow_Active = doc["rainbowColor"];
      Serial.print("Rainbow_Active is: ");
      Serial.println(Rainbow_Active);
      Random_Effect = doc["randomEffect"];
    }
    http.end();
  }
}


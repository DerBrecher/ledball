/*
  Information about Tap : Accommodates the lighting effects. Both status effects and finale effects the ball displays
*/

//---------------------------Light Control---------------------------//
void LightControl() {

  if (Power) {
    if (Random_Color) {
      RandomColor();
    } else {
      FadeColor();
    }

    //Controls the Brightness (can get override by some Effects)
    FadeBrightness();

    //Effect State
    if (Random_Effect) {
      RandomEffect();
    } else {
      if (LightState != Effect_Number) {
        black();
      }
      LightState = Effect_Number;
      //Reset Rave Array
      if (LightState != 6) {
        InitRave = true;
      }
      //Light Sate
      switch (LightState) {

        case 0: black();
          break;

        case 1: fillSolid();
          break;

        case 2: RainDrop();
          break;

        case 3: RingRun();
          break;

        case 4: DiscoBall();
          break;

        case 5: DiscoField();
          break;

        case 6: Rave();
          break;

        case 7: Equalizer();
          break;

        default:
          StatusEffectError();
          break;
      }
    }

  } else {
    fadeall(40);

  }
}

void ShowMatrix() {
  //Convert 2D Matrix into the 1D Matrix
  for (int i = 0 ; i < matrix_x; i++) {
    for (int u = 0; u < matrix_y; u++) {

      if (i % 2) {
        int reverse_U = matrix_y - u - 1;
        ledoutput[i * matrix_y + reverse_U] = leds[i][u];
      } else {
        ledoutput[i * matrix_y + u] = leds[i][u];
      }
    }
  }

  //If Gamma Correction Activated overwrite LED Value
  if (activatedGammaCorrection) {
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t  temp_red   = ledoutput[i].r;
      uint8_t  temp_green = ledoutput[i].g;
      uint8_t  temp_blue  = ledoutput[i].b;
      
      ledoutput[i].r = gamma[temp_red];   //Gamma Correction Red
      ledoutput[i].g = gamma[temp_green]; //Gamma Correction Green
      ledoutput[i].b = gamma[temp_blue];  //Gamma Correction Blue
    }
  }

  LEDS.setBrightness(actualBrightness);
  FastLED.show();
}

void RandomColor() {

  if (Rainbow_Active) {
    switch (IndexRainbow) {
      case 0:
        Color_Red   = 255;
        Color_Green = 0;
        Color_Blue  = 0;
        NextIndex = FadeColor();
        if (NextIndex) {
          IndexRainbow = 1;
        }
        break;
      case 1:
        Color_Red   = 255;
        Color_Green = 255;
        Color_Blue  = 0;
        NextIndex = FadeColor();
        if (NextIndex) {
          IndexRainbow = 2;
        }
        break;
      case 2:
        Color_Red   = 0;
        Color_Green = 255;
        Color_Blue  = 0;
        NextIndex = FadeColor();
        if (NextIndex) {
          IndexRainbow = 3;
        }
        break;
      case 3:
        Color_Red   = 0;
        Color_Green = 255;
        Color_Blue  = 255;
        NextIndex = FadeColor();
        if (NextIndex) {
          IndexRainbow = 4;
        }
        break;
      case 4:
        Color_Red   = 0;
        Color_Green = 0;
        Color_Blue  = 255;
        NextIndex = FadeColor();
        if (NextIndex) {
          IndexRainbow = 5;
        }
        break;
      case 5:
        Color_Red   = 255;
        Color_Green = 0;
        Color_Blue  = 255;
        NextIndex = FadeColor();
        if (NextIndex) {
          IndexRainbow = 0;
        }
        break;
    }
  } else {
    unsigned long CurMillis_RandomColor = millis();
    if (CurMillis_RandomColor - PrevMillis_RandomColor >= TimeOut_RandomColor) {
      PrevMillis_RandomColor = CurMillis_RandomColor;
      while (true) {
        IndexRainbow = int(random(0, 5));
        if (IndexRainbow != LastIndexRainbow) {
          break;
        }
      }
      LastIndexRainbow = IndexRainbow;
    }

    TimeOut_FadeColor = 20; //Set constant fadespeed
    switch (IndexRainbow) {

      case 0:
        Color_Red   = 255;
        Color_Green = 0;
        Color_Blue  = 0;
        FadeColor();
        break;

      case 1:
        Color_Red   = 255;
        Color_Green = 255;
        Color_Blue  = 0;
        FadeColor();
        break;

      case 2:
        Color_Red   = 0;
        Color_Green = 255;
        Color_Blue  = 0;
        FadeColor();
        break;

      case 3:
        Color_Red   = 0;
        Color_Green = 255;
        Color_Blue  = 255;
        FadeColor();
        break;

      case 4:
        Color_Red   = 0;
        Color_Green = 0;
        Color_Blue  = 255;
        FadeColor();
        break;

      case 5:
        Color_Red   = 255;
        Color_Green = 0;
        Color_Blue  = 255;
        FadeColor();
        break;

      default:
        Color_Red   = 0;
        Color_Green = 0;
        Color_Blue  = 0;
        FadeColor();
        break;

    }
  }
}

void RandomEffect() {

}

boolean FadeColor() {

  unsigned long CurMillis_FadeColor = millis();
  if (CurMillis_FadeColor - PrevMillis_FadeColor >= TimeOut_FadeColor) {
    PrevMillis_FadeColor = CurMillis_FadeColor;

    if (prevColorRed != Color_Red || prevColorGreen != Color_Green || prevColorBlue != Color_Blue) {

      int distanceRed   = Color_Red - actualColorRed;
      int distanceGreen = Color_Green - actualColorRedactualColorGreen;
      int distanceBlue  = Color_Blue - actualColorBlue;

      int maxDistance = max(abs(distanceRed), abs(distanceGreen));
      maxDistance = max(maxDistance, abs(distanceBlue));

      double StepSize = 5;

      FadeStepRed   = distanceRed * StepSize / maxDistance;
      FadeStepGreen = distanceGreen * StepSize / maxDistance;
      FadeStepBlue  = distanceBlue * StepSize / maxDistance;

      prevColorRed   = Color_Red;
      prevColorGreen = Color_Green;
      prevColorBlue  = Color_Blue;
    }

    //Color Fade
    if (Color_Red != int(actualColorRed)) {
      actualColorRed += FadeStepRed;
      if ((FadeStepRed > 0 && Color_Red < actualColorRed) || (FadeStepRed < 0 && Color_Red > actualColorRed)) {
        actualColorRed = Color_Red;
      }
    }
    if (Color_Green != int(actualColorRedactualColorGreen)) {
      actualColorRedactualColorGreen += FadeStepGreen;
      if ((FadeStepGreen > 0 && Color_Green < actualColorRedactualColorGreen) || (FadeStepGreen < 0 && Color_Green > actualColorRedactualColorGreen)) {
        actualColorRedactualColorGreen = Color_Green;
      }
    }
    if (Color_Blue != int(actualColorBlue)) {
      actualColorBlue += FadeStepBlue;
      if ((FadeStepBlue > 0 && Color_Blue < actualColorBlue) || (FadeStepBlue < 0 && Color_Blue > actualColorBlue)) {
        actualColorBlue = Color_Blue;
      }
    }
  }
  if ((int(actualColorRed) == Color_Red) && (int(actualColorRedactualColorGreen) == Color_Green) && (int(actualColorBlue) == Color_Blue)) {
    return true;
  } else {
    return false;
  }
}

void FadeBrightness() {
  unsigned long CurMillis_FadeBrightness = millis();
  if (CurMillis_FadeBrightness - PrevMillis_FadeBrightness >= TimeOut_FadeBrightness) {
    PrevMillis_FadeBrightness = CurMillis_FadeBrightness;
    if (actualBrightness < Brightness) {
      actualBrightness++;
    }
    if (actualBrightness > Brightness) {
      actualBrightness--;
    }
    if (actualBrightness == Brightness) {
      actualBrightness = Brightness;
    }
  }
}

//---------------------------End Light Control---------------------------//

//---------------------------Main Light Effects---------------------------//
void fillSolid() {
  unsigned long CurMillis_FPS = millis();
  if (CurMillis_FPS - PrevMillis_FPS >= int(FPS / 1000)) {
    PrevMillis_FPS = CurMillis_FPS;
    for (int i = 0 ; i < matrix_x; i++) {
      for (int u = 0; u < matrix_y; u++) {
        leds[i][u] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
      }
    }
  }
}

void Equalizer() {
  unsigned long CurMillis_FPS = millis();
  if (CurMillis_FPS - PrevMillis_FPS >= int(FPS / 1000)) {
    PrevMillis_FPS = CurMillis_FPS;



  }

}

void RainDrop() {
  //ToDo: Add Direction

  unsigned long CurMillis_FPS = millis();
  if (CurMillis_FPS - PrevMillis_FPS >= (int(FPS / 1000) + Effect_Speed)) {
    PrevMillis_FPS = CurMillis_FPS;

    //Parameter
    int NumberRainDrops = int(matrix_x * 0.2);

    //Shift Pixels Down
    for (int u = (matrix_y - 2); u >= 0; u--) {
      for (int i = 0; i < matrix_x; i++) {
        if (ledActive(leds[i][u + 1])) {
          leds[i][u + 1].nscale8(Fade_Speed);
        } else {
          leds[i][u + 1] = leds[i][u];
          leds[i][u].nscale8(Fade_Speed);
        }
      }
    }

    //Clear Pixels On Top
    for (int i = 0; i < matrix_x; i++) {
      leds[i][0] = CRGB(0, 0, 0);
    }

    //Generate new Random Pixels on Top
    for (int i = 0; i < NumberRainDrops; i++) {
      int RandomPixel = (int)random(0, matrix_x);
      leds[RandomPixel][0] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
    }
  }
}

void RingRun() {
  int DirectionRingRun = 2; //0 = right // 1 = left

  unsigned long CurMillis_FPS = millis();
  if (CurMillis_FPS - PrevMillis_FPS >= (int(FPS / 1000) + Effect_Speed)) {
    PrevMillis_FPS = CurMillis_FPS;
    switch (DirectionRingRun) {

      case 0: //Right
        for (int u = 0; u < matrix_y; u++) {
          leds[PosXEffectRingRun][u] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
        }
        break;

      case 1: //left
        for (int u = 0; u < matrix_y; u++) {
          leds[(matrix_x - 1) - PosXEffectRingRun][u] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
        }
        break;

      case 2:
        for (int i = 0; i < matrix_x; i++) {
          leds[i][PosYEffectRingRun] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
        }
        break;

      case 3:
        for (int i = 0; i < matrix_x; i++) {
          leds[i][(matrix_y - 1) - PosYEffectRingRun] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
        }
        break;
    }
    fadeall(Fade_Speed);
    PosXEffectRingRun++;
    if (PosXEffectRingRun > matrix_x - 1) {
      PosXEffectRingRun = 0;
    }
    PosYEffectRingRun++;
    if (PosYEffectRingRun > matrix_y - 1) {
      PosYEffectRingRun = 0;
    }
  }
}

void DiscoBall() {
  unsigned long CurMillis_FPS = millis();
  if (CurMillis_FPS - PrevMillis_FPS >= (int(FPS / 1000) + Effect_Speed)) {
    PrevMillis_FPS = CurMillis_FPS;
    for (int k = 0; k < NumberPixels; k++) {
      int x = random(0, matrix_x);
      int y = random(0, matrix_y);
      leds[x][y] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
    }
    ShowMatrix();
    fadeall(Fade_Speed);
  }
}

void DiscoField() {

  int NumberFieldsX = 4;
  int NumberFieldsY = 6;
  int NumberNewFields = 3;

  int StepsX = matrix_x / NumberFieldsX;
  int StepsY = matrix_y / NumberFieldsY;

  int RandomFieldX = 0;
  int RandomFieldY = 0;

  int LastRandomX = 0;
  int LastRandomY = 0;

  if (int(CounterNewFieldGen) >= 10) {
    for (int k = 0; k < NumberNewFields; k++) {
      while ((int(LastRandomX) == int(RandomFieldX)) && (int(LastRandomY) == int(RandomFieldY))) {
        RandomFieldX = random(0, (NumberFieldsX));
        RandomFieldY = random(0, (NumberFieldsY));
      }
      LastRandomX = RandomFieldX;
      LastRandomY = RandomFieldY;

      for (int i = int(StepsX * RandomFieldX); i < int(StepsX * RandomFieldX + StepsX); i++) {
        for (int u = int(StepsY * RandomFieldY); u < int(StepsY  * RandomFieldY + StepsY); u++) {
          leds[i][u] = CRGB(actualColorRed, actualColorRedactualColorGreen, actualColorBlue);
        }
      }
    }
    CounterNewFieldGen = 0;
  }

  unsigned long CurMillis_FPS = millis();
  if (CurMillis_FPS - PrevMillis_FPS >= (int(FPS / 1000) + Effect_Speed)) {
    PrevMillis_FPS = CurMillis_FPS;
    ShowMatrix();
    fadeall(Fade_Speed);
    CounterNewFieldGen++;
  }

}

void Rave() {
  if (InitRave) {
    //Init RaveArray
    int NeededSteps = 256 / matrix_x;
    int RaveHue = 0;
    for (int i = 0; i < matrix_x; i++) {
      for (int u = 0; u < matrix_y; u++) {
        leds[i][u] = CHSV(RaveHue, 255, 255);
      }
      RaveHue = RaveHue + NeededSteps;
    }
    InitRave = false;
  } else {
    //Perma Shift Array right
    unsigned long CurMillis_FPS = millis();
    if (CurMillis_FPS - PrevMillis_FPS >= (int(FPS / 1000) + Effect_Speed)) {
      PrevMillis_FPS = CurMillis_FPS;
      ShowMatrix();
      ShiftArrayToLeft();
    }
  }
}

void ShiftArrayToLeft() {
  for (int i = matrix_x; i > 0 ; i--) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i][u] = leds[i - 1][u];
    }
  }
  for (int u = 0; u < matrix_y; u++) {
    leds[0][u] = leds[matrix_x][u];
  }
}

void ShiftArrayToRight() {
  //Not Working
  for (int u = 0; u < matrix_y; u++) {
    leds[0][u] = leds[1][u];
  }
  for (int i = matrix_x; i > 0 ; i--) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i - 1][u] = leds[i][u];
    }
  }
}



//---------------------------End Main Light Effects---------------------------//

//---------------------------Information Effects---------------------------//
void GeneralErrorEffect() {
  unsigned long CurMillis_GeneralError = millis();
  if (CurMillis_GeneralError - PrevMillis_GeneralError >= TimeOut_GeneralError) {
    PrevMillis_GeneralError = CurMillis_GeneralError;
    for (int i = 0; i < matrix_y; i++) {
      leds[PosXStatusEffectError][i] = CRGB(255, 0, 0);
    }
    ShowMatrix();
    fadeall(40);
    if (PosXStatusEffectError >= matrix_x - 1) {
      PosXStatusEffectError = 0;
    } else {
      PosXStatusEffectError++;
    }
  }
}

boolean RGBCheckEffect() {
  unsigned long CurMillis_RGBCheck = millis();
  if (CurMillis_RGBCheck - PrevMillis_RGBCheck >= TimeOut_RGBCheck) {
    PrevMillis_RGBCheck = CurMillis_RGBCheck;
    for (int i = 0; i < matrix_x; i++) {
      for (int u = 0; u < matrix_y; u++) {
        switch (RGBCheckColor) {
          //Check RED
          case 0:  leds[i][u] = CRGB(255, 0, 0);
            break;

          //Check Green
          case 1:  leds[i][u] = CRGB(0, 255, 0);
            break;

          //Check Blue
          case 2:  leds[i][u] = CRGB(0, 0, 255);
            break;
        }
      }
    }
    RGBCheckColor++;
  }
  if (RGBCheckColor >= 4) {
    return true;
  } else {
    return false;
  }
}

boolean StartupReadyEffect() {
  unsigned long CurMillis_EffectStartupReady = millis();
  if (CurMillis_EffectStartupReady - PrevMillis_EffectStartupReady >= TimeOut_EffectStartupReady) {
    PrevMillis_EffectStartupReady = CurMillis_EffectStartupReady;
    for (int i = 0; i < matrix_y; i++) {
      leds[PosXStatusEffectStartupReady][i] = CHSV(hueStartupReady++, 255, 255);
    }
    ShowMatrix();
    if (CounterFinishedStartupSequence >= 9) {
      for (int i = 0; i < matrix_y; i++) {
        leds[PosXStatusEffectStartupReady][i] = CRGB(0, 0, 0);
      }
      ShowMatrix();
    }
    if (PosXStatusEffectStartupReady >= matrix_x - 1) {
      PosXStatusEffectStartupReady = 0;
      CounterFinishedStartupSequence++;
    } else {
      PosXStatusEffectStartupReady++;
    }
  }
  //Effect Finished
  if (CounterFinishedStartupSequence >= 10) {
    return true;
  } else {
    return false;
  }
}

void noWifiConnection() {
  //Effect when no WiFi is Connected
  for (int i = 0; i < matrix_x; i++) {
    for (int u = 0; u < matrix_y; u++) {
      if (i % 2 && u % 2) {
        leds[i][u] = CRGB(255, 0, 0);
      }
    }
  }
}
//---------------------------End Status Light Effects---------------------------//

//---------------------------Global Light Effects---------------------------//

boolean ledActive(CRGB led) {
  if ((led[0] == 0) && (led[1] == 0) && (led[2] == 0)) {
    return false;
  } else {
    return true;
  }
}

void fadeall(int scale) {
  for (int i = 0; i < matrix_x; i++) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i][u].nscale8(scale);
    }
  }
}

void black() {
  for (int i = 0; i < matrix_x; i++) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i][u] = CRGB(0, 0, 0);
    }
  }
}

void fadeAllToBlack() {

}

void fadeAllToPrecent() {

}


//---------------------------End Global Light Effects---------------------------//

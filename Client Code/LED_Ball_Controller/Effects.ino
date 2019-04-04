/*
  Information about Tap : Accommodates the lighting effects. Both status effects and finale effects the ball displays
*/


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
      int tempRed = ledoutput[i].r;
      int tempGreen = ledoutput[i].g;
      int tempBlue = ledoutput[i].b;

      ledoutput[i].r = gamma8[tempRed];   //Gamma Correction Red
      ledoutput[i].g = gamma8[tempGreen]; //Gamma Correction Green
      ledoutput[i].b = gamma8[tempBlue];  //Gamma Correction Blue
    }
  }
  LEDS.setBrightness(actualBrightness);
  FastLED.show();
}

boolean FadeColor() {

  unsigned long CurMillis_NormalColor = millis();
  if (CurMillis_NormalColor - PrevMillis_NormalColor >= TimeOut_NormalColor) {
    PrevMillis_NormalColor = CurMillis_NormalColor;

    if (prevColorRed != Red || prevColorGreen != Green || prevColorBlue != Blue) {

      int distanceRed   = Red - actualColorRed;
      int distanceGreen = Green - actualColorGreen;
      int distanceBlue  = Blue - actualColorBlue;

      int maxDistance = max(abs(distanceRed), abs(distanceGreen));
      maxDistance = max(maxDistance, abs(distanceBlue));

      double StepSize = 5;

      FadeStepRed   = distanceRed * StepSize / maxDistance;
      FadeStepGreen = distanceGreen * StepSize / maxDistance;
      FadeStepBlue  = distanceBlue * StepSize / maxDistance;

      prevColorRed   = Red;
      prevColorGreen = Green;
      prevColorBlue  = Blue;
    }

    //Color Fade
    if (Red != int(actualColorRed)) {
      actualColorRed += FadeStepRed;
      if ((FadeStepRed > 0 && Red < actualColorRed) || (FadeStepRed < 0 && Red > actualColorRed)) {
        actualColorRed = Red;
      }
    }
    if (Green != int(actualColorGreen)) {
      actualColorGreen += FadeStepGreen;
      if ((FadeStepGreen > 0 && Green < actualColorGreen) || (FadeStepGreen < 0 && Green > actualColorGreen)) {
        actualColorGreen = Green;
      }
    }
    if (Blue != int(actualColorBlue)) {
      actualColorBlue += FadeStepBlue;
      if ((FadeStepBlue > 0 && Blue < actualColorBlue) || (FadeStepBlue < 0 && Blue > actualColorBlue)) {
        actualColorBlue = Blue;
      }
    }
  }
  if ((int(actualColorRed) == Red) && (int(actualColorGreen) == Green) && (int(actualColorBlue) == Blue)) {
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
  for (int i = 0 ; i < matrix_x; i++) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
    }
  }
}

void Equalizer() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;

  }
}

void RainDrop() {
  //ToDo: Add Direction

  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;

    //Parameter
    int NumberRainDrops = int(matrix_x * 0.2);

    //Shift Pixels Down
    for (int u = (matrix_y - 2); u >= 0; u--) {
      for (int i = 0; i < matrix_x; i++) {
        if (ledActive(leds[i][u + 1])) {
          leds[i][u + 1].nscale8(FadeSpeed);
        } else {
          leds[i][u + 1] = leds[i][u];
          leds[i][u].nscale8(FadeSpeed);
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
      leds[RandomPixel][0] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
    }
  }
}

void RingRun() {
  int DirectionRingRun = 2;

  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    switch (DirectionRingRun) {

      case 0: //Right
        for (int u = 0; u < matrix_y; u++) {
          leds[PosXEffectRingRun][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;

      case 1: //left
        for (int u = 0; u < matrix_y; u++) {
          leds[(matrix_x - 1) - PosXEffectRingRun][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;

      case 2:
        for (int i = 0; i < matrix_x; i++) {
          leds[i][PosYEffectRingRun] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;

      case 3:
        for (int i = 0; i < matrix_x; i++) {
          leds[i][(matrix_y - 1) - PosYEffectRingRun] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;
    }
    fadeall(FadeSpeed);
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
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    for (int k = 0; k < NumberPixels; k++) {
      int x = random(0, matrix_x);
      int y = random(0, matrix_y);
      leds[x][y] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
    }
    fadeall(FadeSpeed);
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
          leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
      }
    }
    CounterNewFieldGen = 0;
  }

  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    fadeall(FadeSpeed);
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
    unsigned long CurMillis_Effect = millis();
    if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
      PrevMillis_Effect = CurMillis_Effect;
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
    if (CounterFinishedStartupSequence >= 9) {
      for (int i = 0; i < matrix_y; i++) {
        leds[PosXStatusEffectStartupReady][i] = CRGB(0, 0, 0);
      }
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
  unsigned long CurMillis_NoWiFiConntection = millis();
  if (CurMillis_NoWiFiConntection - PrevMillis_NoWiFiConnection >= TimeOut_NoWiFiConnection) {
    PrevMillis_NoWiFiConnection = CurMillis_NoWiFiConntection;
    //Effect when no WiFi is Connected
    for (int i = 0; i < matrix_x; i++) {
      for (int u = 0; u < matrix_y; u++) {
        if (i % 2 && u % 2) {
          leds[i][u] = CRGB(255, 0, 0);
        }
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

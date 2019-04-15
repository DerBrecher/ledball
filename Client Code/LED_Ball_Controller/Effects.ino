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

    //Check boundaries of the Color Values
    Red   = Check8BitBoundaries(Red);
    Green = Check8BitBoundaries(Green);
    Blue  = Check8BitBoundaries(Blue);

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
  //For Random Color Sync
  ColorPickerRandomSyncNotSupported();
}

void FullFlash() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed * 10) {
    PrevMillis_Effect = CurMillis_Effect;
    for (int i = 0 ; i < matrix_x; i++) {
      for (int u = 0; u < matrix_y; u++) {
        leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
    }
    //For Random Color Sync
    NextColor = true;
  }
}

void HalfFlash() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed * 10) {
    PrevMillis_Effect = CurMillis_Effect;
    if (nextHalfFlash) {
      if (SwapHalfFlash) {
        for (int i = 0 ; i < (matrix_x / 2); i++) {
          for (int u = 0; u < matrix_y; u++) {
            leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          }
        }
        //For Random Color Sync
        NextColor = true;
        SwapHalfFlash = false;
      } else {
        black();
        for (int i = (matrix_x / 2) ; i < matrix_x; i++) {
          for (int u = 0; u < matrix_y; u++) {
            leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          }
        }
        //For Random Color Sync
        NextColor = true;
        SwapHalfFlash = true;
      }
    }
    nextHalfFlash = fadeall(FadeSpeed);
  }
}

void QuarterFlash() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    if (nextQuarterFlash) {
      for (int i = (matrix_x / 4) * FlashSection ; i < (matrix_x / 4) * (FlashSection + 1); i++) {
        for (int u = 0; u < matrix_y; u++) {
          leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
      }
      NextColor = true;
      if (FlashSection < 3) {
        FlashSection++;
      } else {
        FlashSection = 0;
      }
    }
    nextQuarterFlash = fadeall(FadeSpeed);
  }
}

void EighthFlash() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    if (nextEighthFlash) {
      for (int i = (matrix_x / 4) * XFlashSection ; i < (matrix_x / 4) * (XFlashSection + 1); i++) {
        for (int u = (matrix_y / 2) * YFlashSection; u < (matrix_y / 2) * (YFlashSection + 1); u++) {
          leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
      }
      NextColor = true;
      if (XFlashSection < 3) {
        XFlashSection++;
      } else {
        XFlashSection = 0;
        if (YFlashSection < 1) {
          YFlashSection++;
        } else {
          YFlashSection = 0;
        }
      }
    }
    nextEighthFlash = fadeall(FadeSpeed);
  }
}

void SingleBounce() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    switch (EffectDirection) {
      case 8: //Up
      case 2: //Down
        if (SwapSingleBounce) {
          fadeall(FadeSpeed);
          for (int i = 0; i < matrix_x; i++) {
            leds[i][(matrix_y - 1) - PosYEffectSingleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          }
          if (PosYEffectSingleBounce == matrix_y - 1) {
            SwapSingleBounce = false;
            PosYEffectSingleBounce = 0;
            //For Random Color Sync
            NextColor = true;
          } else {
            PosYEffectSingleBounce++;
          }
        } else {
          fadeall(FadeSpeed);
          for (int i = 0; i < matrix_x; i++) {
            leds[i][PosYEffectSingleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          }
          if (PosYEffectSingleBounce == matrix_y - 1) {
            SwapSingleBounce = true;
            PosYEffectSingleBounce = 0;
            //For Random Color Sync
            NextColor = true;
          } else {
            PosYEffectSingleBounce++;
          }
        }
        break;
      case 4: //Left
      case 6: //right
        if (SwapSingleBounce) {
          fadeall(FadeSpeed);
          for (int i = 0; i < matrix_y; i++) {
            leds[(matrix_x - 1) - PosXEffectSingleBounce][i] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          }
          if (PosXEffectSingleBounce == matrix_x - 1) {
            SwapSingleBounce = false;
            PosXEffectSingleBounce = 0;
            //For Random Color Sync
            NextColor = true;
          } else {
            PosXEffectSingleBounce++;
          }
        } else {
          fadeall(FadeSpeed);
          for (int i = 0; i < matrix_y; i++) {
            leds[PosXEffectSingleBounce][i] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          }
          if (PosXEffectSingleBounce == matrix_x - 1) {
            SwapSingleBounce = true;
            PosXEffectSingleBounce = 0;
            //For Random Color Sync
            NextColor = true;
          } else {
            PosXEffectSingleBounce++;
          }
        }
        break;
    }
  }
}

void DoubleBounce() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    if (SwapSingleBounce) {
      fadeall(FadeSpeed);
      //First Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][(matrix_y - 1) - PosYEffectSingleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      //Second Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][((matrix_y / 2) - 1) - PosYEffectSingleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      if (PosYEffectSingleBounce == (matrix_y / 2) - 1) {
        SwapSingleBounce = false;
        PosYEffectSingleBounce = 0;
        //For Random Color Sync
        NextColor = true;
      } else {
        PosYEffectSingleBounce++;
      }
    } else {
      fadeall(FadeSpeed);
      //First Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][PosYEffectSingleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      //Second Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][((matrix_y / 2) - 1) + PosYEffectSingleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      if (PosYEffectSingleBounce == (matrix_y / 2) - 1) {
        SwapSingleBounce = true;
        PosYEffectSingleBounce = 0;
        //For Random Color Sync
        NextColor = true;
      } else {
        PosYEffectSingleBounce++;
      }
    }
  }
}

void Equalizer() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;


    //Create New Random Pos for the Hight of the Pillar between 35% and 85% of the Hight of the Pixels
    for (int i = 0; i < matrix_x; i++) {
      if (ledActive(leds[i][RandomPosYEqualizer[i]])) {
        RandomPosYEqualizer[i] = int(random(int(matrix_y * 0.35), int(matrix_y * 0.85)));
      }
    }

    //show or hide Pixel on the way to the new RandomPosYEqualizer
    for (int i = 0; i < matrix_x; i++) {
      for (int u = matrix_y - 1; u > 0; u--) {

        //Check if Show is Needed
        if (RandomPosYEqualizer[i] < u) {
          if ((ledActive(leds[i][u + 1])) and (not (ledActive(leds[i][u])))) {
            leds[i][u] = CRGB(0, 0, 0);
            break;
          }
        }

        //Check if Hide is Needed
        if (RandomPosYEqualizer[i] > u) {
          if ((ledActive(leds[i][u - 1])) and (not (ledActive(leds[i][u])))) {
            leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
            break;
          }
        }

        if (RandomPosYEqualizer[i] == u) {
          leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          break;
        }
      }
    }

    //The last 35% are Solid
    for (int i = 0; i < matrix_x; i++) {
      for (int u = (matrix_y - 1); u > matrix_y - int(matrix_y * 0.35); u--) {
        leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
    }

    //For Random Color Sync
    ColorPickerRandomSyncNotSupported();
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
      if (not (ledActive(leds[RandomPixel][1]))) {
        leds[RandomPixel][0] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
    }
    //For Random Color Sync
    ColorPickerRandomSyncNotSupported();
  }
}

void RingRun() {
  int DirectionRingRun = 2;

  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    switch (EffectDirection) {

      case 6: //Right
        for (int u = 0; u < matrix_y; u++) {
          leds[PosXEffectRingRun][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;

      case 4: //left
        for (int u = 0; u < matrix_y; u++) {
          leds[(matrix_x - 1) - PosXEffectRingRun][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;

      case 8: //up
        for (int i = 0; i < matrix_x; i++) {
          leds[i][PosYEffectRingRun] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;

      case 2: //down
        for (int i = 0; i < matrix_x; i++) {
          leds[i][(matrix_y - 1) - PosYEffectRingRun] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;
    }
    fadeall(FadeSpeed);
    PosXEffectRingRun++;
    if (PosXEffectRingRun > matrix_x - 1) {
      PosXEffectRingRun = 0;
      if (EffectDirection == 6 || EffectDirection == 4) {
        //For Random Color Sync
        NextColor = true;
      }
    }
    PosYEffectRingRun++;
    if (PosYEffectRingRun > matrix_y - 1) {
      PosYEffectRingRun = 0;
      if (EffectDirection == 8 || EffectDirection == 2) {
        //For Random Color Sync
        NextColor = true;
      }
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
    //For Random Color Sync
    ColorPickerRandomSyncNotSupported();
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
    //For Random Color Sync
    NextColor = true;
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
  //For Random Color Sync
  ColorPickerRandomSyncNotSupported();
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
  //Set Brightness to 200
  actualBrightness = 200;

  unsigned long CurMillis_GeneralError = millis();
  if (CurMillis_GeneralError - PrevMillis_GeneralError >= TimeOut_GeneralError) {
    PrevMillis_GeneralError = CurMillis_GeneralError;
    //Effect when a general error occurs
    black();
    for (int i = 0; i < matrix_y; i++) {
      leds[PosXStatusEffectError][i] = CRGB(255, 0, 0);
    }
    if (PosXStatusEffectError >= matrix_x - 1) {
      PosXStatusEffectError = 0;
    } else {
      PosXStatusEffectError++;
    }
  }
}

boolean RGBCheckEffect() {
  //Set Brightness to 200
  actualBrightness = 200;

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
  //Set Brightness to 200
  actualBrightness = 200;

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
  //Set Brightness to 200
  actualBrightness = 200;

  unsigned long CurMillis_NoWiFiConntection = millis();
  if (CurMillis_NoWiFiConntection - PrevMillis_NoWiFiConnection >= TimeOut_NoWiFiConnection) {
    PrevMillis_NoWiFiConnection = CurMillis_NoWiFiConntection;
    //Effect when no WiFi is Connected
    black();
    for (int i = 0; i < matrix_y; i++) {
      leds[PosXStatusEffectError][i] = CRGB(0, 255, 0);
    }
    if (PosXStatusEffectError >= matrix_x - 1) {
      PosXStatusEffectError = 0;
    } else {
      PosXStatusEffectError++;
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

boolean fadeall(int scale) {
  boolean check = true;
  for (int i = 0; i < matrix_x; i++) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i][u].nscale8(scale);
      if (ledActive(leds[i][u])) {
        check = false;
      }
    }
  }
  if (check) {
    return true;
  } else {
    return false;
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

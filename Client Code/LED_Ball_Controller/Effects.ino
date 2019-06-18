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

void RandomEffectPicker() {
  //Randomly Picks Effects. Got 3 Steps: 0. Slow 1. Middle 2.Fast 3. Super Fast
  unsigned long CurMillis_RandomEffectPicker = millis();

  //For Random Color Sync
  ColorPickerRandomSyncNotSupported();

  switch (RandomEffectPower) {

    case 0: //Slow
      if (CurMillis_RandomEffectPicker - PrevMillis_RandomEffectPicker >= 60000) {  //60 Sec
        PrevMillis_RandomEffectPicker = CurMillis_RandomEffectPicker;
        //Random Effect
        RandomEffectNumber = int(random(1, 9));
        //Random Effect Direction
        int temp = int(random(0, 3));
        switch (temp) {
          case 0: RandomEffectDirection = 8;
            break;
          case 1: RandomEffectDirection = 6;
            break;
          case 2: RandomEffectDirection = 2;
            break;
          case 3: RandomEffectDirection = 4;
            break;
        }
      }
      break;

    case 1: //Middle
      if (CurMillis_RandomEffectPicker - PrevMillis_RandomEffectPicker >= 40000) {  //40 Sec
        PrevMillis_RandomEffectPicker = CurMillis_RandomEffectPicker;
        RandomEffectNumber = int(random(1, 9));
        //Random Effect Direction
        int temp = int(random(0, 3));
        switch (temp) {
          case 0: RandomEffectDirection = 8;
            break;
          case 1: RandomEffectDirection = 6;
            break;
          case 2: RandomEffectDirection = 2;
            break;
          case 3: RandomEffectDirection = 4;
            break;
        }
      }
      break;

    case 2: //Fast
      if (CurMillis_RandomEffectPicker - PrevMillis_RandomEffectPicker >= 20000) {  //20 Sec
        PrevMillis_RandomEffectPicker = CurMillis_RandomEffectPicker;
        RandomEffectNumber = int(random(1, 9));
        //Random Effect Direction
        int temp = int(random(0, 3));
        switch (temp) {
          case 0: RandomEffectDirection = 8;
            break;
          case 1: RandomEffectDirection = 6;
            break;
          case 2: RandomEffectDirection = 2;
            break;
          case 3: RandomEffectDirection = 4;
            break;
        }
      }
      break;

    case 3: //Super Fast
      if (CurMillis_RandomEffectPicker - PrevMillis_RandomEffectPicker >= 10000) {  //10 Sec
        PrevMillis_RandomEffectPicker = CurMillis_RandomEffectPicker;
        RandomEffectNumber = int(random(1, 9));
        //Random Effect Direction
        int temp = int(random(0, 3));
        switch (temp) {
          case 0: RandomEffectDirection = 8;
            break;
          case 1: RandomEffectDirection = 6;
            break;
          case 2: RandomEffectDirection = 2;
            break;
          case 3: RandomEffectDirection = 4;
            break;
        }
      }
      break;

  }

  if (RandomEffectNumber == 7 and SingleInitEqualizer) {
    black();
    SingleInitEqualizer = false;
  }
  if (RandomEffectNumber != 7) {
    SingleInitEqualizer = true;
  }

  //Current Random Effect
  switch (RandomEffectNumber) { //Effect List

    case 1: //Matrix
      //Effect Direction not used
      mqtt_EffectDirection = 8;
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 75;
          mqtt_FadeSpeed = 205;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 60;
          mqtt_FadeSpeed = 195;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 45;
          mqtt_FadeSpeed = 180;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 30;
          mqtt_FadeSpeed = 165;
          break;
      }
      Matrix();
      break;

    case 2: //DoubleBounce
      //Effect Direction not used
      mqtt_EffectDirection = 8;
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 65;
          mqtt_FadeSpeed = 205;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 55;
          mqtt_FadeSpeed = 200;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 40;
          mqtt_FadeSpeed = 170;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 30;
          mqtt_FadeSpeed = 145;
          break;
      }
      DoubleBounce();
      break;

    case 3: //SingleBounce
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 65;
          mqtt_FadeSpeed = 205;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 55;
          mqtt_FadeSpeed = 200;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 40;
          mqtt_FadeSpeed = 170;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 30;
          mqtt_FadeSpeed = 145;
          break;
      }
      SingleBounce();
      break;

    case 4: //DiscoField
      //Effect Direction not used
      mqtt_EffectDirection = 8;
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 85;
          mqtt_FadeSpeed = 230;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 75;
          mqtt_FadeSpeed = 215;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 65;
          mqtt_FadeSpeed = 210;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 50;
          mqtt_FadeSpeed = 210;
          break;
      }
      DiscoField();
      break;

    case 5: //DiscoBall
      //Effect Direction not used
      mqtt_EffectDirection = 8;
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 90;
          mqtt_FadeSpeed = 230;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 80;
          mqtt_FadeSpeed = 220;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 70;
          mqtt_FadeSpeed = 205;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 50;
          mqtt_FadeSpeed = 205;
          break;
      }
      DiscoBall();
      break;

    case 6: //RingRun
      //Effect Direction used
      mqtt_EffectDirection = RandomEffectDirection;
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 85;
          mqtt_FadeSpeed = 225;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 75;
          mqtt_FadeSpeed = 215;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 60;
          mqtt_FadeSpeed = 200;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 50;
          mqtt_FadeSpeed = 200;
          break;
      }
      RingRun();
      break;

    case 7: //RainDrop
      //Effect Direction used
      mqtt_EffectDirection = RandomEffectDirection;
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 70;
          mqtt_FadeSpeed = 170;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 60;
          mqtt_FadeSpeed = 170;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 50;
          mqtt_FadeSpeed = 180;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 30;
          mqtt_FadeSpeed = 180;
          break;
      }
      RainDrop();
      break;

    case 8: //Equalizer
      //Effect Direction not used
      mqtt_EffectDirection = 8;
      //Settings for Effect syned with Effect Power
      switch (RandomEffectPower) {
        case 0: //Slow
          mqtt_EffectSpeed = 70;
          mqtt_FadeSpeed = 170;
          break;
        case 1: //Middle
          mqtt_EffectSpeed = 60;
          mqtt_FadeSpeed = 170;
          break;
        case 2: //Fast
          mqtt_EffectSpeed = 50;
          mqtt_FadeSpeed = 180;
          break;
        case 3: //Super Fast
          mqtt_EffectSpeed = 30;
          mqtt_FadeSpeed = 180;
          break;
      }
      Equalizer();
      break;

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

void WaveRefresh() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;

    switch (EffectDirection) {

      case 8: //Up
        for (int i = 0; i < matrix_x; i++) {
          leds[i][YPosWaveRefresh] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        if (YPosWaveRefresh <= 0) {
          YPosWaveRefresh = matrix_y - 1;
          NextColor = true;
        } else {
          YPosWaveRefresh--;
        }
        break;

      case 6: //Right
        for (int u = 0; u < matrix_y; u++) {
          leds[XPosWaveRefresh][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        if (XPosWaveRefresh >= matrix_x - 1) {
          XPosWaveRefresh = 0;
          NextColor = true;
        } else {
          XPosWaveRefresh++;
        }
        break;

      case 2: //Down
        for (int i = 0; i < matrix_x; i++) {
          leds[i][YPosWaveRefresh] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        if (YPosWaveRefresh >= matrix_y - 1) {
          YPosWaveRefresh = 0;
          NextColor = true;
        } else {
          YPosWaveRefresh++;
        }
        break;

      case 4: //left
        for (int u = 0; u < matrix_y; u++) {
          leds[XPosWaveRefresh][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        if (XPosWaveRefresh <= 0) {
          XPosWaveRefresh = matrix_x - 1;
          NextColor = true;
        } else {
          XPosWaveRefresh--;
        }
        break;

    }
  }
}

void Matrix() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    fadeall(FadeSpeed);
    if (SwapMatrix) {
      for (int i = 0; i < matrix_x ; i++) {
        if (i % 2) {
          leds[i][YMatrixDirection] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        } else {
          leds[i][matrix_y - YMatrixDirection] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
      }
    } else {
      for (int i = 0; i < matrix_x ; i++) {
        if (i % 2) {
          leds[i][matrix_y - YMatrixDirection] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        } else {
          leds[i][YMatrixDirection] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
      }
    }
    if (YMatrixDirection < matrix_y) {
      YMatrixDirection++;
    } else {
      YMatrixDirection = 0;
      NextColor = true;
      if (SwapMatrix) {
        SwapMatrix = false;
      } else {
        SwapMatrix = true;
      }
    }
  }
}

void HalfFlash() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed * 2) {
    PrevMillis_Effect = CurMillis_Effect;
    if (nextHalfFlash) {
      black();
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
    if (AllowFade) {
      nextHalfFlash = fadeall(FadeSpeed);
    } else {
      if (nextHalfFlash) {
        nextHalfFlash = false;
      } else {
        nextHalfFlash = true;
      }
    }
  }
}

void QuarterFlash() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    if (nextQuarterFlash) {
      black();
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
    if (AllowFade) {
      nextQuarterFlash = fadeall(FadeSpeed);
    } else {
      if (nextQuarterFlash) {
        nextQuarterFlash = false;
      } else {
        nextQuarterFlash = true;
      }
    }
  }
}

void EighthFlash() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;
    if (nextEighthFlash) {
      black();
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
    if (AllowFade) {
      nextQuarterFlash = fadeall(FadeSpeed);
    } else {
      if (nextEighthFlash) {
        nextEighthFlash = false;
      } else {
        nextEighthFlash = true;
      }
    }
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

    if (SwapDoubleBounce) {
      fadeall(FadeSpeed);
      //First Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][(matrix_y - 1) - PosYEffectDoubleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      //Second Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][(matrix_y / 2) - PosYEffectDoubleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      if (PosYEffectDoubleBounce == (matrix_y / 2)) {
        SwapDoubleBounce = false;
        //For Random Color Sync
        NextColor = true;
      } else {
        PosYEffectDoubleBounce++;
      }
    } else {
      fadeall(FadeSpeed);
      //First Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][(matrix_y - 1) - PosYEffectDoubleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      //Second Bounce
      for (int i = 0; i < matrix_x; i++) {
        leds[i][(matrix_y / 2) - PosYEffectDoubleBounce] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
      if (PosYEffectDoubleBounce == 0) {
        SwapDoubleBounce = true;
        //For Random Color Sync
        NextColor = true;
      } else {
        PosYEffectDoubleBounce--;
      }
    }
  }
}

void Equalizer() {
  unsigned long CurMillis_Effect = millis();
  if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
    PrevMillis_Effect = CurMillis_Effect;

    //Top 20% are Black
    for (int i = 0; i < matrix_x; i++) {
      for (int u = 0; u < (matrix_y * 0.2); u++) {
        leds[i][u] = CRGB(0, 0, 0);
      }
    }

    //Bottom 20% are Filled
    for (int i = 0; i < matrix_x; i++) {
      for (int u = matrix_y - 1; u > matrix_y - int(matrix_y * 0.2); u--) {
        leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
      }
    }

    //Create New Random Pos for the Hight of the Pillar between 35% and 85% of the Hight of the Pixels
    for (int i = 0; i < matrix_x; i++) {
      //New Pos when LED is Active and the LED on Top is nor Active
      if (ledActive(leds[i][RandomPosYEqualizer[i]]) and not ledActive(leds[i][RandomPosYEqualizer[i] - 1])) {
        RandomPosYEqualizer[i] = (int)random((matrix_y * 0.2), (matrix_y * 0.8));
      }
    }

    //show or hide Pixel on the way to the new RandomPosYEqualizer
    for (int i = 0; i < matrix_x; i++) {

      //Check if Show is Needed
      for (int u = matrix_y - 1; u > 0; u--) {
        if (RandomPosYEqualizer[i] != u) {
          if (not ledActive(leds[i][u])) {
            leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
            break;
          }
        } else {
          leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
          break;
        }
      }

      //Check if Hide is Needed
      for (int u = 0; u < matrix_y; u++) {
        if (RandomPosYEqualizer[i] != u) {
          if (ledActive(leds[i][u])) {
            leds[i][u] = CRGB(0, 0, 0);
            break;
          }
        } else {
          break;
        }
      }

    }

    //Update all LEDs for Color Picker
    for (int i = 0; i < matrix_x; i++) {
      for (int u = 0; u < matrix_y; u++) {
        if (ledActive(leds[i][u])) {
          leds[i][u] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
      }
    }

    //For Random Color Sync
    ColorPickerRandomSyncNotSupported();
  }
}

void RainDrop() {
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

      case 2: //down
        for (int i = 0; i < matrix_x; i++) {
          leds[i][PosYEffectRingRun] = CRGB(actualColorRed, actualColorGreen, actualColorBlue);
        }
        break;

      case 8: //up
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
    if (EffectDirection == 6 || EffectDirection == 4) {
      //Init RaveArray for Left / Right
      int NeededStepsX = 256 / matrix_x;
      int RaveHue = 0;
      for (int i = 0; i < matrix_x; i++) {
        for (int u = 0; u < matrix_y; u++) {
          leds[i][u] = CHSV(RaveHue, 255, 255);
        }
        RaveHue = RaveHue + NeededStepsX;
      }
    }
    if (EffectDirection == 8 || EffectDirection == 2) {
      //Init RaveArray for Up / Down
      int NeededStepsY = 256 / matrix_y;
      int RaveHue = 0;
      for (int u = 0; u < matrix_y; u++) {
        for (int i = 0; i < matrix_x; i++) {
          leds[i][u] = CHSV(RaveHue, 255, 255);
        }
        RaveHue = RaveHue + NeededStepsY;
      }
    }
    InitRave = false;
  } else {
    //Perma Shift Array right
    unsigned long CurMillis_Effect = millis();
    if (CurMillis_Effect - PrevMillis_Effect >= EffectSpeed) {
      PrevMillis_Effect = CurMillis_Effect;
      switch (EffectDirection) {

        case 8: //Up
          ShiftArrayUp();
          break;

        case 6: //Right
          ShiftArrayRight();

          break;

        case 2: //Down
          ShiftArrayDown();
          break;

        case 4: //Left
          ShiftArrayLeft();
          break;

      }
    }
  }
  //For Random Color Sync
  ColorPickerRandomSyncNotSupported();
}

void ShiftArrayUp() {
  for (int u = 0; u < matrix_y - 1; u++) {
    for (int i = 0; i < matrix_x; i++) {
      leds[i][u] = leds[i][u + 1];
    }
  }
  for (int i = 0; i < matrix_x; i++) {
    leds[i][matrix_y - 1] = leds[i][0];
  }
}

void ShiftArrayDown() {
  for (int u = matrix_y; u > 0; u--) {
    for (int i = 0; i < matrix_x; i++) {
      leds[i][u] = leds[i][u - 1];
    }
  }
  for (int i = 0; i < matrix_x; i++) {
    leds[i][0] = leds[i][matrix_y - 1];
  }
}

void ShiftArrayLeft() {
  for (int i = 0; i < matrix_x - 1; i++) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i][u] = leds[i + 1][u];
    }
  }
  for (int u = 0; u < matrix_y; u++) {
    leds[matrix_x - 1][u] = leds[0][u];
  }
}

void ShiftArrayRight() {
  for (int i = matrix_x; i > 0 ; i--) {
    for (int u = 0; u < matrix_y; u++) {
      leds[i][u] = leds[i - 1][u];
    }
  }
  for (int u = 0; u < matrix_y; u++) {
    leds[0][u] = leds[matrix_x - 1][u];
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

void noMqttConnection() {
  //Set Brightness to 200
  actualBrightness = 200;

  unsigned long CurMillis_NoWiFiConntection = millis();
  if (CurMillis_NoWiFiConntection - PrevMillis_NoWiFiConnection >= TimeOut_NoWiFiConnection) {
    PrevMillis_NoWiFiConnection = CurMillis_NoWiFiConntection;
    //Effect when no WiFi is Connected
    black();
    for (int i = 0; i < matrix_y; i++) {
      leds[PosXStatusEffectError][i] = CRGB(0, 0, 255);
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

//---------------------------End Global Light Effects---------------------------//

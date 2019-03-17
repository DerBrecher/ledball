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
    ShowMatrix();
  }
}

void ShowMatrix() {
  //Copy Data form 2D FastLED Array to 1D NeoPixelBusArray & apply Brightness & apply gamma8 only to brightness
  for (int i = 0 ; i < matrix_x; i++) {
    for (int u = 0; u < matrix_y; u++) {
      if (i % 2) {
        int reverse_U = matrix_y - u - 1;
        *(strip.Pixels() + (i * matrix_y + reverse_U) * 3) = int(leds[i][u][0])*gamma8[oldBrightness]/255; //Red
        *(strip.Pixels() + (i * matrix_y + reverse_U) * 3 + 1) = int(leds[i][u][1])*gamma8[oldBrightness]/255; //Green
        *(strip.Pixels() + (i * matrix_y + reverse_U) * 3 + 2) = int(leds[i][u][2])*gamma8[oldBrightness]/255; //Blue
      } else {
        *(strip.Pixels() + (i * matrix_y + u) * 3) = int(leds[i][u][0])*gamma8[oldBrightness]/255; //Red
        *(strip.Pixels() + (i * matrix_y + u) * 3 + 1) = int(leds[i][u][1])*gamma8[oldBrightness]/255; //Green
        *(strip.Pixels() + (i * matrix_y + u) * 3 + 2) = int(leds[i][u][2])*gamma8[oldBrightness]/255; //Blue
      }
    }
  }
  strip.Dirty();
  strip.Show();
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

      int distanceRed   = Color_Red - oldColorRed;
      int distanceGreen = Color_Green - oldColorGreen;
      int distanceBlue  = Color_Blue - oldColorBlue;

      int maxDistance = max(abs(distanceRed), abs(distanceGreen));
      maxDistance = max(maxDistance, abs(distanceBlue));

      double StepSize = 5;

      StepRed   = distanceRed * StepSize / maxDistance;
      StepGreen = distanceGreen * StepSize / maxDistance;
      StepBlue  = distanceBlue * StepSize / maxDistance;

      prevColorRed   = Color_Red;
      prevColorGreen = Color_Green;
      prevColorBlue  = Color_Blue;
    }

    //Color Fade
    if (Color_Red != int(oldColorRed)) {
      oldColorRed += StepRed;
      if ((StepRed > 0 && Color_Red < oldColorRed) || (StepRed < 0 && Color_Red > oldColorRed)) {
        oldColorRed = Color_Red;
      }
    }
    if (Color_Green != int(oldColorGreen)) {
      oldColorGreen += StepGreen;
      if ((StepGreen > 0 && Color_Green < oldColorGreen) || (StepGreen < 0 && Color_Green > oldColorGreen)) {
        oldColorGreen = Color_Green;
      }
    }
    if (Color_Blue != int(oldColorBlue)) {
      oldColorBlue += StepBlue;
      if ((StepBlue > 0 && Color_Blue < oldColorBlue) || (StepBlue < 0 && Color_Blue > oldColorBlue)) {
        oldColorBlue = Color_Blue;
      }
    }
  }
  if ((int(oldColorRed) == Color_Red) && (int(oldColorGreen) == Color_Green) && (int(oldColorBlue) == Color_Blue)) {
    return true;
  } else {
    return false;
  }
}

void FadeBrightness() {
  unsigned long CurMillis_FadeBrightness = millis();
  if (CurMillis_FadeBrightness - PrevMillis_FadeBrightness >= TimeOut_FadeBrightness) {
    PrevMillis_FadeBrightness = CurMillis_FadeBrightness;
    if (oldBrightness < Brightness) {
      oldBrightness++;
    }
    if (oldBrightness > Brightness) {
      oldBrightness--;
    }
    if (oldBrightness == Brightness) {
      oldBrightness = Brightness;
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
        leds[i][u] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
      }
    }
    ShowMatrix();
  }
}

void Equalizer() {
  unsigned long CurMillis_FPS = millis();
  if (CurMillis_FPS - PrevMillis_FPS >= int(FPS / 1000)) {
    PrevMillis_FPS = CurMillis_FPS;



    ShowMatrix();
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
      leds[RandomPixel][0] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
    }
    ShowMatrix();
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
          leds[PosXEffectRingRun][u] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
        }
        break;

      case 1: //left
        for (int u = 0; u < matrix_y; u++) {
          leds[(matrix_x - 1) - PosXEffectRingRun][u] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
        }
        break;

      case 2:
        for (int i = 0; i < matrix_x; i++) {
          leds[i][PosYEffectRingRun] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
        }
        break;

      case 3:
        for (int i = 0; i < matrix_x; i++) {
          leds[i][(matrix_y - 1) - PosYEffectRingRun] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
        }
        break;
    }
    ShowMatrix();
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
      leds[x][y] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
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
          leds[i][u] = CRGB(oldColorRed, oldColorGreen, oldColorBlue);
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
  ShowMatrix();
}

void fadeAllToBlack() {

}

void fadeAllToPrecent() {

}


//---------------------------End Global Light Effects---------------------------//


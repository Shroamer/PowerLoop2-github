void plotA(float inLimitLevel, float currentValue) {
  //display.clearDisplay();
  char floatstring[12];  // to line up values by right side
  //float currentValue;
  float numPrec;
  bool currentActive = 0;
  int numLenght = 0;

  //currentValue = inaVal.R;
  numLenght = VAL_R_DIGITS;
  numPrec = VAL_R_PREC;

  //ESP_LOGD("Min-Max=", "%.8f - %.8f", screenArray.Min, screenArray.Max);
  float gridSize = findGridSize(screenArray.Min, screenArray.Max);  // find grid size from range
  //ESP_LOGD("gridSize=", "%.8f", gridSize);

  screenmap.init(screenArray.Min, screenArray.Max, SCREEN_HEIGHT - 1, 0);  // init mapping scale;
  for (int i = 0; i < 11; ++i) {                                           // drawing grid lines
    float line = findGridLine(screenArray.Min, screenArray.Max, gridSize, i);
    if (isnan(line)) {
      //ESP_LOGD("grid", "-no line-");
      break;
    }
    //Serial.println(line);
    //ESP_LOGD("grid", "Line #%d at %f", i, line, 6);
    byte lineVal = screenmap.map(line);  // mapping inbound level line
    for (int x = 0; x < 128; x += 12) {  // draw line line
      display.drawPixel(x, lineVal, SSD1306_INVERSE);
      //display.fillRect(x, lineVal, 2, 1, SSD1306_INVERSE);
    }
  }

  // draw a limit line if limit is within screen area
  if (inLimitLevel < screenArray.Max && inLimitLevel > screenArray.Min) {
    //screenmap.init(screenArray.Min, screenArray.Max, SCREEN_HEIGHT - 1, 0);  // init mapping scale
    byte limitVal = screenmap.map(inLimitLevel);  // mapping inbound level line
    for (int x = 0; x < 128; x += 6) {            // draw inLimitLevel line
      //display.drawPixel(x, limitVal, SSD1306_INVERSE);
      display.fillRect(x, limitVal, 2, 1, SSD1306_INVERSE);
    }
  }

  {  // printing min, max, current, limit values
    display.setTextSize(1);
    {                                                             // print topMargin value
      dtostrf(screenArray.Max, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(0, 8 * 0);                                // 0 line
      display.print(floatstring);
    }
    {                                                             // print bottomMargin value
      dtostrf(screenArray.Min, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(0, 8 * 7);                                // 7 line (last)
      if (limitMode == 1) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.print(floatstring);
      if (limitMode == 1) display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }
    {                                                          // print CURRENT value
      dtostrf(currentValue, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(0, 8 * 3);                             // 3 line (above centerline)
      //if (currentActive) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.print(floatstring);
      //if (currentActive) display.setTextColor(SSD1306_WHITE);
    }
    {                                                          // print inLimitLevel value
      dtostrf(inLimitLevel, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(0, 8 * 4);                             // 4 line (below centerline)
      display.print(floatstring);
    }
  }

  for (int i = 127; i >= 0; i--) {  // plotting screenArray along the screen
    display.fillRect(127 - i, screenArray.Val[i], 1, SCREEN_HEIGHT, SSD1306_INVERSE);
  }
  //ESP_LOGD("PLOT", "OK");
  //display.display();
}

float findGridSize(float inMin, float inMax) {
  float screenRange = inMax - inMin;
  float gridSize;
  gridSize = pow(2, floor(log2(screenRange))) / 2;  // log2(screenRange) gives base-2 logarithm of the screenRange.
                                                    // floor function rounds this down to the nearest whole number.
                                                    // pow(2, ...) raises 2 to the power of this number.
                                                    // This gives a grid size that doubles for each doubling of the screen range
                                                    //ESP_LOGD("screenRange=inMax-inMin=", "%.8f = %.8f - %.8f", screenRange, inMax, inMin);
                                                    //ESP_LOGD("screenRange=", "%f", screenRange);
  return (gridSize);
}

float findGridLine(float windowMin, float windowMax, float gridSize, int skipSeed) {
  float firstLine = ceil(windowMin / gridSize) * gridSize;  // Calculate the position of the first grid line within the window
  float line = firstLine + skipSeed * gridSize;             // Skip the specified number of lines
  if (line <= windowMax) {                                  // If the line is within the window, return its position
    return line;
  }
  return NAN;  // If no line was found within the window, return NAN
}

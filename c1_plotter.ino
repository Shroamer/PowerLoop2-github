void plotA(float inLimitLevel) {
  //display.clearDisplay();
  char floatstring[12];  // to line up values by right side
  float currentValue;
  float numPrec;
  bool currentActive = 0;
  int numLenght = 0;

  currentValue = inaVal.R;
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
    for (int x = 0; x < 128; x += 10) {  // draw line line
      display.drawPixel(x, lineVal, SSD1306_INVERSE);
    }
  }

  if (inLimitLevel < screenArray.Max && inLimitLevel > screenArray.Min) {  // draw a limit line if limit is within screen area
    //screenmap.init(screenArray.Min, screenArray.Max, SCREEN_HEIGHT - 1, 0);  // init mapping scale
    byte limitVal = screenmap.map(inLimitLevel);  // mapping inbound level line
    for (int x = 0; x < 128; x += 4) {            // draw inLimitLevel line
      display.drawPixel(x, limitVal, SSD1306_INVERSE);
    }
  }

  {  // printing min, max, current, limit values
    display.setTextSize(1);
    {                                                             // print topMargin value
      dtostrf(screenArray.Max, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 0);            // 0 line
      display.print(floatstring);
    }
    {                                                             // print bottomMargin value
      dtostrf(screenArray.Min, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 7);            // 7 line (last)
      display.print(floatstring);
    }
    {                                                          // print CURRENT value
      dtostrf(currentValue, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 3);         // 3 line (above centerline)
      //if (currentActive) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.print(floatstring);
      //if (currentActive) display.setTextColor(SSD1306_WHITE);
    }
    {                                                          // print inLimitLevel value
      dtostrf(inLimitLevel, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 4);         // 4 line (below centerline)
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
  //ESP_LOGD("screenRange=inMax-inMin=", "%.8f = %.8f - %.8f", screenRange, inMax, inMin);
  //ESP_LOGD("screenRange=", "%f", screenRange);
  if (screenRange < 0.00000005f) gridSize = 0.00000005f;
  if (screenRange > 0.00000005f) gridSize = 0.0000001f;
  if (screenRange > 0.0000001f) gridSize = 0.000005f;
  if (screenRange > 0.0000005f) gridSize = 0.000001f;
  if (screenRange > 0.000001f) gridSize = 0.000005f;
  if (screenRange > 0.000005f) gridSize = 0.00001f;
  if (screenRange > 0.00001f) gridSize = 0.00005f;
  if (screenRange > 0.00005f) gridSize = 0.0001f;
  if (screenRange > 0.0001f) gridSize = 0.0005f;
  if (screenRange > 0.0005f) gridSize = 0.001f;
  if (screenRange > 0.001f) gridSize = 0.005f;
  if (screenRange > 0.005f) gridSize = 0.01f;
  if (screenRange > 0.01f) gridSize = 0.05f;
  if (screenRange > 0.05f) gridSize = 0.01f;
  if (screenRange > 0.1f) gridSize = 0.05f;
  if (screenRange > 0.5f) gridSize = 0.1f;
  if (screenRange > 1.0f) gridSize = 0.5f;
  if (screenRange > 5.0f) gridSize = 1.0f;
  if (screenRange > 10.0f) gridSize = 5.0f;
  if (screenRange > 50.0f) gridSize = 10.0f;
  if (screenRange > 100.0f) gridSize = 50.0f;
  if (screenRange > 500.0f) gridSize = 100.0f;
  if (screenRange > 1000.0f) gridSize = 500.0f;
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

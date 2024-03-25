void logArrayInit() {  // init logArray in setup() after initing ina226
  //read_ina();
  for (int i = 0; i < 128; i++) {  // setting all log values to something that will not pull scale window
    logArray.V[i] = NAN;           //inaVal.V
    logArray.A[i] = NAN;           //inaVal.A;
    logArray.W[i] = NAN;           //inaVal.W;
    logArray.Vs[i] = NAN;          //inaVal.Vs;
    logArray.R[i] = NAN;           //1.7f;  //inaVal.R;
  }
  logArray.index = 0;
}

void logArrayProcess() {  // takes inaVal content and put it into logArray
  ESP_LOGV("LOGARRAY", "trigger: %f", trigR - inaVal.R);
  if (inaVal.R < trigR && !std::isnan(inaVal.R)) {  // process actual values if resistance is low enough to start logging
    ESP_LOGV("LOGARRAY", "triggered");
    if (logArray.index < 127) logArray.index++;  // cycle iterate logArray.index
    else logArray.index = 0;
    logArray.V[logArray.index] = inaVal.V;  //inaVal.V
    logArray.A[logArray.index] = inaVal.A;  //inaVal.A;
    logArray.W[logArray.index] = inaVal.W;
    logArray.Vs[logArray.index] = inaVal.Vs;
    logArray.R[logArray.index] = inaVal.R;
    newLogData = 1;
  } else {  // if resistance did not triggered logging
    newLogData = 0;
  }
}

void logArrayGetMargins(float logArrayInput[], float bottomVal, float topVal) {  // find maximum/minimum values of array for scaling data (if value outside top/bottom - store "0")
  ESP_LOGV("LOGARRAY", "margins");
  int arrayIndex = 0;
  screenArray.Max = NAN;
  screenArray.Min = NAN;
  for (arrayIndex = 0; arrayIndex < 128; arrayIndex++) {                                  // searching for a first value in range
    if (logArrayInput[arrayIndex] >= bottomVal && logArrayInput[arrayIndex] <= topVal) {  // store in min/max if found, then break
      screenArray.Max = logArrayInput[arrayIndex];
      screenArray.Min = logArrayInput[arrayIndex];
      break;  // breaking for() when first value found, holding arrayIndex of first good value
    }
  }
  if (arrayIndex < 127) {                               // if array isn't done yet
    for (arrayIndex; arrayIndex < 128; arrayIndex++) {  // continue to the end of array, stretching min/max
      if (screenArray.Max < logArrayInput[arrayIndex]) screenArray.Max = logArrayInput[arrayIndex];
      if (screenArray.Min > logArrayInput[arrayIndex]) screenArray.Min = logArrayInput[arrayIndex];
    }
  }
  if (isnan(screenArray.Max) || isnan(screenArray.Min)) {  // set min/max to bottom/top values if none good samples found
    ESP_LOGV("screenArray.Max=", "NAN");
    screenArray.Max = topVal;
    screenArray.Min = bottomVal;
  }
  if (screenArray.Max - screenArray.Min < MIN_WINDOW_SIZE) screenArray.Max = screenArray.Min + MIN_WINDOW_SIZE;  // setting minimum log window height
}

void screenArrayProcess(float logArrayInput[], float inMin, float inMax) {
  //logArrayMargins(logArrayInput); // find maximum/minimum values of array for scaling data
  screenmap.init(inMin, inMax, SCREEN_HEIGHT - 1, 0);  //init mapping scale to min/max window size
  int ii = 127;
  if (logArray.index < 127) {
    for (int i = logArray.index + 1; i < 128; i++) {  // starting from the oldest value (index+1)
      if (!std::isnan(logArrayInput[i])) screenArray.Val[ii] = screenmap.map(logArrayInput[i]);
      else screenArray.Val[ii] = SCREEN_HEIGHT - 1;  // having to deal with NaN values
      ii--;
    }
  }
  for (int i = 0; i < logArray.index + 1; i++) {  // continue from the start of array
    if (!std::isnan(logArrayInput[i])) screenArray.Val[ii] = screenmap.map(logArrayInput[i]);
    else screenArray.Val[ii] = SCREEN_HEIGHT - 1;  // having to deal with NaN values
    ii--;
  }
}

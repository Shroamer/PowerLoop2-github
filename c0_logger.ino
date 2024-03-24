void logArrayInit() {  // init logArray in setup() after initing ina226
  //read_ina();
  for (int i = 0; i < 128; i++) {  // setting all log values to something that will not pull scale window
    logArray.V[i] = inaVal.V;      //inaVal.V
    logArray.A[i] = inaVal.A;      //inaVal.A;
    logArray.W[i] = inaVal.W;
    logArray.Vs[i] = inaVal.Vs;
    logArray.R[i] =  1.7f;//inaVal.R;
  }
  logArray.index = 0;
}

void logArrayProcess() {  // takes inaVal content and put it into logArray
  ESP_LOGV("LOGARRAY", "trigger: ", trigR - inaVal.W);
  if (inaVal.R < trigR && inaVal.R > 0) {  // process actual values if resistance is low enough to start logging
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

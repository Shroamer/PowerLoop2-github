

void loop() {  // put your main code here, to run repeatedly:
  ESP_LOGD("loop()", "run");
  if (!flagInaReady)                              // rise flag if we miss last ALE ISR
    if (!digitalRead(INA_ALE)) flagInaReady = 1;  // INA alert is pulling DOWN pin to alert

  if (flagInaReady) {
    ESP_LOGD("VAR", "INA READY");
    flagInaReady = 0;
    //xSemaphoreTake(inaSemaphore, portMAX_DELAY);  //moved to read_ina // Take the semaphore before reading INA
    read_ina();
    //xSemaphoreGive(inaSemaphore);  //moved to read_ina // Give the semaphore after reading INA
    ledOff();
  }
  /*
  if (flagInaNewData) {
    ESP_LOGD("VAR", "NEW DATA");
    flagInaNewData = 0;
    logArrayProcess();
  }
  if (newLogData) {
    ESP_LOGD("VAR", "NEW LOG");
    logArrayGetMargins(logArray.R, 0.001f, 500000.0f);  // getting margins within r001...500k
    screenArrayProcess(logArray.R, screenArray.Min, screenArray.Max);
  }
  #ifdef BSP
    logToSerial();
  #endif
  display.clearDisplay();
  plotA(trigR);
  display.display();
  //ESP_LOGD("INA ALE PIN: ", "%d", digitalRead(INA_ALE));
  newLogData = 0;
  */
}

void display_task(void* pvParameters) {  // This is display task to run on separate core. It waits for the semaphore and then processes the log and updates the display.
  ESP_LOGD("display_task()", "run");
  while (1) {
    //if (xSemaphoreTake(inaSemaphore, 1000) == pdTRUE) {//(xSemaphoreTake(inaSemaphore, portMAX_DELAY) == pdTRUE) {  // wait maximum time for the ticket to start processing
    //ESP_LOGD("semaphore", "take");
    if (flagInaNewData) {
      ESP_LOGD("VAR", "NEW DATA");
      flagInaNewData = 0;
      logArrayProcess();
    }
    if (newLogData) {
      ESP_LOGD("VAR", "NEW LOG");
      logArrayGetMargins(logArray.R, 0.001f, 500000.0f);  // getting margins within r001...500k
      screenArrayProcess(logArray.R, screenArray.Min, screenArray.Max);
    }
#ifdef BSP
    logToSerial();
#endif
    display.clearDisplay();
    plotA(trigR);
    display.display();
    //ESP_LOGD("INA ALE PIN: ", "%d", digitalRead(INA_ALE));
    newLogData = 0;
    //xSemaphoreGive(inaSemaphore);  // Give the semaphore after reading INA
    //ESP_LOGD("semaphore", "give");
    //}
  }
}

void logToSerial() {
  Serial.print(inaVal.R, 6);
  Serial.print(VALUES_SEPARATOR);
  Serial.print(inaVal.A, 6);
  Serial.print(VALUES_SEPARATOR);
  Serial.print(inaVal.V, 6);
  Serial.print(VALUES_SEPARATOR);
  Serial.print(inaVal.W, 6);
  Serial.print(VALUES_SEPARATOR);
  Serial.println(inaVal.Vs, 6);
  ESP_LOGD("VAR", "NEW DATA");
}
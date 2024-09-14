void display_task(void* pvParameters) {  // This is display task to run on separate core. It waits for the semaphore and then processes the log and updates the display.
  ESP_LOGD("CORE0 task", "run");
  while (1) {
    //if (xSemaphoreTake(inaSemaphore, 1000) == pdTRUE) {  //(xSemaphoreTake(inaSemaphore, portMAX_DELAY) == pdTRUE) {  // wait maximum time for the ticket to start processing
      //ESP_LOGD("semaphore", "take");
      if (flagInaNewData) {
        ESP_LOGD("FLAG", "flagInaNewData");
        flagInaNewData = 0;
        logArrayProcess();
      }
      if (newLogData) {
        ESP_LOGD("FLAG", "newLogData");
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
    //vTaskDelay(pdMS_TO_TICKS(10));  // Add a short delay to yield to other tasks
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
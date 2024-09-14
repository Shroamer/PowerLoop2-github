

void loop() {  // put your main code here, to run repeatedly:
  //ESP_LOGV("loop()", "run");
  if (!flagInaReady)              // rise flag if we miss last ALE ISR
    if (!digitalRead(INA_ALE)) {  // INA alert is pulling DOWN pin to alert
      flagInaReady = 1;
      ESP_LOGD("ALE", "rising flag manually");
    }

  if (flagInaReady) {
    ESP_LOGD("FLAG", "flagInaReady");
    flagInaReady = 0;
    //xSemaphoreTake(inaSemaphore, portMAX_DELAY);  //moved to read_ina // Take the semaphore before reading INA
    read_ina();
    //xSemaphoreGive(inaSemaphore);  //moved to read_ina // Give the semaphore after reading INA
    ledOff();
  }
}

void loop() {  // put your main code here, to run repeatedly:
  //ESP_LOGD("loop()", "run");
  if (aleISRattached)                               //only when we're listening to isr
    if (!flagInaReady)                              // rise flag if we miss last ALE ISR
      if (!digitalRead(INA_ALE)) flagInaReady = 1;  // INA alert is pulling DOWN pin to alert

  if (flagInaReady) {
    ESP_LOGD("FLAG", "flagInaReady");
    flagInaReady = 0;
    //xSemaphoreTake(inaSemaphore, portMAX_DELAY);  //moved to read_ina // Take the semaphore before reading INA
    read_ina();
    //xSemaphoreGive(inaSemaphore);  //moved to read_ina // Give the semaphore after reading INA
    ledOff();
  }
  if (flagInaSingleMeasureStart) {
    ESP_LOGD("FLAG", "flagInaSingleMeasureStart");
    read_ina_single();
  }
}

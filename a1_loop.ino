

void loop() {  // put your main code here, to run repeatedly:
  if (!flagInaReady)// rise flag if we miss last ALE ISR
    if (!digitalRead(INA_ALE)) flagInaReady = 1;  // INA alert is pulling DOWN pin to alert

  if (flagInaReady) {
    ESP_LOGD("VAR", "INA READY");
    flagInaReady = 0;
    read_ina();
    ledOff();
  }
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
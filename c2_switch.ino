void switchInit() {
  pinMode(OUT_PIN, OUTPUT);
}

void switchOn() {
  digitalWrite(OUT_PIN, HIGH);
  ledOn();
  //ESP_LOGV("SWITCH", "ON");
}

void switchOff() {
  digitalWrite(OUT_PIN, LOW);
  ledOff();
  //ESP_LOGV("SWITCH", "OFF");
}

bool isSwitched() {
  return (digitalRead(OUT_PIN));
}
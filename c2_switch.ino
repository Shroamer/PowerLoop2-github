void switchInit() {
  pinMode(OUT_PIN, OUTPUT);
}

void switchOnCheck() {
  switchedOff = 0;
  inaCheck = 1;
  digitalWrite(OUT_PIN, HIGH);
  ledOn();
  ESP_LOGV("SWITCH", "CHECK");
}
void switchOn() {
  switchedOff = 0;
  digitalWrite(OUT_PIN, HIGH);
  ledOn();
  ESP_LOGV("SWITCH", "ON");
}
void switchOff() {
  switchedOff = 1;
  digitalWrite(OUT_PIN, LOW);
  ledOff();
  ESP_LOGV("SWITCH", "OFF");
}

bool isSwitched() {
  return (digitalRead(OUT_PIN));
}
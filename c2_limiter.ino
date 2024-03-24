void switchInit() {
  pinMode(OUTPIN, OUTPUT);
}

void switchOn() {
  digitalWrite(OUTPIN, HIGH);
  ledOn();
  ESP_LOGV("SWITCH", "ON");
}

void switchOff() {
  digitalWrite(OUTPIN, LOW);
  ledOff();
  ESP_LOGV("SWITCH", "OFF");
}

bool isSwitched() {
  return (digitalRead(OUTPIN));
}
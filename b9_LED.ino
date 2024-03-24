void ledInit() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
}
void ledOn() {
  digitalWrite(LEDPIN, HIGH);
}
void ledOff() {
  digitalWrite(LEDPIN, LOW);
}
void ledToggle() {
  digitalWrite(LEDPIN, !digitalRead(LEDPIN));
}
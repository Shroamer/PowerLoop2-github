/*void IRAM_ATTR idleTimerISR() {  // ideling is over - time to check values
  ESP_LOGD("ISR", "timer");
  switchOnCheck();                          // switching on
  timerStop(idleTimer);                     // stop timer
  inaAleAttachInterrupt();                  // + attach ale interrupt to read when ALE ready
  //ina226.startSingleMeasurementNoWait();    // request a single measurement
  //ina226.startSingleMeasurement();        // request a single measurement
  flagInaSingleMeasureStart = 1;
  ESP_LOGV("timeISR", "ISR run");
  // + clear flags and read
}

void initTimer() {                      // run once in setup()
  idleTimer = timerBegin(0, 80, true);  // Use timer 0, with a prescaler of 80, forward
  timerAttachInterrupt(idleTimer, &idleTimerISR, true);
  timerAlarmWrite(idleTimer, timeToIdle, false);
  timerWrite(idleTimer, 0);
  //timerStart(idleTimer);
  //timerAlarmEnable(idleTimer);
}

void startIdling(u_int64_t microSeconds) {  // run timer for microSeconds
  inaAleDetachInterrupt();               // + DETACH ALE INTERRUPT
  ina226.setMeasureMode(TRIGGERED);         // + INA TRIGGERED MODE
  u_int64_t inaReadTime = getInaReadTime();
  if (microSeconds > inaReadTime + 2500) microSeconds -= inaReadTime;  // ? adjust idler delay - ina read time to compensate over long conversion modes
  // Start the timer
  ESP_LOGV("IDLER", "START for %d us", microSeconds);
  timerWrite(idleTimer, 0);
  ESP_LOGV("IDLER", "timer value: %d", timerRead(idleTimer));
  timerAlarmWrite(idleTimer, microSeconds, false);
  ESP_LOGV("IDLER", "alarm writen");
  timerAlarmEnable(idleTimer);
  ESP_LOGV("IDLER", "alarm enabled");
  timerStart(idleTimer);
  ESP_LOGV("IDLER", "timer RUN, switching off");
  switchOff();  // switching off
}
*/
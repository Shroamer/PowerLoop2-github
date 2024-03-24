void IRAM_ATTR inaAlertISR() {  //  INA226 set ALERT flag, informing it has new data to share
  /*BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(sensorSemaphore, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }*/
  flagInaReady = 1;
  ledOn();
  inaAleDetachInterrupt();  // detaching interrupt untill we read data? should we stop ina?
}

void setup() {  // put your setup code here, to run once:
#ifndef SERIAL256000
  Serial.begin(115200);
#endif
#ifdef SERIAL256000
  Serial.begin(256000);
#endif
  init_display();
  Wire.begin();  // for ina226 i2c bus
  init_ina();
  read_ina();      // read ina before initing log with it's values
  logArrayInit();  //ina values must be read before initing array
  newLogData = 0;
  ledInit();
  switchInit();
  switchOn();
  inaInrettuptInit();  // right before loop() starts
}
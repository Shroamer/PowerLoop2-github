// https://github.com/wollewald/INA226_WE/tree/master

void init_ina() {
  if (!ina226.init()) {
    Serial.println("Failed to init INA226. Check your wiring.");
    while (1) {}
  }

  /* Set Number of measurements for shunt and bus voltage which shall be averaged
    Mode *     * Number of samples
    AVERAGE_1            1 (default)
    AVERAGE_4            4
    AVERAGE_16          16
    AVERAGE_64          64
    AVERAGE_128        128
    AVERAGE_256        256
    AVERAGE_512        512
    AVERAGE_1024      1024
  */
  //ina226.setAverage(AVERAGE_1); // choose mode and uncomment for change of default
  setInaAvg(inaAverageSamples);

  /* Set conversion time in microseconds
     One set of shunt and bus voltage conversion will take:
     number of samples to be averaged x conversion time x 2

       Mode *         * conversion time
     CONV_TIME_140          140 µs
     CONV_TIME_204          204 µs
     CONV_TIME_332          332 µs
     CONV_TIME_588          588 µs
     CONV_TIME_1100         1.1 ms (default)
     CONV_TIME_2116       2.116 ms
     CONV_TIME_4156       4.156 ms
     CONV_TIME_8244       8.244 ms
  */
  //ina226.setConversionTime(CONV_TIME_1100); //choose conversion time and uncomment for change of default
  setConvTime(inaConvTime);

  /* Set measure mode
    POWER_DOWN - INA226 switched off
    TRIGGERED  - measurement on demand
    CONTINUOUS  - continuous measurements (default)
  */
  ina226.setMeasureMode(CONTINUOUS);  // choose mode and uncomment for change of default

  /* Set Resistor and Current Range
     resistor is 5.0 mOhm
     current range is up to 10.0 A
     default was 100 mOhm and about 1.3 A
  */
  ina226.setResistorRange(0.01, 8.0);  // choose resistor r01 and gain range up to 8 A

  /* If the current values delivered by the INA226 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA226
  */
  // ina226.setCorrectionFactor(0.95);

  ina226.waitUntilConversionCompleted();  //if you comment this line the first data might be zero
  ina226.enableConvReadyAlert();          // an interrupt will occur on interrupt pin when conversion is ready
}

void setInaAvg(int inInaAvg) {  // sets INA averaging as 0..7 (1/4/16/64/128/256/512/1024 x)
  switch (inInaAvg) {
    case 0:
      ina226.setAverage(AVERAGE_1);
      break;
    case 1:
      ina226.setAverage(AVERAGE_4);
      break;
    case 2:
      ina226.setAverage(AVERAGE_16);
      break;
    case 3:
      ina226.setAverage(AVERAGE_64);
      break;
    case 4:
      ina226.setAverage(AVERAGE_128);
      break;
    case 5:
      ina226.setAverage(AVERAGE_256);
      break;
    case 6:
      ina226.setAverage(AVERAGE_512);
      break;
    case 7:
      ina226.setAverage(AVERAGE_1024);
      break;
    default:
      break;
  }
}

void setConvTime(int inConvTime) {  // sets INA conversion time as 0..7 (140/204/332/588/1100*/2116/4156/8244 µs)
  switch (inConvTime) {
    case 0:
      ina226.setConversionTime(CONV_TIME_140);  // 140 uS
      break;
    case 1:
      ina226.setConversionTime(CONV_TIME_204);
      break;
    case 2:
      ina226.setConversionTime(CONV_TIME_332);
      break;
    case 3:
      ina226.setConversionTime(CONV_TIME_588);
      break;
    case 4:
      ina226.setConversionTime(CONV_TIME_1100);
      break;
    case 5:
      ina226.setConversionTime(CONV_TIME_2116);
      break;
    case 6:
      ina226.setConversionTime(CONV_TIME_4156);
      break;
    case 7:
      ina226.setConversionTime(CONV_TIME_8244);
      break;
    default:
      break;
  }
}

u_int64_t getInaReadTime() {  // return estimated sampling time (uS) by multiplying actual ConversionTime * AverageSamples
  return ((long)ctValue[inaConvTime] * avgValue[inaAverageSamples]);
}

//==================== ALE PIN INTERRUPT ====================

// https://microcontrollerslab.com/esp32-external-interrupts-tutorial-arduino-ide/
void inaInrettuptInit() {  // setting pin mode and attaching ina ale interrupt
  pinMode(INA_ALE, INPUT);
  //ESP_LOGV("ALE-ISR", "detach");
  inaAleAttachInterrupt();
}

void inaAleAttachInterrupt() {
  //ina226.readAndClearFlags(); // suppose there's sometimes ALE pin left HIGH, then interrupt stops working
  //flagInaReady = 0;
  //ESP_LOGV("ALE-ISR", "attach");
  attachInterrupt(INA_ALE, inaAlertISR, FALLING);
  void enableConvReadyAlert();
  //attachInterrupt(INA_ALE, inaAlertISR, LOW);
}

void inaAleDetachInterrupt() {
  void disableConvReadyAlert();
  detachInterrupt(INA_ALE);
}

//==================== INA226 OPERATIONS ====================

void read_ina() {  // clear ale flag, read ina226, store in memory, rise new data flag
  ina226.readAndClearFlags();
  inaImportValues();
  flagInaNewData = 1;
  inaAleAttachInterrupt();  // reattach interrupt after readings are done (not sure if that's a right place to reattch interrupt)
}

void inaImportValues() {  // put values read into corresponding variables
                          //if (xSemaphoreTake(inaSemaphore, 1000) == pdTRUE) {  // Take the semaphore before reading INA (portMAX_DELAY - maximum time wait)
  //ESP_LOGD("semaphore", "take");
  inaVal.V = ina226.getBusVoltage_V();          // V
  inaVal.A = ina226.getCurrent_mA() / 1000.0f;  // µA = mA/1000
  inaVal.W = ina226.getBusPower() / 1000.0f;    // mW = W/1000 (?)
  inaVal.Vs = ina226.getShuntVoltage_mV();      // mV
  //ESP_LOGV("INA", "getting resistance...");     // debug in
  if (inaVal.V != 0 && inaVal.A != 0) {  // open loop check. legacy:(inaVal.V > NO_CURRENT && inaVal.A > NO_CURRENT)
    inaVal.R = inaVal.V / inaVal.A;      // calculate resistance R=V/I
  } else inaVal.R = OPEN_LOOP;           // setting "open loop" value
  //xSemaphoreGive(inaSemaphore);          // Give the semaphore after reading INA
  //ESP_LOGD("semaphore", "give");
  //ESP_LOGV("R=V/I", "%f = %f / %f", inaVal.R, inaVal.V, inaVal.A);               // debug out
  if (ina226.overflow) Serial.println("Overflow! Choose higher current range");  // overflow alert
                                                                                 //ESP_LOGV("INA READ", "values: %fR %fA  %fV  %fW  %fVs", inaVal.R, inaVal.A, inaVal.V, inaVal.W, inaVal.Vs);
  //}
}

void inaContinuousMode() {  //
  inaCheck = 0;
  ina226.setMeasureMode(CONTINUOUS);  // + CONTINUOUS MODE
  ina226.readAndClearFlags();
}

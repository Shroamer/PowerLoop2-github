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
  //inaSemaphore = xSemaphoreCreateBinary();                                         // Create the semaphore.
  //xSemaphoreGive(inaSemaphore);                                                    // Give the semaphore initially, so the display task can take it.
  //ESP_LOGD("semaphore", "give");
  ESP_LOGD("INIT", "display_task");
  xTaskCreatePinnedToCore(display_task, "Display Task", 16 * 1024, NULL, 1, NULL, 0);  // Create the display task on core 1.
                                                                                       //display_task: This is a pointer to the function that implements the task. The function must be implemented to never return (i.e., it should contain an infinite loop), or it should be terminated using the vTaskDelete function.
                                                                                       //"Display Task": This is a descriptive name for the task. It’s mainly used to facilitate debugging.
                                                                                       //16*1024: This is the size of the task stack specified as the number of bytes.
                                                                                       //NULL: This is a pointer that will be used as the parameter for the task being created.
                                                                                       //1: This is the priority at which the task should run. Tasks with higher numbers run before tasks with lower numbers.
                                                                                       //NULL: This is used to pass back a handle by which the created task can be referenced.
                                                                                       //0: This is the core ID. If the value is tskNO_AFFINITY, the created task is not pinned to any CPU, and the scheduler can run it on any core available. Values 0 or 1 indicate the index number of the CPU which the task should be pinned to.
  ESP_LOGD("INIT", "display");
  init_display();
  ESP_LOGD("INIT", "Wire");
  Wire.begin();  // for ina226 i2c bus
  ESP_LOGD("INIT", "init_ina");
  init_ina();
  read_ina();      // read ina before initing log with it's values
  ESP_LOGD("INIT", "logArray");
  logArrayInit();  //ina values must be read before initing array
  newLogData = 0;
  ESP_LOGD("INIT", "LED");
  ledInit();
  ESP_LOGD("INIT", "switch");
  switchInit();
  switchOn();
  ESP_LOGD("INIT", "timer");
  //initTimer();
  ESP_LOGD("INIT", "ale interrupt");
  inaInrettuptInit();  // right before loop() starts
}
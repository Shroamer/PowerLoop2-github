
void limiterRoutine() {
  if (!switchedOff) {
    //ESP_LOGV("LIM", "limitMode: %d", limitMode);
    if (limitMode) {  // Resistance limiting mode
      ESP_LOGV("LIM", "R.dif: %f", limitR - inaVal.R, 6);
      if (inaVal.R < limitR) {  // resistance is still to low
        /*modifyIdleTime();
        startIdling(timeToIdle);*/
      } else if (inaCheck) {
        /*inaContinuousMode();*/
      }
    }
  }
}

void modifyIdleTime() {
  if (!inaCheck) {  // when we're overheat for the 1st time
    //if (timeToIdle > BASE_TIMETOIDLE) timeToIdle -= BASE_TIMETOIDLE;
    //else timeToIdle = BASE_TIMETOIDLE;
    timeToIdle = BASE_TIMETOIDLE;
  } else {  // when we were overheat again
    timeToIdle += BASE_TIMETOIDLE;
  }
}

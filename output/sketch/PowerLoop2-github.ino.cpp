#include <Arduino.h>
#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\PowerLoop2-github.ino"
/*
r2.0.0
testing resistive capabilities on existing hardware
*/

//==================== TEMPORARY DEFINES  ====================
#define VALUES_SEPARATOR " "  // define separator to print serial data into plotter


//#define BSP // uncomment for betterSerialTerminal output at 256000bps
#define SPLASHSCREEN_TIMEOUT 500 // Splashscreen visible timeout. comment for no splashscreen
#define REV_NO "2.0.2"
#define REV_DATE "2024.03.19"
#define SPLASH_MESSAGE "adding controls"

//==================== ESP32 PIN DEFINITIONS ====================
#define OUT_PIN 27     // pin connected to power mosfet module that switches main load
#define LED_PIN 2      // internal LED pin
#define INA_ALE 32  // connected to INA226 ALE pin, which alerts about new readings available

//==================== ESP32 HARDWARE ====================
#include <SPI.h>   // for ssd1306 spi display
#include <Wire.h>  // for ina226 voltage/current i2c monitor


//==================== INA226 V/A meter ====================

#include <INA226_WE.h>  // https://github.com/wollewald/INA226_WE/tree/master
#define INA_ADDRESS 0x40
/* There are several ways to create your INA226 object:
   INA226_WE ina226 = INA226_WE(); -> uses I2C Address = 0x40 / Wire
   INA226_WE ina226 = INA226_WE(I2C_ADDRESS);
   INA226_WE ina226 = INA226_WE(&Wire); -> uses I2C_ADDRESS = 0x40, pass any Wire Object
   INA226_WE ina226 = INA226_WE(&Wire, I2C_ADDRESS);
*/
INA226_WE ina226 = INA226_WE(&Wire, INA_ADDRESS);

int inaConvTime = 5;                                              // store conversion time 0...7 (140/204/332/588/1100*/2116/4156/8244 µs)
int ctValue[8] = { 140, 204, 332, 588, 1100, 2116, 4156, 8244 };  // storing these values for convenient use ctValue[inaConvTime]
int inaAverageSamples = 2;                                        // store ina averaging 0...7 (1/4/16/64/128/256/512/1024 x)
int avgValue[8] = { 1, 4, 16, 64, 128, 256, 512, 1024 };          // storing these values for convenient use avgValue[inaAverageSamples]

#define OPEN_LOOP 1000000.0f

volatile bool flagInaReady = 0;    // ina226 data is ready to be read - raised by ina226 via ISR
volatile bool flagInaNewData = 0;  // raised after ina226 data registers are read to run data storing routine
volatile bool inaCheck = 0;        // true when we're turned switch on only to check values

struct inaDataStruct {  //standard power line data sample
  float V;              // Bus voltage     (V)   ina226 register data
  float A;              // Bus current     (uA)  ina226 register data
  float W;              // Bus power       (uW)  W=V*A - ina226 register data
  float Vs;             // Shunt voltage   (V)   ina226 register data
  float R;              // Load resistance (Ohm) R=V/I - calculated locally
};
inaDataStruct inaVal;  // here we'll keep all latest INA values


//==================== SSD1306 128x64 OLED SPI DISPLAY ====================

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels//

#define OLED_CLK 18
//#define OLED_MOSI  23
#define OLED_RST 17
#define OLED_DC 16
#define OLED_CS 26
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS);  //Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);


//==================== LOGGER ====================

struct inaLog {
  int index = 0;
  float V[128];
  float A[128];
  float W[128];
  float Vs[128];
  float R[128];
} logArray;  // storing 128 last values for screen log + index (last position)
//logArray[128];  // logged data logArray[logArray.index].V
//int logArray.index = 0;
bool newLogData = 0;  // flag if there's update in logger data
float trigR = 2.0f;   // resistance to trigger logging
#define MIN_WINDOW_SIZE 0.00001f * 64.0f

//==================== PLOTTER ====================
#include "FastMap.h"  // used in plotter for fast map() of floats
FastMap screenmap;

#define VAL_R_PREC 6    // resistance precission (digits after point)
#define VAL_R_DIGITS 9  // maximuim length of value including negative sign, digits before and after dot, dot itself: VAL_R_PREC + 3 // -1.000000
#define VAL_A_PREC 6
#define VAL_A_DIGITS 9  // VAL_A_PREC + 3 // -1.000000
#define VAL_V_PREC 5
#define VAL_V_DIGITS 9  // VAL_V_PREC + 4 // -10.000000
#define VAL_W_PREC 6
#define VAL_W_DIGITS 11  // VAL_W_PREC + 5 // -100.000000
#define VAL_Vs_PREC 6
#define VAL_Vs_DIGITS 9  // VAL_Vs_PREC + 3 // -1.000000
#define VAL_Trig_PREC 6
#define VAL_Trig_DIGITS 11  // VAL_Trig_PREC + 5 // -100.000000

struct screenData {
  byte Val[128];
  float Min;
  float Max;
} screenArray;  // storing log in a screen-dimension format for easy plotting

struct lineCoords {
  byte X;
  byte Y;
};

#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\a0_setup.ino"
void IRAM_ATTR inaAlertISR() {  //  INA226 set ALERT flag, informing it has new data to share
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
  read_ina();  // read ina before initing log with it's values
  logArrayInit();  //ina values must be read before initing array
  newLogData = 0;
  ledInit();
  switchInit();
  switchOn();
  inaInrettuptInit();  // right before loop() starts
}
#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\a1_loop.ino"
void loop() {  // put your main code here, to run repeatedly:
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
#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\b1_ina226.ino"
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
  ESP_LOGV("ALE-ISR", "detach");
  inaAleAttachInterrupt();
}

void inaAleAttachInterrupt() {
  //ina226.readAndClearFlags(); // suppose there's sometimes ALE pin left HIGH, then interrupt stops working
  //flagInaReady = 0;
  ESP_LOGV("ALE-ISR", "attach");
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

void inaImportValues() {                        // put values read into corresponding variables
  inaVal.V = ina226.getBusVoltage_V();          // V
  inaVal.A = ina226.getCurrent_mA() / 1000.0f;  // µA = mA/1000
  inaVal.W = ina226.getBusPower() / 1000.0f;    // mW = W/1000 (?)
  inaVal.Vs = ina226.getShuntVoltage_mV();      // mV
  //ESP_LOGV("INA", "getting resistance...");     // debug in
  if (inaVal.V != 0 && inaVal.A != 0) {  // open loop check
    inaVal.R = inaVal.V / inaVal.A;      // calculate resistance R=V/I
  } else inaVal.R = OPEN_LOOP;               // setting "open loop" value
  ESP_LOGV("R=V/I", "%f = %f / %f", inaVal.R,inaVal.V,inaVal.A);     // debug out
  if (ina226.overflow) Serial.println("Overflow! Choose higher current range");  // overflow alert
  ESP_LOGV("INA READ", "values: %fR %fA  %fV  %fW  %fVs", inaVal.R, inaVal.A, inaVal.V, inaVal.W, inaVal.Vs);
}

void inaContinuousMode() {  //
  inaCheck = 0;
  ina226.setMeasureMode(CONTINUOUS);  // + CONTINUOUS MODE
  ina226.readAndClearFlags();
}

#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\b2_ssd1306.ino"
void init_display() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  ESP_LOGV("SSD1306", "display.began");

  //display.display();
  //delay(1000);

  display.clearDisplay();  // Clear the buffer
  display.cp437(true);     // Use full 256 char 'Code Page 437' font
  display.setTextWrap(false);
#ifdef SPLASHSCREEN_TIMEOUT
  printVersion();
  display.display();
  delay(SPLASHSCREEN_TIMEOUT);
#endif
  display.setTextSize(1);       // Normal 1:1 pixel scale
  display.setTextColor(WHITE);  // Draw white text
  display.setCursor(0, 0);      // Start at top-left corner
  //display.invertDisplay(true);  // invert display image, no need to display.display
  display.clearDisplay();
  display.display();
}

void printVersion() {
  display.fillRect(0, 0, 128, 17, WHITE);
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(5, 1);
  display.println("PowerLoop2");
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 18);
  display.print("r");
  display.print(REV_NO);
  display.print(", ");
  display.println(REV_DATE);
  display.setCursor(0, 8 * 4);
  display.setTextWrap(true);
  display.fillRect(0, 26, 128, 4, WHITE);
  display.println(SPLASH_MESSAGE);
  display.setTextWrap(false);
}
#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\b9_LED.ino"
void ledInit() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}
void ledOn() {
  digitalWrite(LED_PIN, HIGH);
}
void ledOff() {
  digitalWrite(LED_PIN, LOW);
}
void ledToggle() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}
#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\c0_logger.ino"
void logArrayInit() {  // init logArray in setup() after initing ina226
  //read_ina();
  for (int i = 0; i < 128; i++) {  // setting all log values to something that will not pull scale window
    logArray.V[i] = inaVal.V;      //inaVal.V
    logArray.A[i] = inaVal.A;      //inaVal.A;
    logArray.W[i] = inaVal.W;
    logArray.Vs[i] = inaVal.Vs;
    logArray.R[i] =  1.7f;//inaVal.R;
  }
  logArray.index = 0;
}

void logArrayProcess() {  // takes inaVal content and put it into logArray
  ESP_LOGV("LOGARRAY", "trigger: ", trigR - inaVal.W);
  if (inaVal.R < trigR && inaVal.R > 0) {  // process actual values if resistance is low enough to start logging
    ESP_LOGV("LOGARRAY", "triggered");
    if (logArray.index < 127) logArray.index++;  // cycle iterate logArray.index
    else logArray.index = 0;
    logArray.V[logArray.index] = inaVal.V;  //inaVal.V
    logArray.A[logArray.index] = inaVal.A;  //inaVal.A;
    logArray.W[logArray.index] = inaVal.W;
    logArray.Vs[logArray.index] = inaVal.Vs;
    logArray.R[logArray.index] = inaVal.R;
    newLogData = 1;
  } else {  // if resistance did not triggered logging
    newLogData = 0;
  }
}

#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\c1_plotter.ino"
void logArrayGetMargins(float logArrayInput[], float bottomVal, float topVal) {  // find maximum/minimum values of array for scaling data (if value outside top/bottom - store "0")
  ESP_LOGV("LOGARRAY", "margins");
  int arrayIndex = 0;
  screenArray.Max = NAN;
  screenArray.Min = NAN;
  for (arrayIndex = 0; arrayIndex < 128; arrayIndex++) {                                  // searching for a first value in range
    if (logArrayInput[arrayIndex] >= bottomVal && logArrayInput[arrayIndex] <= topVal) {  // store in min/max if found, then break
      screenArray.Max = logArrayInput[arrayIndex];
      screenArray.Min = logArrayInput[arrayIndex];
      break;  // breaking for() when first value found, holding arrayIndex of first good value
    }
  }
  if (arrayIndex < 127) {                               // if array isn't done yet
    for (arrayIndex; arrayIndex < 128; arrayIndex++) {  // continue to the end of array, stretching min/max
      if (screenArray.Max < logArrayInput[arrayIndex]) screenArray.Max = logArrayInput[arrayIndex];
      if (screenArray.Min > logArrayInput[arrayIndex]) screenArray.Min = logArrayInput[arrayIndex];
    }
  }
  if (isnan(screenArray.Max) || isnan(screenArray.Min)) {  // set min/max to bottom/top values if none good samples found
    ESP_LOGV("screenArray.Max=", "NAN");
    screenArray.Max = topVal;
    screenArray.Min = bottomVal;
  }
  if (screenArray.Max - screenArray.Min < MIN_WINDOW_SIZE) screenArray.Max = screenArray.Min + MIN_WINDOW_SIZE;  // setting minimum log window height
}

void screenArrayProcess(float logArrayInput[], float inMin, float inMax) {
  //logArrayMargins(logArrayInput); // find maximum/minimum values of array for scaling data
  screenmap.init(inMin, inMax, SCREEN_HEIGHT - 1, 0);  //init mapping scale to min/max window size
  int ii = 127;
  if (logArray.index < 127) {
    for (int i = logArray.index + 1; i < 128; i++) {  // starting from the oldest value (index+1)
      screenArray.Val[ii] = screenmap.map(logArrayInput[i]);
      ii--;
    }
  }
  for (int i = 0; i < logArray.index + 1; i++) {  // continue from the start of array
    screenArray.Val[ii] = screenmap.map(logArrayInput[i]);
    ii--;
  }
}

void plotA(float inLimitLevel) {
  //display.clearDisplay();
  char floatstring[12];  // to line up values by right side
  float currentValue;
  float numPrec;
  bool currentActive = 0;
  int numLenght = 0;

  currentValue = inaVal.R;
  numLenght = VAL_R_DIGITS;
  numPrec = VAL_R_PREC;

  //ESP_LOGD("Min-Max=", "%.8f - %.8f", screenArray.Min, screenArray.Max);
  float gridSize = findGridSize(screenArray.Min, screenArray.Max);  // find grid size from range
  //ESP_LOGD("gridSize=", "%.8f", gridSize);
  screenmap.init(screenArray.Min, screenArray.Max, SCREEN_HEIGHT - 1, 0);  // init mapping scale;
  for (int i = 0; i < 11; ++i) {                                           // drawing grid lines
    float line = findGridLine(screenArray.Min, screenArray.Max, gridSize, i);
    if (isnan(line)) {
      //ESP_LOGD("grid", "-no line-");
      break;
    }
    //Serial.println(line);
    //ESP_LOGD("grid", "Line #%d at %f", i, line, 6);
    byte lineVal = screenmap.map(line);  // mapping inbound level line
    for (int x = 0; x < 128; x += 10) {  // draw line line
      display.drawPixel(x, lineVal, SSD1306_INVERSE);
    }
  }

  if (inLimitLevel < screenArray.Max && inLimitLevel > screenArray.Min) {  // draw a limit line if limit is within screen area
    //screenmap.init(screenArray.Min, screenArray.Max, SCREEN_HEIGHT - 1, 0);  // init mapping scale
    byte limitVal = screenmap.map(inLimitLevel);  // mapping inbound level line
    for (int x = 0; x < 128; x += 4) {            // draw inLimitLevel line
      display.drawPixel(x, limitVal, SSD1306_INVERSE);
    }
  }

  {  // printing min, max, current, limit values
    display.setTextSize(1);
    {                                                             // print topMargin value
      dtostrf(screenArray.Max, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 0);            // 0 line
      display.print(floatstring);
    }
    {                                                             // print bottomMargin value
      dtostrf(screenArray.Min, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 7);            // 7 line (last)
      display.print(floatstring);
    }
    {                                                          // print CURRENT value
      dtostrf(currentValue, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 3);         // 3 line (above centerline)
      //if (currentActive) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.print(floatstring);
      //if (currentActive) display.setTextColor(SSD1306_WHITE);
    }
    {                                                          // print inLimitLevel value
      dtostrf(inLimitLevel, numLenght, numPrec, floatstring);  // putting value into string
      display.setCursor(128 - (6 * numLenght), 8 * 4);         // 4 line (below centerline)
      display.print(floatstring);
    }
  }

  for (int i = 127; i >= 0; i--) {  // plotting screenArray along the screen
    display.fillRect(i, screenArray.Val[i], 1, SCREEN_HEIGHT, SSD1306_INVERSE);
  }
  //ESP_LOGD("PLOT", "OK");
  //display.display();
}

float findGridSize(float inMin, float inMax) {
  float screenRange = inMax - inMin;
  float gridSize;
  //ESP_LOGD("screenRange=inMax-inMin=", "%.8f = %.8f - %.8f", screenRange, inMax, inMin);
  //ESP_LOGD("screenRange=", "%f", screenRange);
  if (screenRange < 0.00000005f) gridSize = 0.00000005f;
  if (screenRange > 0.00000005f) gridSize = 0.0000001f;
  if (screenRange > 0.0000001f) gridSize = 0.000005f;
  if (screenRange > 0.0000005f) gridSize = 0.000001f;
  if (screenRange > 0.000001f) gridSize = 0.000005f;
  if (screenRange > 0.000005f) gridSize = 0.00001f;
  if (screenRange > 0.00001f) gridSize = 0.00005f;
  if (screenRange > 0.00005f) gridSize = 0.0001f;
  if (screenRange > 0.0001f) gridSize = 0.0005f;
  if (screenRange > 0.0005f) gridSize = 0.001f;
  if (screenRange > 0.001f) gridSize = 0.005f;
  if (screenRange > 0.005f) gridSize = 0.01f;
  if (screenRange > 0.01f) gridSize = 0.05f;
  if (screenRange > 0.05f) gridSize = 0.01f;
  if (screenRange > 0.1f) gridSize = 0.05f;
  if (screenRange > 0.5f) gridSize = 0.1f;
  if (screenRange > 1.0f) gridSize = 0.5f;
  if (screenRange > 5.0f) gridSize = 1.0f;
  if (screenRange > 10.0f) gridSize = 5.0f;
  if (screenRange > 50.0f) gridSize = 10.0f;
  if (screenRange > 100.0f) gridSize = 50.0f;
  if (screenRange > 500.0f) gridSize = 100.0f;
  if (screenRange > 1000.0f) gridSize = 500.0f;
  return (gridSize);
}

float findGridLine(float windowMin, float windowMax, float gridSize, int skipSeed) {
  float firstLine = ceil(windowMin / gridSize) * gridSize;  // Calculate the position of the first grid line within the window
  float line = firstLine + skipSeed * gridSize;             // Skip the specified number of lines
  if (line <= windowMax) {                                  // If the line is within the window, return its position
    return line;
  }
  return NAN;  // If no line was found within the window, return NAN
}

#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\c2_limiter.ino"
void switchInit() {
  pinMode(OUT_PIN, OUTPUT);
}

void switchOn() {
  digitalWrite(OUT_PIN, HIGH);
  ledOn();
  ESP_LOGV("SWITCH", "ON");
}

void switchOff() {
  digitalWrite(OUT_PIN, LOW);
  ledOff();
  ESP_LOGV("SWITCH", "OFF");
}

bool isSwitched() {
  return (digitalRead(OUT_PIN));
}
#line 1 "C:\\Users\\shroa\\Documents\\Arduino\\PowerLoop2-github\\z9_BOARD.ino"
/*
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/_images/esp32-devkitC-v4-pinout.png
          CONNECTED   FEATURE  GPIO PIN       PIN GPIO  FEATURE   CONNECTED
                                    _____________
                   |         |[3v3]|1          38|[GND]|         |
                   |         | RST |2          37|  23 | V_MOSI  | OLED_MOSI
                   |         | 36  |3          36|  22 | I2C_SCL | INA_SCL
             EN_SW |         | 39  |4          35|   1 |         |
              EN_A |         | 34  |5          34|   3 |         |
              EN_B |         | 35  |6          33|  21 | I2C_SDA | INA_SDA
           INA_ALE |         | 32  |7          32|[GND]|         |
                   |         | 33  |8          31|  19 | V_MISO  |
                   |         | 25  |9          30|  18 | V_SCK   | OLED_CLK
           OLED_CS |         | 26  |10         29|   5 | V_SS    |
            OUT_PIN |         | 27  |11         28|  17 |         | OLED_RST
                   | H_SCK   | 14  |12         27|  16 |         | OLED_DC
                   | H_MISO  | 12  |13         26|   4 |         | 
                   |         |[GND]|14         25|   0 |         | 
                   | H_MOSI  | 13  |15         24|   2 | INT_LED | LED_PIN
                   |         | 9   |16         23|  15 | H_SS    |
                   |         | 10  |17         22|   8 |         | 
                   |         | 11  |18         21|   7 |         | 
                   |         |[VIN]|19  _____  20|   6 |         | 
                                         USB
*/


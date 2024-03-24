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
#define OUTPIN 27     // pin connected to power mosfet module that switches main load
#define LEDPIN 2      // internal LED pin
#define ALERT_PIN 32  // connected to INA226 ALE pin, which alerts about new readings available

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
#define OLED_RESET 17
#define OLED_DC 16
#define OLED_CS 26
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);  //Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


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

/* ESP32 DevKit Module
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/_images/esp32-devkitC-v4-pinout.png
*          CONNECTED   FEATURE  GPIO PIN       PIN GPIO  FEATURE   CONNECTED
*                                    _____________
*                   |         |[3v3]|1          38|[GND]|         |
*                   |         |[RST]|2          37|  23 | V_MOSI  | OLED_MOSI
*                   |         | 36  |3          36|  22 | I2C_SCL | INA_SCL
*             EN_SW |         | 39  |4          35|   1 |         |
*              EN_A |         | 34  |5          34|   3 |         |
*              EN_B |         | 35  |6          33|  21 | I2C_SDA | INA_SDA
*           INA_ALE |         | 32  |7          32|[GND]|         |
*                   |         | 33  |8          31|  19 | V_MISO  |
*                   |         | 25  |9          30|  18 | V_SCK   | OLED_CLK
*           OLED_CS |         | 26  |10         29|   5 | V_SS    |
*           OUT_PIN |         | 27  |11         28|  17 |         | OLED_RST
*                   | H_SCK   | 14  |12         27|  16 |         | OLED_DC
*                   | H_MISO  | 12  |13         26|   4 |         | 
*                   |         |[GND]|14         25|   0 |         | 
*                   | H_MOSI  | 13  |15         24|   2 | INT_LED | LED_PIN
*                   |         | 9   |16         23|  15 | H_SS    |
*                   |         | 10  |17         22|   8 |         | 
*                   |         | 11  |18         21|   7 |         | 
*                   |         |[VIN]|19  _____  20|   6 |         | 
*                                         USB
*/



/* ESP32 D1 mini
https://nettigo.eu/system/images/3869/original.png?1619638105

       | ------- |[GND]|[RST]| ------- |                                 |     TXD |   1 |[GND]| ------- |
       | ------- | N/C | 36  | SVP     |                                 |     RXD |   3 | 27  |         | 
       |     SVN |  39 | 26  |         | OLED_CS                 INA_SCL | I2C_SCL |  22 | 25  |         |
 EN_SW |         |  35 | 18  | V_SCK   | OLED_CLK                INA_SDA | I2C_SDA |  21 | 32  |         | 
  EN_B |         |  33 | 19  | V_MISO  | OLED_RST                INA_ALE |         |  17 | 12  | TDI     |
  EN_A |         |  34 | 23  | V_MOSI  | OLED_MOSI               OUT_PIN |         |  16 | 4   |         |
       |     TMS |  14 | 5   | V_SS    | OLED_DC                         | ------- |[GND]| 0   |         |
       | ------- | N/C |[3v3]| ------- |                                 | ------- |[VCC]| 2   | INT_LED | LED_PIN
       |   FL_D2 |   9 | 13  | TCK     |                                 |     TD0 |  15 | 8   | FL_D1   | 
       |     CMD |  11 | 10  | FL_D3   |              _____              |   FL_D0 |  17 | 6   | FL_CLK  | 
*                                                      USB
*/
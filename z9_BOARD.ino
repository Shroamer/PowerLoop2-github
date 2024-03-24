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
            OUTPIN |         | 27  |11         28|  17 |         | OLED_RST
                   | H_SCK   | 14  |12         27|  16 |         | OLED_DC
                   | H_MISO  | 12  |13         26|   4 |         | 
                   |         |[GND]|14         25|   0 |         | 
                   | H_MOSI  | 13  |15         24|   2 | INT_LED | LEDPIN
                   |         | 9   |16         23|  15 | H_SS    |
                   |         | 10  |17         22|   8 |         | 
                   |         | 11  |18         21|   7 |         | 
                   |         |[VIN]|19  _____  20|   6 |         | 
                                         USB
*/

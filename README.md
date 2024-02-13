# Metro M4 with WM8960
 Interfacing Metro M4 board to the WM8960 I2S Audio Codec


Requires I2C connections and I2S:
 
| Metro M4 | WM8960 |
| --- | --- |
 |   3.3  |     3V3 |
|   5V |       VIN |
 |   GND |      GND |
|   SDA   |    SDA |
|   SCL   |    SCL |
|   3  |       BCLK |
|   9   |      ALRC and DLRC |
|   8   |      DDAT |
|  1 |        ADAT |
   
   Audio:
 *   LIN1/RIN1 - inputs
 *   GND       - input common
 *   HPL/HPR   - outputs (headphones)
 *   OUT3      - output common

Note: Code samples are based on examples from the SparkFun WM8960 Arduino Library [1] and the Adafruit ZeroI2S Library [2].
[1] https://github.com/sparkfun/SparkFun_WM8960_Arduino_Library
[2] https://github.com/adafruit/Adafruit_ZeroI2S



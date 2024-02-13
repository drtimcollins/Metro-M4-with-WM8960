# Metro M4 with WM8960
 Interfacing Metro M4 board to the WM8960 I2S Audio Codec

A combination of Example 08 from SparkFun_WM8960_Arduino_Library and Example 'Basic' from Adafruit_ZeroI2S

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

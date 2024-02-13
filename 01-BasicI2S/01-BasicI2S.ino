/* A combination of Example 08 from SparkFun_WM8960_Arduino_Library and Example 'Basic' from Adafruit_ZeroI2S
 * Requires I2C connections and I2S:
 * 
 * Metro M4 - WM8960
 *   3.3       3V3 
 *   5V        VIN
 *   GND       GND
 *   SDA       SDA
 *   SCL       SCL
 *   3         BCLK
 *   9         ALRC and DLRC
 *   8         DDAT
 *   1         ADAT
 *   
 *   Audio:
 *   LIN1/RIN1 - inputs
 *   GND       - input common
 *   HPL/HPR   - outputs (headphones)
 *   OUT3      - output common
 */
  

#include <Arduino.h>

#include <Adafruit_ZeroI2S.h>
#include "wm8960.h"
#include <math.h>

/* max volume for 32 bit data */
#define VOLUME ( (1UL << 31) - 1)

/* create a buffer for both the left and right channel data */
#define BUFSIZE 256
int left[BUFSIZE];
int right[BUFSIZE];

// Use default pins in board variant
Adafruit_ZeroI2S i2s = Adafruit_ZeroI2S();

WM8960interface* codec;

void setup()
{
  while (!Serial) delay(10);

  Serial.println("I2S demo: 01-BasicI2S");

  codec = new WM8960interface();

  for(int i=0; i<BUFSIZE; i++){
      /* create a sine wave on the left channel */
        left[i] = sin( (2*PI / (BUFSIZE) ) * i) * VOLUME;

        /* create a cosine wave on the right channel */
        right[i] = cos( (2*PI / (BUFSIZE) ) * i) * VOLUME;
  }

  /* begin I2S on the default pins. 24 bit depth at
   * 44100 samples per second
   */
  i2s.begin(I2S_32_BIT, 44100);
  i2s.enableTx();
}

void loop()
{
  /* write the output buffers
   * note that i2s.write() will block until both channels are written.
   */
  for(int i=0; i<BUFSIZE; i++){
    i2s.write(left[i], right[i]);
  }
}

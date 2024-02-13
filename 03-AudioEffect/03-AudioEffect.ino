/* A combination of Example 08 from SparkFun_WM8960_Arduino_Library and Example 'dma_passthrough' from Adafruit_ZeroI2S
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
  

#include <Adafruit_ZeroI2S.h>
#include <Adafruit_ZeroDMA.h>
#include "utility/dma.h"
#include <math.h>
#include "wm8960.h"

#ifndef __SAMD51__
#error "this example is for SAMD51 devices only"
#endif

// Simple low-pass filter effect
class Filter{
  public:
  float x,y;
  Filter(){
    x = y = 0;
  }
  int tick(int s){
    y = 0.066*((float)s + x + 13.0*y);
    x = s;
    return (int)y;
  }
};


/* max volume for 32 bit data */
#define VOLUME ( (1UL << 31) - 1)

/* create a buffer for both the left and right channel data */
#define BUFSIZE 128
int txBuf[2][BUFSIZE];
int rxBuf[2][BUFSIZE];

Filter lFilter, rFilter;

/* we need one DMA channel for receive (RX), 
* and another for transmit (TX) 
*/
Adafruit_ZeroDMA txDMA;
Adafruit_ZeroDMA rxDMA;

DmacDescriptor *txDesc;
DmacDescriptor *rxDesc;

ZeroDMAstatus    stat; // DMA status codes returned by some functions

Adafruit_ZeroI2S i2s;
WM8960interface* codec;

unsigned long ptime;

bool rxDone = false;
bool txDone = false;
int bindex = 0;
void processBuffer(){
  if(txDone && rxDone){
    unsigned long tt = micros();
    
    txDone = rxDone = false;

    // Process buffer number 'index'
    for(int i = 0; i < BUFSIZE; i++){
      txBuf[bindex][i] = lFilter.tick(rxBuf[bindex][i]);
      i++;
      txBuf[bindex][i] = rFilter.tick(rxBuf[bindex][i]);
    }
    // Done processing
    
    bindex = 1 - bindex; // Swap buffers

    ptime += (micros()-tt);
  }
}
void txcallback(Adafruit_ZeroDMA *dma) {
  txDone = true;
  processBuffer();
}
void rxcallback(Adafruit_ZeroDMA *dma) {
  rxDone = true;
  processBuffer();
}

void setup()
{
  ptime = 0;
  
  Serial.begin(115200);
  while(!Serial);                 // Wait for Serial monitor before continuing

  Serial.println("I2S throughput via DMA");
  
  codec = new WM8960interface();

  Serial.println("Configuring DMA triggers");
  txDMA.setTrigger(I2S_DMAC_ID_TX_0);
  rxDMA.setTrigger(I2S_DMAC_ID_RX_0);
  txDMA.setAction(DMA_TRIGGER_ACTON_BEAT);
  rxDMA.setAction(DMA_TRIGGER_ACTON_BEAT);

  Serial.print("Allocating DMA channels...");
  stat = txDMA.allocate();
  txDMA.printStatus(stat);

  stat = rxDMA.allocate();
  rxDMA.printStatus(stat);

  Serial.println("Setting up transfer");
  txDesc = txDMA.addDescriptor(
    txBuf[0],                     // move data from here
    (void *)(&I2S->TXDATA.reg),   // to here
    BUFSIZE,                      // this many...
    DMA_BEAT_SIZE_WORD,           // bytes/hword/words
    true,                         // increment source addr?
    false);
  txDesc->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;

  txDesc = txDMA.addDescriptor(
    txBuf[1],                     // move data from here
    (void *)(&I2S->TXDATA.reg),   // to here
    BUFSIZE,                      // this many...
    DMA_BEAT_SIZE_WORD,           // bytes/hword/words
    true,                         // increment source addr?
    false);
  txDesc->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;
  txDMA.loop(true);

  //this will be the initial tx buffer
  //txBuf = ping;

  rxDesc = rxDMA.addDescriptor(
    (void *)(&I2S->RXDATA.reg),   // move data from here
    rxBuf[0],                     // to here
    BUFSIZE,                      // this many...
    DMA_BEAT_SIZE_WORD,           // bytes/hword/words
    false,                        // increment source addr?
    true);
  rxDesc->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;
  
  rxDesc = rxDMA.addDescriptor(
    (void *)(&I2S->RXDATA.reg),   // move data from here
    rxBuf[1],                     // to here
    BUFSIZE,                      // this many...
    DMA_BEAT_SIZE_WORD,           // bytes/hword/words
    false,                        // increment source addr?
    true);
  rxDesc->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_INT;
  rxDMA.loop(true);

  Serial.println("Adding callbacks");
  txDMA.setCallback(txcallback);
  rxDMA.setCallback(rxcallback);

  /* begin I2S on the default pins. 32 bit depth at
   * 44100 samples per second
   */
  i2s.begin(I2S_32_BIT, 44100);

  /* uncomment this if your I2S device uses the MCLK line */
  i2s.enableMCLK();

  /* enable transmit and receive channels */
  i2s.enableTx();
  i2s.enableRx();

  stat = rxDMA.startJob();
  stat = txDMA.startJob();
}

void loop()
{
  Serial.println("do other things here while your DMA runs in the background.");
  Serial.println(ptime);
  ptime = 0;
  delay(2000);
}

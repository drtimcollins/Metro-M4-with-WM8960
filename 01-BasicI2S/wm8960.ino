#include <Wire.h>
#include <SparkFun_WM8960_Arduino_Library.h> 


WM8960interface::WM8960interface(){
  Serial.begin(115200);  
  Wire.begin();
  if (codec.begin() == false) //Begin communication over I2C
  {
    Serial.println("The device did not respond. Please check wiring.");
    while (1); // Freeze
  }
  Serial.println("Device is connected properly.");

  // General setup needed
  codec.enableVREF();
  codec.enableVMID();

  // Setup signal flow to the ADC

  codec.enableLMIC();
  codec.enableRMIC();

  // Connect from INPUT1 to "n" (aka inverting) inputs of PGAs.
  codec.connectLMN1();
  codec.connectRMN1();

  // Disable mutes on PGA inputs (aka INTPUT1)
  codec.disableLINMUTE();
  codec.disableRINMUTE();

  // Set pga volumes
  codec.setLINVOLDB(0.00); // Valid options are -17.25dB to +30dB (0.75dB steps)
  codec.setRINVOLDB(0.00); // Valid options are -17.25dB to +30dB (0.75dB steps)

  // Set input boosts to get inputs 1 to the boost mixers
  codec.setLMICBOOST(WM8960_MIC_BOOST_GAIN_0DB);
  codec.setRMICBOOST(WM8960_MIC_BOOST_GAIN_0DB);

  // Connect from MIC inputs (aka pga output) to boost mixers
  codec.connectLMIC2B();
  codec.connectRMIC2B();

  // Enable boost mixers
  codec.enableAINL();
  codec.enableAINR();

  // Disconnect LB2LO (booster to output mixer (analog bypass)
  // For this example, we are going to pass audio throught the ADC and DAC
  codec.disableLB2LO();
  codec.disableRB2RO();

  // Connect from DAC outputs to output mixer
  codec.enableLD2LO();
  codec.enableRD2RO();

  // Set gainstage between booster mixer and output mixer
  // For this loopback example, we are going to keep these as low as they go
  codec.setLB2LOVOL(WM8960_OUTPUT_MIXER_GAIN_NEG_21DB); 
  codec.setRB2ROVOL(WM8960_OUTPUT_MIXER_GAIN_NEG_21DB);

  // Enable output mixers
  codec.enableLOMIX();
  codec.enableROMIX();

  // CLOCK STUFF, These settings will get you 44.1KHz sample rate, and class-d 
  // freq at 705.6kHz
  codec.enablePLL(); // Needed for class-d amp clock
  codec.setPLLPRESCALE(WM8960_PLLPRESCALE_DIV_2);
  codec.setSMD(WM8960_PLL_MODE_FRACTIONAL);
  codec.setCLKSEL(WM8960_CLKSEL_PLL);
  codec.setSYSCLKDIV(WM8960_SYSCLK_DIV_BY_2);
  codec.setBCLKDIV(4);
  codec.setDCLKDIV(WM8960_DCLKDIV_16);
  codec.setPLLN(7);
  codec.setPLLK(0x86, 0xC2, 0x26); // PLLK=86C226h
  //codec.setADCDIV(0); // Default is 000 (what we need for 44.1KHz)
  //codec.setDACDIV(0); // Default is 000 (what we need for 44.1KHz)
  codec.setWL(WM8960_WL_16BIT);

  codec.enablePeripheralMode();
  //codec.enableMasterMode();
  //codec.setALRCGPIO(); // Note, should not be changed while ADC is enabled.

  // Enable ADCs and DACs
  codec.enableAdcLeft();
  codec.enableAdcRight();
  codec.enableDacLeft();
  codec.enableDacRight();
  codec.disableDacMute();

  //codec.enableLoopBack(); // Loopback sends ADC data directly into DAC
  codec.disableLoopBack();

  // Default is "soft mute" on, so we must disable mute to make channels active
  codec.disableDacMute(); 

  codec.enableHeadphones();
  codec.enableOUT3MIX(); // Provides VMID as buffer for headphone ground

  Serial.println("Volume set to +0dB");
  codec.setHeadphoneVolumeDB(0.00);  
}

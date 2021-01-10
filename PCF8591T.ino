//Example code for PCF8591/PCF8591T/PCF8591-T as tested on Arduino UNO
//META:Arduino-Raspberry-ADC-DAC-PCF8591-I2C-Convertor YL-40-Module-Board 293583458105
//spec sheet: https://www.nxp.com/docs/en/data-sheet/PCF8591.pdf

#include "Wire.h"
#define PCF8591 (0x90 >> 1) // IC bus address
byte adc0, adc1, adc2, adc3, aout;
//be warned about data type bytes, (mis)using basic addition can cause "rollover" e.g. 255+3 -> 2 !


////Chose a debug mode (at the cost of speed!)
//bool dbgMode = 0; //0 == False
bool dbgMode = 1; //1 == True

void setup(){
  Serial.begin(115200); //Go full speed!!! 9600 makes for a slower duty cycle in the event of any output
  Wire.begin();

  ////These change nothing on my UNO wrt to duty cycle/io speeds
  //Wire.setClock(3400000); //High Speed SCL
  //Wire.setClock(100000); //Stnd Speed SCL
  //Wire.setClock(10000); //Low Speed SCL
} //end setup

void loop(){
  ////no delay pls, we luv speeeed
  //delay(1000);

  //Init. readings from the DAC
  Wire.beginTransmission(PCF8591); // ping PCF8591T
  Wire.write(0x04); // control byte - go read mode
  Wire.endTransmission();

  //Get the values...
  Wire.requestFrom(PCF8591, 5); // Read 5 values
  adc0=Wire.read(); //Necessary trick per one dev (John Baxell)

  //...into byte variables
  //driven by pins UNLESS you set the Jumpers on the header
  adc0=Wire.read(); //J5/P5 Photoresitor 255 is DARK, 0 is very bright - level adj. as a function of VR1
  adc1=Wire.read(); //J4/P4 Thermistor 255 is room temp ~20C, 0 is very hot (boiling?!)  - level adj. as a function of VR1
  adc2=Wire.read(); //AIN1 This is analog input, high volts like 100% of vcc will be 255, ground 0.
  adc3=Wire.read(); //J6/P6 This is the VR1 varistor, with fully CCW being 255, CW 0
  //This is the common pin out on the two-sided, two-ended board with the silkscreened designation `YL-40`

  //Let's do something fun here
  aout = adc3; //let the varistor drive the AOUT value ;P

  //Init. write mode to Analog OUT
  Wire.beginTransmission(PCF8591); // ping PCF8591T
  Wire.write(0x40); // control byte - turn on DAC (binary 1000000)
  Wire.write(aout); //  send to DAC for AOUT pin (255 is high -- vcc less approx 3.1Vout at 3.3vcc, and 4.5Vout at 5.20Vcc)
  Wire.endTransmission(); // end tranmission

  ////Arduino UNO SCL speeds yield a 2.25ms duty cycle (not very fast), plus
  ////Enabling serial output comes with a little computer science consequence;
  ////Every bit of output will result delay, making the duty cycle longer, e.g.:
  //Serial.println(); // 36 of these would make your duty cycle 8ms
  //Serial.println( <...96 ascii characters...> ); // would make your duty cycle 8ms
  ////This is a great reason to have multithread/asynch systems, right?!
  /////If you need output for more than debug purposes, consider adding a millis counter

  if (dbgMode){
    Serial.print(adc0);
    Serial.print(" ,");
    Serial.print(adc1); 
    Serial.print(" ,");
    Serial.print(adc2); 
    Serial.print(" ,");
    Serial.print(adc3); 
    Serial.println();
  }   

} //end loop

////Fun note:
//If you measure the output at the above duty cylce(s), a voltmeter will see about 2.34vdc@50/50 or 1.51vdc @33/66
//but an o-scope could confirm the peaks are at nearly the vcc.  Check wikipedia for the topic of `RMS` for more info.
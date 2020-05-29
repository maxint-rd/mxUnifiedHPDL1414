
// Use the virtual port expander of the mxUnifiedIO base library
/*
#include <mxUnifiedIO.h>
mxUnifiedIO unio = mxUnifiedIO();   // create the object to access the expanded pins
*/

/* This is the default hardware pin-mapping of the mxUnifiedIO base class:
#ifdef ESP8266
  _aPins[0]=0;
  _aPins[1]=2;    // ESP12 LED on pin 2 is active low
  _aPins[2]=12;
  _aPins[3]=13;
  _aPins[4]=14;
  _aPins[5]=15;
  _aPins[6]=4;
  _aPins[7]=5;
#else
  _aPins[0]=0;    // sometimes there is a LED on pin 0 (TX)
  _aPins[1]=11;
  _aPins[2]=12;
  _aPins[3]=13;   // most Arduino's have a LED on pin 13
  _aPins[4]=4;
  _aPins[5]=5;
  _aPins[6]=6;
  _aPins[7]=7;
#endif
*/

// Use the PCF8575 I2C 16-bit I/O expander with hardware SDA/SCL (on Arduino these are A4, A5)
#include <mxUnifiedPCF8574.h>
mxUnifiedPCF8575 unio = mxUnifiedPCF8575(0x20);

/*
*
*  HPDL1414 pinout:
*
*  1 1 1                       
*  2 1 0 9 8 7                 1 D5  data input*       7 GND
*  +-+-+-+-+-+                 2 D4  data input       8 D0 data input
*  |         |                 3 !WR write            9 D1 data input
*  | A B C D |                 4 A1  digit select    10 D2 data input
*  |         |                 5 A0  digit select    11 D3 data input
*  /-+-+-+-+-+                 6 Vdd (5V)            12 D6 data input*
*  1 2 3 4 5 6                 
*  
*  *Note: some datasheets have pins D5 and D6 reversed.
*   If your text is messed up, change your wiring!
*  
*/

#include <mxUnifiedHPDL1414.h>
/* 
// When using an 8-bit expander,  expanded pins 1-7 are data and regular pins can be used for address and !write
// Then only two display modules can be used.
#define _A0 8
#define _A1 9
#define _W0 2
#define _W1 3
mxUnifiedHPDL1414 hpdl414 = mxUnifiedHPDL1414(&unio, _A0, _A1, _W0, _W1);   // create the object to access the expanded pins
*/

// when using an 16-bit expander, expanded pins 1-7 are data and pins 8 and up are used for address and !write
byte _nCntModules=2;    // when using a 16-bit expander up to 6 modules can be used (using expanded pins 10-15 for !WR)
mxUnifiedHPDL1414 hpdl414 = mxUnifiedHPDL1414(&unio, _nCntModules);   // create the object to access the expanded pins


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(F("\r\nmxUnifiedIO HPDL1414"));

  unio.begin();     // start using the mxUnifiedIO object
  hpdl414.begin();     // start using the mxUnifiedIO display module
  hpdl414.setOverflow(true);   // restart at position 0 when display is full

  hpdl414.println("Hello");   // println resets cursor to position 0
  delay(1000);

  hpdl414.print("Test");      // Next print continues at lastest position
  hpdl414.print("ing!");
  delay(2000);

  // show a brief ticker-text
  hpdl414.setOverflow(false);   // don't restart at position 0 when display is full
  hpdl414.clear();
  String sTicker="Hello hpdl414, welcome to Arduino!";
  Serial.println(sTicker);
  for(int n=(int)hpdl414.getNumDigits(); n>=(-1 * (int)sTicker.length()); n--)
  {
    hpdl414.clear();
    hpdl414.setCursor(n);     // cursor position can be negative to print before visible area
    hpdl414.print(sTicker);
    delay(200);
  }
  delay(200);

  hpdl414.println("OK...");
  hpdl414.setOverflow(true);   // restart at position 0 when display is full
  delay(2000);
}

void loop()
{
  for(int nChar=' '; nChar<='_'; nChar++)
  {
    Serial.write(nChar);
    Serial.print(F(" "));
    hpdl414.write(nChar);     // write() can be used to print a sing;e character at the current position
    delay(250);
  }
  Serial.println(F("."));
  delay(200);
}

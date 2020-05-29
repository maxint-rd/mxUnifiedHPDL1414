/*********************************************************************
This is the mxUnifiedIO library for the HPDL1414 display module

Parts of this library are based on the following information and examples:
 - https://github.com/marecl/HPDL1414/
	
HPDL1414 pinout

1 1 1                       
2 1 0 9 8 7                 1 D5  data input*      7 GND
+-+-+-+-+-+                 2 D4  data input       8 D0 data input
|         |                 3 !WR write            9 D1 data input
| A B C D |                 4 A1  digit select    10 D2 data input
|         |                 5 A0  digit select    11 D3 data input
/-+-+-+-+-+                 6 Vdd (5V)            12 D6 data input*
1 2 3 4 5 6                 

*Note: some datasheets have pins D5 and D6 reversed!

Library made by Maxint R&D to drive the HPDL1414 display module via an 
I2C I/O expander or the 74HC595 shift register using the mxUnifiedIO API.
For more information ee https://github.com/maxint-rd/mxUnifiedIO
*********************************************************************/
#ifndef _MXUNIFIEDHPDL1414_H
#define _MXUNIFIEDHPDL1414_H

#include <mxUnifiedIO.h>

class mxUnifiedHPDL1414 : public Print
{
 public:
 	mxUnifiedHPDL1414(mxUnifiedIO *pUniOut, int8_t nNumModules);	// use expanded pins 1-7 for data, expanded pins 8,9 A0 and A1, expanded pins 10,11,... for the _WR (one per module)
 	mxUnifiedHPDL1414(mxUnifiedIO *pUniOut, int8_t nPinA0, int8_t nPinA1, int8_t nPinNotWR0, int8_t nPinNotWR1=0);		  	// use only expanded pins 1-7 for data, specify regular pins for A0 and A1, and for the _WR (max two modules)

  void clear();
  void begin();
	void setOverflow(bool fAllowOverflow);
	uint8_t getNumModules(void);
	uint8_t getNumDigits(void);
	void writeChr(uint8_t ch);		// write a character at the current position
	
  // support for the Print class, see https://playground.arduino.cc/Code/Printclass
  void setCursor(int8_t nPos); 	// allows setting negative to support scrolled printing
  using Print::write; // pull in write(str) and write(buf, size) from Print
  virtual size_t write(uint8_t ch);
  /* virtual size_t write(const char *str); */   // MMOLE: not called by print()

 private:
  mxUnifiedIO *_pUniOut;
  int8_t _nPin_A0;    		// regular pin or set to 0 to use expanded pin 7
  int8_t _nPin_A1;    		// regular pin or set to 0 to use expanded pin 8
  int8_t _nPin_NotWR0;		// regular pin or set to 0 to use expanded pin 9
  int8_t _nPin_NotWR1;		// regular pin or set to 0 to use expanded pin 10
	uint8_t _nNumModules;
	uint8_t _nMaxDigits;

  void strobeWritePin(uint8_t nPin, bool fVirtual);
  void strobeWriteModule(uint8_t nModule);
	void setDigitAddress(uint8_t a);
	
	static const int8_t _nVPin_D0=1;			// assume datapins on virtual pins 1-7 (pin 0 available for blinking led or something else
	static const int8_t _nVPin_A0=8;
	static const int8_t _nVPin_A1=9;
	static const int8_t _nVPin_NotWR0=10;


  // support for the Print class
	int8_t _nPrintPos;			// current print position (can be negative for easy scrolling)

  char translate(char);
  bool _fAllowOverflow;
};

#endif
/*

This is the mxUnifiedIO library for the HPDL1414 display module.
See mxUnifiedHPDL1414.h for more information

Made by Maxint R&D. See https://github.com/maxint-rd/

*/

#include "mxUnifiedHPDL1414.h"
#include <string.h>

mxUnifiedHPDL1414::mxUnifiedHPDL1414(mxUnifiedIO *pUniOut, int8_t nNumModules)
{ // use expanded pins 0-6 for data, expanded pins 7,8 A0 and A1, expanded pins 9,10,... for the _WR (one per module)
	_pUniOut=pUniOut;
	_nNumModules=nNumModules;
	_nMaxDigits=_nNumModules*4;
}


mxUnifiedHPDL1414::mxUnifiedHPDL1414(mxUnifiedIO *pUniOut, int8_t nPinA0, int8_t nPinA1, int8_t nPinNotWR0, int8_t nPinNotWR1)
{		  	// use only expanded pins 0-7 for data, specify regular pins for A0 and A1, and for the _WR (max two modules)
	_pUniOut=pUniOut;
	_nNumModules=(nPinNotWR1==0)?1:2;
	_nMaxDigits=_nNumModules*4;

	_nPin_A0=nPinA0;    		// regular pin or set to 0 to use expanded pin 7
	_nPin_A1=nPinA1;    		// regular pin or set to 0 to use expanded pin 8
	_nPin_NotWR0=nPinNotWR0;		// regular pin or set to 0 to use expanded pin 9
	_nPin_NotWR1=nPinNotWR1;		// regular pin or set to 0 to use expanded pin 10
	
	_nPrintPos=0;
}

void mxUnifiedHPDL1414::strobeWritePin(uint8_t nPin, bool fVirtual)
{
	if(fVirtual)
	{
		//Serial.print(F("~"));
		//Serial.print(nPin);
		_pUniOut->digitalWrite(nPin, LOW);
		delayMicroseconds(1); // Needs ~150ns so it's okay
		_pUniOut->digitalWrite(nPin, HIGH);
		delayMicroseconds(1);
	}
	else
	{
		//Serial.print(F("^"));
		//Serial.print(nPin);
		digitalWrite(nPin, LOW);
		delayMicroseconds(1); // Needs ~150ns so it's okay
		digitalWrite(nPin, HIGH);
		delayMicroseconds(1);
	}
}

void mxUnifiedHPDL1414::strobeWriteModule(uint8_t nModule)
{	// Strobe the write-pin of the specified module to show the character currently on the data pins at the currently selected digit position.
	if(nModule>=_nNumModules) return;
	
	if(_nPin_NotWR0!=0 && nModule==0)
		strobeWritePin(_nPin_NotWR0, false);
	else if(_nPin_NotWR1!=0 && nModule==1)
		strobeWritePin(_nPin_NotWR1, false);
	else 
		strobeWritePin(_nVPin_NotWR0+nModule, true);
}

void mxUnifiedHPDL1414::setDigitAddress(uint8_t a)
{	// Set the address lines for a particular digit of a segment.
	// Note: Address 0 is normally the rightmost digit. Using ! before the address-bit reverses the digit position.
	if(_nPin_A0!=0)
		digitalWrite(_nPin_A0, !(a & 0x01));
	else
		_pUniOut->digitalWrite(_nVPin_A0, !(a & 0x01));

	if(_nPin_A1!=0)
		digitalWrite(_nPin_A1, !(a & 0x02));
	else
		_pUniOut->digitalWrite(_nVPin_A1, !(a & 0x02));
}

char mxUnifiedHPDL1414::translate(char i) {
	if (i > 31 && i < 96) return i;
	else if (i > 96 && i < 123) return i - 32;
	return 32;
}


void mxUnifiedHPDL1414::writeChr(uint8_t ch)
{	// write a character at the current position
#if 0
	Serial.print(F("Pos "));
	Serial.print(_nPrintPos);
	Serial.print(F(" chr "));
	Serial.print(ch);
	Serial.print("=");
	Serial.write(ch);
	//Serial.println("");
#endif

	//Serial.print(F("==>"));
	byte nMod = (_nPrintPos - (_nPrintPos % 4)) / 4;
	setDigitAddress(_nPrintPos);
	//Serial.print(F("mod:"));
	//Serial.print(nMod);
	ch = translate(ch);
	for (byte n = 0; n < 7; n++)
		_pUniOut->digitalWrite(_nVPin_D0+n, ((ch >> n) & 0x01));			// assume datapins on virtual pins 1-7 (pin 0 available for blinking led or something else
	
	strobeWriteModule(nMod);
	//Serial.println(F("."));
}

/*
 * Support for the Print class
 *
 * See https://playground.arduino.cc/Code/Printclass
 *
*/
size_t mxUnifiedHPDL1414::write(uint8_t ch)
{
	// reset cursor position after new line, return or end of string
	if(ch=='\0' || ch=='\n' || ch=='\r')	//  || _nPrintPos>=_nMaxDigits)
	{
		while(_nPrintPos>0 && _nPrintPos<_nMaxDigits)
		{	// clear the remainder of the line
			this->writeChr(' ');
			_nPrintPos++;
		}
		_nPrintPos=0;
		return(0);	// returning zero will stop printing rest of the string
	}

	// Implement wrapping. Allow negative position to support easy scrolling
	if(_nPrintPos<0)
	{
		_nPrintPos++;
		return 1;	// returning zero will stop printing rest of the string
	}
	if (_nPrintPos >= this->_nMaxDigits)
	{
		if (_fAllowOverflow) _nPrintPos = 0;
		else return 0;
	}

	// actually write a character to the display at the current position
	this->writeChr(ch);
	_nPrintPos++;
	return(1);
}

void mxUnifiedHPDL1414::clear(void)
{	// Clear the digits of all modules 
	// Set character data pins to zero
	for (byte n = 0; n < 7; n++)
		_pUniOut->digitalWrite(_nVPin_D0+n, LOW);
	
	// Set the digit addresses and strobe the write pin of each module.
	// Nested loop is slightly slower due to to extra strobes of module write,
	// Faster way is to select all modules for writing and then sweep the digits
	for (byte nMod = 0; nMod < this->_nNumModules; nMod++)
		for (byte a = 0; a < 4; a++)
		{
			this->setDigitAddress(a);
			strobeWriteModule(nMod);
		}
	
	_nPrintPos = 0;
};

void mxUnifiedHPDL1414::setCursor(int8_t nPos)
{	// Set the print position. Allow negative numbers to support scrolling
  _nPrintPos = nPos;
}


void mxUnifiedHPDL1414::begin()
{
	bool fOnlyVirtualPins=true;	

	// initialize address and module-write pins
	if(_nPin_A0!=0)
	{
		fOnlyVirtualPins=false;
	  pinMode(_nPin_A0, OUTPUT);
	  digitalWrite(_nPin_A0, LOW);
	}
	if(_nPin_A1!=0)
	{
		pinMode(_nPin_A1, OUTPUT);
		digitalWrite(_nPin_A1, LOW);
	}
	
	if(_nPin_NotWR0!=0)
	{
		pinMode(_nPin_NotWR0, OUTPUT);
		digitalWrite(_nPin_NotWR0, HIGH);
	}
	if(_nPin_NotWR1!=0)
	{
		pinMode(_nPin_NotWR1, OUTPUT);
		digitalWrite(_nPin_NotWR1, HIGH);
	}

	if(fOnlyVirtualPins)
	{
		_pUniOut->digitalWrite(_nVPin_A0, LOW);		
		_pUniOut->digitalWrite(_nVPin_A1, LOW);		
		for(int n=0; n<_nNumModules; n++)
			_pUniOut->digitalWrite(_nVPin_NotWR0+n, HIGH);		
	}

	// initialize data pins
	for(int n=0; n<7; n++)
		_pUniOut->digitalWrite(_nVPin_D0+n, LOW);

	_nPrintPos = 0;
	_fAllowOverflow = false;
	
	// clear display
	clear();
}


void mxUnifiedHPDL1414::setOverflow(bool fAllowOverflow)
{	// set if Overflowing characters should be wrapped and printed
	_fAllowOverflow = fAllowOverflow;
};

uint8_t mxUnifiedHPDL1414::getNumModules(void)
{	// Return how many modules are configured
	return this->_nNumModules;
}

uint8_t mxUnifiedHPDL1414::getNumDigits(void)
{	// Return how many digits are available
	return this->_nMaxDigits;
}
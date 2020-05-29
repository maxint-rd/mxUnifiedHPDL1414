# mxUnifiedHPDL1414
mxUnifiedIO device library for Arduino. Device specific driver for the HPDL1414 bubble LED display module. Use SPI shift registers or I2C I/O expanders via the [mxUnifiedIO](https://github.com/maxint-rd/mxUnifiedIO) API. Then simply use the familiar Arduino print() function to print text or numbers.

# Introduction
This is an Arduino library for the HPDL1414 bubble LED display module. The HPDL-1414 four-character LED display can show a selection of alpha-numeric characters in refined retro style. As most of electronics for driving the LEDs is integrated, it doesn't require external components and can be hooked up directly to the Arduino or via an IO-expander if you want to minimize pin usage.
Multiple display modules can be combined to show longer texts. The library implements the familiar Arduino print() function to simplify printing text on the display.

This library requires the the mxUnifiedIO base-library and a suitable mxUnifiedIO expander driver. Instead of directly using MCU pins, the [mxUnifiedIO](https://github.com/maxint-rd/mxUnifiedIO) libraries allow the display to be driven via the expanded pins of an I2C I/O expander or a shift-register. 

# Pinouts & connections
The HPDL1414 display requires 5V power via VCC/GND, character selection via seven data pins, position selection via two address pins (A0,A1) and one write pin per module (/WR). See the [datasheet](documentation/datasheetHPDL1414.pdf) for more information.

This fine piece of ASCII-art shows the pinout of the HPDL1414. Of course you can also review the [datasheet](documentation/datasheetHPDL1414.pdf) (PDF).
```
HPDL1414 pinout

1 1 1                       
2 1 0 9 8 7                 1 D5  data input*      7 GND
+-+-+-+-+-+                 2 D4  data input       8 D0 data input
|         |                 3 !WR write            9 D1 data input
| A B C D |                 4 A1  digit select    10 D2 data input
|         |                 5 A0  digit select    11 D3 data input
/-+-+-+-+-+                 6 Vdd (5V)            12 D6 data input*
1 2 3 4 5 6                 
```
*Note: some datasheets have pins D5 and D6 reversed. If your display isn't showing the characters you expected, please check your connections.

### Wiring your I/O expander
The library has expanded pins 1-7 reserved for D0 to D6. Expanded pin 0 can be used for your own purpose such as a buzzer or blinking LED. Expanded pins 8 and 9 are used for A0 and A1. Expanded pins 10 and up are used as the !WR line of each module. Using a 16-bit I/O expander, up to 6 modules can be connected.
When only 8 expanded pins are ;available A0, A1, !WR0 and !WR1 can be connected directly to available MCU pins. These then need to be specified in the constructor. Alternatively the virtual expanded pins of the mxUnifiedIO base class can also be used for the data pins.
See the pinout diagram above to find the various pins. (Note the notch on pin 1).

# References
- Part of this library is based on the [HPDL1414 library by Marek](https://github.com/marecl/HPDL1414/)
- The [YouTube video by David Watts](https://youtu.be/PlggHChsFKw) inspired me to buy some modules and write this library

# Features & limitations
- The current version of this library was tested using the Arduino 1.8.2 IDE, a Pro Mini with ATmega328@16Mhz and two HP DL1414 display modules.
- The library was tested to work with the PCF8575 16-bit I/O expander as well as virtual expanded pins of the base class.
- It should also work with a 74HC595 shift-register on pin limited MCU's such as the WeMos D1 mini (ESP8266 ESP-12F) and even more limited devices such as an ATtiny85 or an ESP-01. Note that this may require external 5V power and a voltage level converter.
- The library should also work using two cascaded shiftregisters or the PCF8574 I2C I/O expander, as well as using other Arduino IDE supported MCUs such as the ESP32 or the ATmega328/ATmega168 as found on the Arduino Uno and Nano.

# Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.

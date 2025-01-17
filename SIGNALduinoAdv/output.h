// output.h
#ifndef _OUTPUT_h
#define _OUTPUT_h


/*#ifdef ARDUINO_AVR_ICT_BOARDS_ICT_BOARDS_AVR_RADINOCC1101
#define portOfPin(P) \
((((P) >= 0 && (P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &PORTD : (((P) == 5 || (P) == 13) ? &PORTC : (((P) >= 18 && (P) <= 23)) ? &PORTF : (((P) == 7) ? &PORTE : &PORTB)))
#define ddrOfPin(P) \
((((P) >= 0 && (P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &DDRD : (((P) == 5 || (P) == 13) ? &DDRC : (((P) >= 18 && (P) <= 23)) ? &DDRF : (((P) == 7) ? &DDRE : &DDRB)))
#define pinOfPin(P) \
((((P) >= 0 && (P) <= 4) || (P) == 6 || (P) == 12 || (P) == 24 || (P) == 25 || (P) == 29) ? &PIND : (((P) == 5 || (P) == 13) ? &PINC : (((P) >= 18 && (P) <= 23)) ? &PINF : (((P) == 7) ? &PINE : &PINB)))
#define pinIndex(P) \
(((P) >= 8 && (P) <= 11) ? (P) - 4 : (((P) >= 18 && (P) <= 21) ? 25 - (P) : (((P) == 0) ? 2 : (((P) == 1) ? 3 : (((P) == 2) ? 1 : (((P) == 3) ? 0 : (((P) == 4) ? 4 : (((P) == 6) ? 7 : (((P) == 13) ? 7 : (((P) == 14) ? 3 : (((P) == 15) ? 1 : (((P) == 16) ? 2 : (((P) == 17) ? 0 : (((P) == 22) ? 1 : (((P) == 23) ? 0 : (((P) == 24) ? 4 : (((P) == 25) ? 7 : (((P) == 26) ? 4 : (((P) == 27) ? 5 : 6 )))))))))))))))))))
#endif
*/
#if !defined(MAPLE_Mini) && !defined(ESP32)
#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define ddrOfPin(P)\
  (((P)>=0&&(P)<8)?&DDRD:(((P)>7&&(P)<14)?&DDRB:&DDRC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P)((uint8_t)(P>13?P-14:P&7))
#endif

#if !defined(MAPLE_Mini) && !defined(ESP32)
#define pinMask(P)((uint8_t)(1<<pinIndex(P)))
#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P)((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P)((uint8_t)isHigh(P))
#elif defined(MAPLE_Mini)
	#define pinAsInput(pin) pinMode(pin, INPUT)
	#define pinAsOutput(pin) pinMode(pin, OUTPUT)
	#define pinAsInputPullUp(pin) pinMode(pin, INPUT_PULLUP)
	#define digitalLow(pin) digitalWrite(pin, LOW)
	#define digitalHigh(pin) digitalWrite(pin, HIGH)
	#define isHigh(pin) (digitalRead(pin) == HIGH)
	#define isLow(pin) (digitalRead(pin) == LOW)
#else
// ESP32
	#define pinAsInput(pin) pinMode(pin, INPUT)
	#define pinAsOutput(pin) pinMode(pin, OUTPUT)
	#define pinAsInputPullUp(pin) pinMode(pin, INPUT_PULLUP)

	#ifndef digitalLow
		#define digitalLow(pin) digitalWrite(pin, LOW)
	#endif
	#ifndef digitalHigh
		#define digitalHigh(pin) digitalWrite(pin, HIGH)
	#endif
	#ifndef isHigh
		#define isHigh(pin) (digitalRead(pin) == HIGH)
	#endif
	#ifndef isLow
		#define isLow(pin) (digitalRead(pin) == LOW)
	#endif
#endif
//#define DEBUG

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
//	#include "WProgram.h"
#endif

#ifdef SERIAL_USART2
  extern HardwareSerial Serial;
#endif

#ifdef ESP32
  #include <WiFiClient.h>
  extern WiFiClient client;
  #define MSG_PRINTER client
#elif LAN_WIZ
  #include <SPI.h>
  #include <Ethernet.h>

  extern EthernetClient client;

  #define MSG_PRINTER client
#else
  #define MSG_PRINTER Serial
#endif

#if defined(LAN_WIZ) || defined(ESP32)
//#ifdef ETHERNET_DEBUG
#define DBG_PRINTER client
#else
#define DBG_PRINTER Serial
#endif

#define MSG_PRINT(...) { MSG_PRINTER.print(__VA_ARGS__); }
#define MSG_PRINTLN(...) { MSG_PRINTER.println(__VA_ARGS__); }
#define MSG_WRITE(...) { MSG_PRINTER.write(__VA_ARGS__); }

#ifdef DEBUG
	#define DBG_PRINT(...) {  DBG_PRINTER.print(__VA_ARGS__); }
	#define DBG_PRINTLN(...) { DBG_PRINTER.println(__VA_ARGS__); }
#else
	#define DBG_PRINT(...) 
	#define DBG_PRINTLN(...) 
#endif

#endif

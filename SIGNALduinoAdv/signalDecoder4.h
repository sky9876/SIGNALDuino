/*
*   Pattern Decoder Library V4
*   Library to decode radio signals based on patternd detection
*   2014-2015  N.Butzek, S.Butzek
*   2015-2017  S.Butzek
*   2020 Ralf9

*   This library contains different classes to perform detecting of digital signals
*   typical for home automation. The focus for the moment is on different sensors
*   like weather sensors (temperature, humidity Logilink, TCM, Oregon Scientific, ...),
*   remote controlled power switches (Intertechno, TCM, ARCtech, ...) which use
*   encoder chips like PT2262 and EV1527-type and manchester encoder to send
*   information in the 433MHz or 868 Mhz Band.
*
*   The classes in this library follow the approach to detect a recurring pattern in the
*   recived signal. For Manchester there is a class which decodes the signal.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef _SIGNALDECODER_h
#define _SIGNALDECODER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	//#include "WProgram.h"
#endif

#include "compile_config.h"

#define DEBUG 1

#ifdef DEBUG_BackupReg
	#include <backup.h>
#endif

#include "output.h"
#include "bitstore4.h"

#define maxNumPattern 16
#define defMaxMsgSize 1500 //2000
#define defMsMaxMsgSize 254
#define minMessageLen 40
#define maxMCmstart 254
#define syncMinFact 6
#define syncMaxFact 45 // 39
#define syncMaxMicros 17000
#define maxPulse 32001  // Magic Pulse Length


#define SERIAL_DELIMITER  char(';')
#define MSG_START char(0x2)			// this is a non printable Char
#define MSG_END char(0x3)			// this is a non printable Char

//#define DEBUGDETECT 1
//#define DEBUGDETECT 255  // Very verbose output
//#define MCDEBUGDECODE 2    // debug mc-decoder
//#define MCDEBUGDETECT 5    // debug isManchester
//#define DEBUGGLEICH 1
//#define DEBUGDECODE 1    // debug processMessage
//#define DEBUGDoDETECT 3  // debug compress_pattern
//#define DEBUGMUREPEAT 1  // debug isMuMessageRepeat

enum status { searching, clockfound, syncfound, detecting };


class rssiCallbackInterface // fuer getRSSI
{
public:
    // The prefix "cbi" is to prevent naming clashes.
    virtual uint8_t cbiRssiCallbackFunction(void) = 0;
};

class SignalDetectorClass
{
	friend class ManchesterpatternDecoder;

public:
	SignalDetectorClass() : first(buffer), last(NULL), message(4) { 
		buffer[0] = 0;
		reset();
		mcMinBitLen = 17; 	
		MsMoveCount = 0;
		MuMoveCount = 0;
	};

	void reset();
	bool decode(const int16_t* pulse);
	const status getState();
	
	// Clients can connect their callback with this
	void rssiConnectCallback(rssiCallbackInterface *cb)
	{
		m_cb = cb;
	}

	//private:
	int8_t clock;                           // index to clock in pattern
	bool MUenabled;
	bool MCenabled;
	bool MSenabled;
	bool MSeqEnabled;
	bool MredEnabled;                       // 1 = compress printMsgRaw
	//bool MuNoOverflow;
	bool MdebEnabled;                       // 1 = print message debug info  enabled
	//bool MfiltEnabled;        // fuer Nachrichten Filter reserviert
	uint8_t MsMoveCountmax;
	uint8_t MsMoveCount;
	uint8_t MuMoveCount;
	uint16_t maxMuPrint;
	uint16_t maxMsgSize;
	uint16_t MuSplitThresh;
	uint8_t cMaxNumPattern;
	int16_t cMaxPulse;
	bool printMsgSuccess;
	bool hasCC1101;
	
	uint16_t histo[maxNumPattern];
	//uint8_t message[defMaxMsgSize];
	
	char buf[350] = {};
#ifdef DEBUGGLEICH
	uint8_t bMoveFlag;   // nur zu debugzwecke
#endif
	int16_t firstLast;
	int16_t lastPulse;
	uint16_t messageLen;					  // Todo, kann durch message.valcount ersetzt werden
	uint8_t mstart;						  // Holds starting point for message
	uint16_t mend;						  // Holds end point for message if detected
	bool success;                         // True if a valid coding was found
	bool m_truncated;					// Identify if message has been truncated
	bool m_overflow;
	void bufferMove(const uint16_t start);

	uint16_t tol;                           // calculated tolerance for signal

	status state;                           // holds the status of the detector
	int16_t buffer[2];                          // Internal buffer to store two pules length
	int16_t* first;                             // Pointer to first buffer entry
	int16_t* last;                              // Pointer to last buffer entry 
	BitStore<defMaxMsgSize/2> message;       // A store using 4 bit for every value stored.
	float tolFact;                          //
	int16_t pattern[maxNumPattern];				// 1d array to store the pattern
	int16_t ms0pattern[10];
	uint8_t patternLen;                     // counter for length of pattern
	uint8_t msPatternLen;
	uint8_t pattern_pos;
	int8_t sync;							// index to sync in pattern if it exists
	bool mcDetected;						// MC Signal alread detected flag
	bool mcValid;
	bool mcRepeat;				// ist true wenn evtl noch eine Wiederholung folgen kann
	uint8_t mcMinBitLen;					// min bit Length
	uint8_t rssiValue;						// Holds the RSSI value retrieved via a rssi callback
	uint8_t idxDat;
	uint8_t msEqCnt;
	bool MsEqSkip;
	
	void addData(const uint8_t value);
	void addPattern();
	inline void updPattern(const uint8_t ppos);

	void doDetect();
	void processMessage(const uint8_t p_valid);
	bool compress_pattern();
	void calcHisto(const uint8_t startpos = 0, uint16_t endpos = 0);
	bool getClock(); // Searches a clock in a given signal
	bool getSync();	 // Searches clock and sync in given Signal
	//int8_t printMsgRaw(uint8_t m_start, const uint_t m_end, const String *preamble = NULL, const String *postamble = NULL);
	void printMsgStr(const String *first, const String *second, const String *third);
	const bool inTol(const int16_t val, const int16_t set, const int16_t tolerance); // checks if a value is in tolerance range

	void printOut();

	int8_t findpatt(const int16_t val);              // Finds a pattern in our pattern store. returns -1 if te pattern is not found

	bool checkMBuffer();
	bool isMuMessageRepeat();
#ifdef DEBUG_BackupReg
	void setBackupRegSD(uint32_t n);
#endif
	
private:
    // The callback provided by the client via connectCallback().
    rssiCallbackInterface *m_cb;
};

class ManchesterpatternDecoder
{
public:
	ManchesterpatternDecoder(SignalDetectorClass *ref_dec) : ManchesterBits(1), longlow(-1), longhigh(-1), shorthigh(-1), shortlow(-1) { pdec = ref_dec; 	reset(); };
	~ManchesterpatternDecoder();
	const bool doDecode();
	void setMinBitLen(const uint8_t len);
	void getMessageHexStr(String *message);
	void printMessageHexStr();
	void printBufMessageHexStr();

	const bool isManchester();
	void reset();
#ifndef UNITTEST
//private:
#endif
	BitStore<50> ManchesterBits;       // A store using 1 bit for every value stored. It's used for storing the Manchester bit data in a efficent way
	SignalDetectorClass *pdec;
	int8_t longlow;
	int8_t longhigh;
	int8_t shorthigh;
	int8_t shortlow;
	int16_t clock; // Manchester calculated clock		
	int8_t minbitlen;
	uint16_t mc_sync_pos;
	
	bool mc_start_found = false;

	const bool isLong(const uint8_t pulse_idx);
	const bool isShort(const uint8_t pulse_idx);
	unsigned char getMCByte(const uint8_t idx); // Returns one Manchester byte in correct order. This is a helper function to retrieve information out of the buffer
};


#endif

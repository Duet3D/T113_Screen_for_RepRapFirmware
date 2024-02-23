/*
 * SerialIO.cpp
 *
 * Created: 04/02/2023
 *  Author: Loïc G.
 *
 *  This file is a modified version of the SerialIO.cpp source file from PanelDue
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/Hardware/SerialIo.cpp
 *
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO

#include "SerialIo.h"
#include "uart/UartContext.h"
#include <string>

#include "Debug.h"

#define jserror(fmt, args...) error("jsError id=%s @ %d: " fmt, fieldId.c_str(), nextOut, ##args)

const size_t MaxArrayNesting = 4;

namespace SerialIo {

	static struct SerialIoCbs *cbs = nullptr;

	static int serialIoErrors = 0;

	// Translation tables for combining characters.
	// The first character in each pair is the character that the combining mark is applied to.
	// The second character is what is translates to if the value is >=0x80, else the value it translates to minus 0x0100.
	const char* _ecv_array const trGrave =		"A\xC0"			"E\xC8" 				"I\xCC"         "O\xD2" "U\xD9"
											"a\xE0"			"e\xE8" 				"i\xEC"			"o\xF2" "u\xF9"			;
	const char* _ecv_array const trAcute =		"A\xC1" "C\x06" "E\xC9" 				"I\xCD" "L\x39" "N\x43" "O\xD3" "R\x54" "S\x5A" "U\xDA" "Y\xDD"	"Z\x79"
											"a\xE1" "c\x07" "e\xE9" 				"i\xED" "l\x39"	"n\x44"	"o\xF3" "r\x55" "s\x5B" "u\xFA" "y\xFD" "z\x7a"	;
	const char* _ecv_array const trCircumflex =	"A\xC2" "C\x08" "E\xCA" "G\x1C" "H\x24" "I\xCE" "J\x34"	"O\xD4" "S\x5C" "U\xDB" "W\x74" "Y\x76"
											"a\xE2" "c\x09" "e\xEA" "g\x1D" "h\x25" "i\xEE" "j\x35"	"o\xF4" "s\x5D" "u\xFB"	"w\x75"	"y\x77"	;
	const char* _ecv_array const trTilde =		"A\xC3"                 				"I\x28" "N\xD1" "O\xD5" "U\x68"
											"a\xE3"                 				"i\x29"	"n\xF1"	"o\xF5"	"u\x69"			;
	const char* _ecv_array const trBreve =		"A\x02"			"E\x14" "G\x1E" 		"I\x2C"			"O\x4E" "U\x6c"
											"a\x03"			"e\x15" "g\x1F" 		"i\x2D"			"o\x4F" "u\x6d"			;
	const char* _ecv_array const trUmlaut =		"A\xC4"			"E\xCB" 				"I\xCF"         "O\xD6" "U\xDC" "Y\x78"
											"a\xE4"			"e\xEB" 				"i\xEF"			"o\xF6" "u\xFC" "y\xFF"	;
	const char* _ecv_array const trCircle =		"A\xC5"															"U\x6E"
											"a\xE5"                                 						"u\x6F"			;
	const char* _ecv_array const trCaron =		"C\x0C"	"D\x0C" "E\x1A" "N\x47" "R\x58" "S\x60" "T\x64" "Z\x7D"
											"c\x0D" "d\x0F" "e\x1B" "n\x48" "r\x59" "s\x61" "t\x65" "z\x7E"					;
	const char* _ecv_array const trCedilla =		"C\xC7"
											"c\xE7"																			;

	void Init(uint32_t baudRate, struct SerialIoCbs *callbacks){
		cbs = callbacks;
		// TODO: Set baudrate
	}

	bool Send(const char* data, size_t len)
	{
		return UARTCONTEXT->send((unsigned char*)data, len);
	}

	size_t Sendf(const char *fmt, ...) {
		va_list vargs;
		va_start(vargs, fmt);

		std::string buf;
		size_t ret = vsnprintf(0, 0, fmt, vargs);
		if (ret >= buf.capacity())
			buf.reserve(ret + sizeof(char));
		buf.resize(ret);
		vsnprintf((char*)buf.data(), buf.capacity(), fmt, vargs);
		info("Sending %s", buf.c_str());
		UARTCONTEXT->send((unsigned char*)buf.c_str(), ret);

		va_end(vargs);

		return ret;
	}

	// Receive data processing
	//const size_t rxBufsize = 8192;
	//static char rxBuffer[rxBufsize];
	//static size_t nextIn = 0, nextOut = 0;
	static size_t nextOut = 0;
	static bool inError = false;

	// Enumeration to represent the json parsing state.
	// We don't allow nested objects or nested arrays, so we don't need a state stack.
	// An additional variable elementCount is 0 if we are not in an array, else the number of elements we have found (including the current one)
	enum JsonState {
		jsBegin,			// initial state, expecting '{'
		jsExpectId,			// just had '{' so expecting a quoted ID
		jsId,				// expecting an identifier, or in the middle of one
		jsHadId,			// had a quoted identifier, expecting ':'
		jsVal,				// had ':', expecting value
		jsStringVal,		// had '"' and expecting or in a string value
		jsStringEscape,		// just had backslash in a string
		jsIntVal,			// receiving an integer value
		jsNegIntVal,		// had '-' so expecting a integer value
		jsFracVal,			// receiving a fractional value
		jsEndVal,			// had the end of a string or _ecv_array value, expecting comma or ] or }
		jsCharsVal,			// receiving an alphanumeric value such as true, false, null
		jsError				// something went wrong
	};

	JsonState state = jsBegin;
	JsonState lastState = jsBegin;

	// fieldId is the name of the field being received. A '^' character indicates the position of an _ecv_array index, and a ':' character indicates a field separator.
	String<200> fieldId;
	String<2048> fieldVal;
	size_t arrayIndices[MaxArrayNesting];
	size_t arrayDepth = 0;

	static void RemoveLastId() {
		verbose("%s, len: %d", fieldId.c_str(), fieldId.strlen());
		size_t index = fieldId.strlen();
		while (index != 0 && fieldId[index - 1] != '^' && fieldId[index - 1] != ':') {
			--index;
		}
		fieldId.Truncate(index);

		verbose("RemoveLastId: %s, len: %d", fieldId.c_str(), fieldId.strlen());
	}

	static void RemoveLastIdChar() {
		verbose();

		if (fieldId.strlen() != 0) {
			fieldId.Truncate(fieldId.strlen() - 1);
		}
	}

	static bool InArray() {
		verbose();

		return fieldId.strlen() > 0 && fieldId[fieldId.strlen() - 1] == '^';
	}

	static void ProcessField() {
		dbg("id \"%s\" (%s)", fieldId.c_str(), fieldVal.c_str());
		if (state == jsCharsVal) {
			if (fieldVal.Equals("null")) {
				fieldVal.Clear();				// so that we can distinguish null from an empty string
			}
		}
		if (cbs && cbs->ProcessReceivedValue) {
			verbose("%s: %s", fieldId.c_str(), fieldVal.c_str());
			cbs->ProcessReceivedValue(fieldId.GetRef(), fieldVal.c_str(), arrayIndices);
		}
		fieldVal.Clear();
	}

	static void EndArray() {
		verbose();

		if (cbs && cbs->ProcessArrayEnd) {
			cbs->ProcessArrayEnd(fieldId.c_str(), arrayIndices);
		}
		if (arrayDepth != 0) {			// should always be true
			verbose("id %s (%s), arrayIndices [%d|%d|%d|%d], arrayDepth %d",
					fieldId.c_str(),
					fieldVal.c_str(),
					arrayIndices[0],
					arrayIndices[1],
					arrayIndices[2],
					arrayIndices[3],
					arrayDepth);
			arrayIndices[arrayDepth - 1] = 0;
			--arrayDepth;
			RemoveLastIdChar();
		}
	}

	// Look for combining characters in the string value and convert them if possible
	static void ConvertUnicode() {
		unsigned int numContinuationBytesLeft = 0;
		uint32_t charVal;
		for (size_t i = 0; i < fieldVal.strlen(); ) {
			const unsigned char c = fieldVal[i++];
			if (numContinuationBytesLeft == 0) {
				if (c >= 0x80) {
					if ((c & 0xE0) == 0xC0) {
						charVal = (uint32_t)(c & 0x1F);
						numContinuationBytesLeft = 1;
					} else if ((c & 0xF0) == 0xE0) {
						charVal = (uint32_t)(c & 0x0F);
						numContinuationBytesLeft = 2;
					} else if ((c & 0xF8) == 0xF0) {
						charVal = (uint32_t)(c & 0x07);
						numContinuationBytesLeft = 3;
					} else if ((c & 0xFC) == 0xF8) {
						charVal = (uint32_t)(c & 0x03);
						numContinuationBytesLeft = 4;
					} else if ((c & 0xFE) == 0xFC) {
						charVal = (uint32_t)(c & 0x01);
						numContinuationBytesLeft = 5;
					}
				}
			} else if ((c & 0xC0) == 0x80) {
				charVal = (charVal << 6) | (c & 0x3F);
				--numContinuationBytesLeft;
				if (numContinuationBytesLeft == 0) {
					const char* _ecv_array trtab;
					switch(charVal) {
					case 0x0300:	// grave accent
						trtab = trGrave;
						break;
					case 0x0301:	// acute accent
						trtab = trAcute;
						break;
					case 0x0302:	// circumflex
						trtab = trCircumflex;
						break;
					case 0x0303:	// tilde
						trtab = trTilde;
						break;
					case 0x0306:	// breve
						trtab = trBreve;
						break;
					case 0x0308:	// umlaut
						trtab = trUmlaut;
						break;
					case 0x030A:	// small circle
						trtab = trCircle;
						break;
					case 0x030C:	// caron
						trtab = trCaron;
						break;
					case 0x0327:	// cedilla
						trtab = trCedilla;
						break;
					default:
						trtab = nullptr;
						break;
					}

					// If it is a diacritical mark that we handle, try to combine it with the previous character.
					// The diacritical marks are in the range 03xx so they are encoded as 2 UTF8 bytes.
					if (trtab != nullptr && i > 2) {
						const char c2 = fieldVal[i - 3];
						while (*trtab != 0 && *trtab != c2) {
							trtab += 2;
						}
						if (*trtab != 0) {
							// Get the translated character and encode it as 2 UTF8 bytes
							uint16_t c3 = (uint16_t)(uint8_t)trtab[1];
							if (c3 < 0x80) {
								c3 |= 0x0100;
							}
							fieldVal[i - 3] = (c3 >> 6) | 0xC0;
							fieldVal[i - 2] = (c3 & 0x3F) | 0x80;
							fieldVal.Erase(i - 1);
							--i;
						}
					}
				}
			} else {
				// Bad UTF8 state
				numContinuationBytesLeft = 0;
			}
		}
	}

	// Check whether the incoming character signals the end of the value. If it does, process it and return true.
	static bool CheckValueCompleted(char c, bool doProcess) {
		verbose();

		switch(c)
		{
		case ',':
			if (doProcess) {
				ProcessField();
			}
			if (InArray()) {
				++arrayIndices[arrayDepth - 1];
				fieldVal.Clear();
				state = jsVal;
			} else {
				RemoveLastId();
				state = jsExpectId;
			}
			return true;

		case ']':
			if (InArray()) {
				if (doProcess) {
					ProcessField();
				}
				++arrayIndices[arrayDepth - 1];
				EndArray();
				state = jsEndVal;
			} else {
				state = jsError;

				jserror("CheckValueCompleted: ]");
			}
			return true;

		case '}':
			if (InArray()) {
				state = jsError;

				jserror("CheckValueCompleted: }");
			} else {
				if (doProcess) {
					ProcessField();
				}
				RemoveLastId();
				if (fieldId.strlen() == 0) {
					serialIoErrors = 0;

					if (cbs && cbs->EndReceivedMessage) {
						cbs->EndReceivedMessage();
					}
					state = jsBegin;
				} else {
					RemoveLastIdChar();
					state = jsEndVal;
				}
			}
			return true;

		default:
			return false;
		}
	}

#define dbg_if(cond, fmt, args...)                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		if (cond)                                                                                                      \
		{                                                                                                              \
			dbg(fmt, ##args);                                                                                          \
		}                                                                                                              \
	} while (0)

	// This is the JSON parser state machine
	void CheckInput(const unsigned char* rxBuffer, unsigned int len, bool log)
	{
		nextOut = 0;
		dbg_if(log, "CheckInput[%d]: %s", len, rxBuffer);
		while (len != nextOut) {
			char c = rxBuffer[nextOut];
			verbose("char %d: %c", nextOut, c);
			nextOut = (nextOut + 1) % (len + 1);
			if (c == '\n') {
				if (state == jsError) {
					dbg_if(log, "ParserErrorEncountered @ %d", nextOut);

					serialIoErrors++;

					if (cbs && cbs->ParserErrorEncountered) {
						cbs->ParserErrorEncountered(lastState, fieldId.c_str(), serialIoErrors); // Notify the consumer that we ran into an error
						error("rxBuffer: %s", rxBuffer);
						lastState = jsBegin;
					}
				}
				state = jsBegin;		// abandon current parse (if any) and start again
			} else {
				lastState = state;

				switch(state) {
				case jsBegin:			// initial state, expecting '{'
					if (c == '{') {
						if (cbs && cbs->StartReceivedMessage) {
							cbs->StartReceivedMessage();
						}
						state = jsExpectId;
						fieldVal.Clear();
						fieldId.Clear();
						arrayDepth = 0;
					}
					break;

				case jsExpectId:		// expecting a quoted ID
					switch (c) {
					case ' ':
						break;
					case '"':
						state = jsId;
						break;
					case '}':			// empty object, or extra comma at end of field list
						RemoveLastId();
						if (fieldId.strlen() == 0) {
							serialIoErrors = 0;

							if (cbs && cbs->EndReceivedMessage) {
								cbs->EndReceivedMessage();
							}
							state = jsBegin;
						} else {
							RemoveLastIdChar();
							state = jsEndVal;
						}
						break;
					default:
						state = jsError;

						error("jsError: jsExpectId, expected [\" or }] but got \"%c\"", c);
						break;
					}
					break;

				case jsId:				// expecting an identifier, or in the middle of one
					switch (c) {
					case '"':
						state = jsHadId;
						break;
					default:
						if (c < ' ') {
							state = jsError;

							jserror("jsId 1, expected \" but got \"%c\"", c);
						} else if (c != ':' && c != '^') {
							if (fieldId.cat(c)) {
								state = jsError;

								jserror("jsId 2, id not finished, received \"%c\"", c);
							}
						}
						break;
					}
					break;

				case jsHadId:			// had a quoted identifier, expecting ':'
					switch(c) {
					case ':':
						state = jsVal;
						break;
					case ' ':
						break;
					default:
						state = jsError;

						jserror("jsHadId, expected : but got \"%c\"", c);
						break;
					}
					break;

				case jsVal:				// had ':' or ':[', expecting value
					switch(c) {
					case ' ':
						break;
					case '"':
						fieldVal.Clear();
						state = jsStringVal;
						break;
					case '[':
						if (arrayDepth < MaxArrayNesting && !fieldId.cat('^')) {
							arrayIndices[arrayDepth] = 0;		// start an array
							++arrayDepth;
						} else {
							state = jsError;

							jserror("[, could not start array, current depth: %d", arrayDepth);
						}
						break;
					case ']':
						if (InArray()) {
							EndArray();			// empty array
							state = jsEndVal;
						} else {
							state = jsError;	// ']' received without a matching '[' first

							jserror("], not in array");
						}
						break;
					case '-':
						fieldVal.Clear();
						fieldVal.cat(c);
						state = jsNegIntVal;
						break;
					case '{':					// start of a nested object
						state = (!fieldId.cat(':')) ? jsExpectId : jsError;

						if (state == jsError) {
							jserror("{, failed to start nested object");
						}
						break;
					default:
						if (c >= '0' && c <= '9') {
							fieldVal.Clear();
							fieldVal.cat(c);	// must succeed because we just cleared fieldVal
							state = jsIntVal;
						}
						else if (c >= 'a' && c <= 'z') {
							fieldVal.Clear();
							fieldVal.cat(c);	// must succeed because we just cleared fieldVal
							state = jsCharsVal;
						} else {
							state = jsError;

							jserror("jsVal default, expected [a-z0-9] but got \"%c\"", c);
						}
					}
					break;

				case jsStringVal:		// just had '"' and expecting a string value
					switch (c) {
					case '"':
						ConvertUnicode();
						ProcessField();
						state = jsEndVal;
						break;
					case '\\':
						state = jsStringEscape;
						break;
					default:
						if (c < ' ') {
							state = jsError;

							jserror("jsStringVal, got \"%c\"", c);
						} else {
							fieldVal.cat(c);	// ignore any error so that long string parameters just get truncated
						}
						break;
					}
					break;

				case jsStringEscape:	// just had backslash in a string
					if (!fieldVal.IsFull()) {
						switch (c) {
						case '"':
						case '\\':
						case '/':
							if (fieldVal.cat(c)) {
								state = jsError;

								jserror("jsStringEscape 1, failed to append %c", c);
							}
							break;
						case 'n':
						case 't':
							if (fieldVal.cat(' ')) {		// replace newline and tab by space
								state = jsError;

								jserror("jsStringEscape 2, failed to append space");
							}
							break;
						case 'b':
						case 'f':
						case 'r':
						default:
							break;
						}
					}
					state = jsStringVal;
					break;

				case jsNegIntVal:		// had '-' so expecting a integer value
					state = (c >= '0' && c <= '9' && !fieldVal.cat(c)) ? jsIntVal : jsError;

						if (state == jsError) {
							jserror("jsNegIntVal, expected negative int but got %c", c);
						}
					break;

				case jsIntVal:			// receiving an integer value
					if (CheckValueCompleted(c, true)) {
						break;
					}

					if (c == '.') {
						state = (!fieldVal.cat(c)) ? jsFracVal : jsError;

						if (state == jsError) {
							jserror("jsIntVal, failed to append %c", c);
						}
					}
					else if (!(c >= '0' && c <= '9' && !fieldVal.cat(c))) {
						state = jsError;

						jserror("jsIntVal, expected [0-9] but got \"%c\", or failed to append to fieldVal", c);
					}
					break;

				case jsFracVal:			// receiving a fractional value
					if (CheckValueCompleted(c, true)) {
						break;
					}

					if (!(c >= '0' && c <= '9' && !fieldVal.cat(c))) {
						state = jsError;

						jserror("jsFracVal, expected [0-9] but got \"%c\", or failed to append to fieldVal", c);
					}
					break;

				case jsCharsVal:
					if (CheckValueCompleted(c, true)) {
						break;
					}

					if (!(c >= 'a' && c <= 'z' && !fieldVal.cat(c))) {
						state = jsError;

						jserror("jsCharsVal, expected [a-z] but got \"%c\", or failed to append to fieldVal", c);
					}
					break;

				case jsEndVal:			// had the end of a string or array value, expecting comma or ] or }
					if (CheckValueCompleted(c, false)) {
						break;
					}

					state = jsError;

					jserror("jsEndVal");
					break;

				case jsError:
					// Ignore all characters. State will be reset to jsBegin at the start of this function when we receive a newline.
					break;
				}

				if (lastState != state)
					dbg_if(log, "state %d -> %d", lastState, state);
#if DEBUG
#endif
			}
		}
	}

	// Called by the ISR to signify an error. We wait for the next end of line.
	void receiveError() {
		inError = true;
	}

}

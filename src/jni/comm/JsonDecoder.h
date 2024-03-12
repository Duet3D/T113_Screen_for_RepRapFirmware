/*
 * JsonDecoder.h
 *
 *  Created on: 23 Feb 2024
 *      Author: andy
 *
 *  This file is a modified version of the SerialIO.h source file from PanelDue
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/Hardware/SerialIo.h
 */

#ifndef JNI_LOGIC_HARDWARE_JSONDECODER_H_
#define JNI_LOGIC_HARDWARE_JSONDECODER_H_

#include "ecv.h"
#include <Duet3D/General/String.h>
#include <cstddef>
#include <sys/types.h>
#undef array
#undef result
#undef value
#undef in

namespace Comm
{
	constexpr size_t MaxArrayNesting = 4;

	class JsonDecoder
	{
	  public:
		// Enumeration to represent the json parsing state.
		// We don't allow nested objects or nested arrays, so we don't need a state stack.
		// An additional variable elementCount is 0 if we are not in an array, else the number of elements we have found
		// (including the current one)
		enum JsonState
		{
			jsBegin,		// initial state, expecting '{'
			jsExpectId,		// just had '{' so expecting a quoted ID
			jsId,			// expecting an identifier, or in the middle of one
			jsHadId,		// had a quoted identifier, expecting ':'
			jsVal,			// had ':', expecting value
			jsStringVal,	// had '"' and expecting or in a string value
			jsStringEscape, // just had backslash in a string
			jsIntVal,		// receiving an integer value
			jsNegIntVal,	// had '-' so expecting a integer value
			jsFracVal,		// receiving a fractional value
			jsEndVal,		// had the end of a string or _ecv_array value, expecting comma or ] or }
			jsCharsVal,		// receiving an alphanumeric value such as true, false, null
			jsError			// something went wrong
		};

		JsonDecoder() : serialIoErrors(0), nextOut(0), inError(false), arrayDepth(0) {}
		void CheckInput(const unsigned char* rxBuffer, unsigned int len);
		void ProcessReceivedValue(StringRef id, const char val[], const size_t indices[]);
		bool SetPrefix(const char* prefix) { return fieldPrefix.copy(prefix); }

	  private:
		void StartReceivedMessage(void);
		void EndReceivedMessage(void);
		void ProcessArrayEnd(const char id[], const size_t indices[]);
		void ParserErrorEncountered(int currentState, const char* id, int errors);
		void RemoveLastId();
		void RemoveLastIdChar();
		bool InArray();
		void ProcessField();
		void EndArray();
		void ConvertUnicode();
		bool CheckValueCompleted(char c, bool doProcess);
		void receiveError();

		// fieldId is the name of the field being received. A '^' character indicates the position of an _ecv_array
		// index, and a ':' character indicates a field separator.
		String<50> fieldPrefix;
		String<200> fieldId;
		String<4096> fieldVal; // rr_thumbnail seems to be biggest response we get
		JsonState state = jsBegin;
		JsonState lastState = jsBegin;
		int serialIoErrors;
		size_t nextOut;
		bool inError;
		size_t arrayIndices[MaxArrayNesting];
		size_t arrayDepth;
        
	};
} // namespace Comm
#endif /* JNI_LOGIC_HARDWARE_JSONDECODER_H_ */

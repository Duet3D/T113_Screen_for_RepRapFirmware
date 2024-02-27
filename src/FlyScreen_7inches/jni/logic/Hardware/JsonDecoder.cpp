/*
 * JsonDecoder.cpp
 *
 *  Created on: 23 Feb 2024
 *      Author: andy
 *
 *  This file is a modified version of the SerialIO.cpp source file from PanelDue
 *  The original can be found at https://github.com/Duet3D/PanelDueFirmware/blob/master/src/Hardware/SerialIo.cpp
 *
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_INFO

#include "UI/UserInterface.h"

#include "Commands.h"
#include "Communication.h"
#include "ControlCommands.h"
#include "JsonDecoder.h"
#include "ObjectModel/Alert.h"
#include "ObjectModel/Utils.h"
#include "Reset.h"
#include "SerialIo.h"
#include "UI/OmObserver.h"
#include "uart/UartContext.h"
#include "utils.h"
#include <string>

#include "Debug.h"

#define jserror(fmt, args...) error("jsError id=%s @ %d: " fmt, fieldId.c_str(), nextOut, ##args)

namespace Comm
{
	constexpr int parserMinErrors = 2;

	// Translation tables for combining characters.
	// The first character in each pair is the character that the combining mark is applied to.
	// The second character is what is translates to if the value is >=0x80, else the value it translates to minus
	// 0x0100.
	const char* _ecv_array const trGrave = "A\xC0"
										   "E\xC8"
										   "I\xCC"
										   "O\xD2"
										   "U\xD9"
										   "a\xE0"
										   "e\xE8"
										   "i\xEC"
										   "o\xF2"
										   "u\xF9";
	const char* _ecv_array const trAcute = "A\xC1"
										   "C\x06"
										   "E\xC9"
										   "I\xCD"
										   "L\x39"
										   "N\x43"
										   "O\xD3"
										   "R\x54"
										   "S\x5A"
										   "U\xDA"
										   "Y\xDD"
										   "Z\x79"
										   "a\xE1"
										   "c\x07"
										   "e\xE9"
										   "i\xED"
										   "l\x39"
										   "n\x44"
										   "o\xF3"
										   "r\x55"
										   "s\x5B"
										   "u\xFA"
										   "y\xFD"
										   "z\x7a";
	const char* _ecv_array const trCircumflex = "A\xC2"
												"C\x08"
												"E\xCA"
												"G\x1C"
												"H\x24"
												"I\xCE"
												"J\x34"
												"O\xD4"
												"S\x5C"
												"U\xDB"
												"W\x74"
												"Y\x76"
												"a\xE2"
												"c\x09"
												"e\xEA"
												"g\x1D"
												"h\x25"
												"i\xEE"
												"j\x35"
												"o\xF4"
												"s\x5D"
												"u\xFB"
												"w\x75"
												"y\x77";
	const char* _ecv_array const trTilde = "A\xC3"
										   "I\x28"
										   "N\xD1"
										   "O\xD5"
										   "U\x68"
										   "a\xE3"
										   "i\x29"
										   "n\xF1"
										   "o\xF5"
										   "u\x69";
	const char* _ecv_array const trBreve = "A\x02"
										   "E\x14"
										   "G\x1E"
										   "I\x2C"
										   "O\x4E"
										   "U\x6c"
										   "a\x03"
										   "e\x15"
										   "g\x1F"
										   "i\x2D"
										   "o\x4F"
										   "u\x6d";
	const char* _ecv_array const trUmlaut = "A\xC4"
											"E\xCB"
											"I\xCF"
											"O\xD6"
											"U\xDC"
											"Y\x78"
											"a\xE4"
											"e\xEB"
											"i\xEF"
											"o\xF6"
											"u\xFC"
											"y\xFF";
	const char* _ecv_array const trCircle = "A\xC5"
											"U\x6E"
											"a\xE5"
											"u\x6F";
	const char* _ecv_array const trCaron = "C\x0C"
										   "D\x0C"
										   "E\x1A"
										   "N\x47"
										   "R\x58"
										   "S\x60"
										   "T\x64"
										   "Z\x7D"
										   "c\x0D"
										   "d\x0F"
										   "e\x1B"
										   "n\x48"
										   "r\x59"
										   "s\x61"
										   "t\x65"
										   "z\x7E";
	const char* _ecv_array const trCedilla = "C\xC7"
											 "c\xE7";

	void JsonDecoder::StartReceivedMessage()
	{
		if (thumbnailContext.state == ThumbnailState::Init)
		{
			thumbnailContext.Init();
			ThumbnailInit(thumbnail);
			memset(&thumbnailData, 0, sizeof(thumbnailData));
		}
	}

	void JsonDecoder::EndReceivedMessage()
	{
		KickWatchdog();

		if (currentRespSeq != nullptr)
		{
			currentRespSeq->state = SeqStateOk;
			dbg("seq %s %d DONE", currentRespSeq->key, currentRespSeq->state);
			currentRespSeq = nullptr;
		}

		// FileManager::EndReceivedMessage();

		// Open M291 message box if required
		if (OM::currentAlert.mode != OM::Alert::Mode::None && OM::currentAlert.seq != OM::lastAlertSeq)
		{
			UI::POPUP_WINDOW->Open();
			OM::lastAlertSeq = OM::currentAlert.seq;
		}

		if (thumbnailContext.parseErr != 0 || thumbnailContext.err != 0)
		{
			error("thumbnail parseErr %d err %d.\n", thumbnailContext.parseErr, thumbnailContext.err);
			thumbnailContext.state = ThumbnailState::Init;
		}

#if 1 // && DEBUG
		if (thumbnail.imageFormat != Thumbnail::ImageFormat::Invalid)
		{
			dbg("filename %s offset %d size %d format %d width %d height %d\n",
				thumbnailContext.filename.c_str(),
				thumbnailContext.offset,
				thumbnailContext.size,
				thumbnail.imageFormat,
				thumbnail.width,
				thumbnail.height);
		}
#endif
		int ret;

		if (stopThumbnailRequest)
		{
			warn("Thumbnail request cancelled");
			thumbnailContext.state = ThumbnailState::Init;
			thumbnailRequestInProgress = false;
		}

		if (thumbnail.imageFormat == Thumbnail::ImageFormat::Invalid)
			thumbnailRequestInProgress = false;

		verbose("thumbnailContext state %d", thumbnailContext.state);
		switch (thumbnailContext.state)
		{
		case ThumbnailState::Init:
		case ThumbnailState::DataRequest:
		case ThumbnailState::DataWait:
			break;
		case ThumbnailState::Header:
			if (!ThumbnailIsValid(thumbnail))
			{
				error("thumbnail meta invalid.\n");
				break;
			}
			if (!thumbnail.New(thumbnail.width, thumbnail.height, thumbnailContext.filename.c_str()))
			{
				error("Failed to create thumbnail file.");
				break;
			}
			thumbnailContext.state = ThumbnailState::DataRequest;
			break;
		case ThumbnailState::Data:
			if (!ThumbnailDataIsValid(thumbnailData))
			{
				error("thumbnail meta or data invalid.\n");
				thumbnailContext.state = ThumbnailState::Init;
				break;
			}
			if ((ret = ThumbnailDecodeChunk(thumbnail, thumbnailData)) < 0)
			{
				error("failed to decode thumbnail chunk %d.\n", ret);
				thumbnailContext.state = ThumbnailState::Init;
				break;
			}
			if (thumbnailContext.next == 0)
			{
				thumbnail.Close();
				info("Updating thumbnail %s", thumbnailContext.filename.c_str());
				UI::GetThumbnail()->setText("");
				thumbnailContext.state = ThumbnailState::Init;
				thumbnailRequestInProgress = false;
			}
			else
			{
				thumbnailContext.state = ThumbnailState::DataRequest;
			}

			if (stopThumbnailRequest)
			{
				warn("Thumbnail request cancelled");
				thumbnailContext.state = ThumbnailState::Init;
				thumbnailRequestInProgress = false;
			}
			break;
		}
	}

	// Public functions called by the SerialIo module
	void JsonDecoder::ProcessReceivedValue(StringRef id, const char data[], const size_t indices[])
	{
		verbose(
			"id %s, data %s, indices [%d|%d|%d|%d]", id.c_str(), data, indices[0], indices[1], indices[2], indices[3]);
		if (StringStartsWith(id.c_str(), "result"))
		{
			// We might either get something like:
			// * "result[optional modified]:[key]:[field]" for a live response or
			// * "result[optional modified]:[field]" for a detailed response
			// If live response remove "result:"
			// else replace "result" by "key" (do NOT replace anything beyond "result" as there might be an _ecv_array
			// modifier)

			id.Erase(0, 6);
			if (currentRespSeq != nullptr)
			{
				id.Prepend(currentRespSeq->key);
			}
			else
			{
				// if empty key also erase the colon
				id.Erase(0);
			}
		}

		// search for key in observerMap
		verbose("searching for observers for %s\n", id.c_str());
		auto observers = UI::observerMap.GetObservers(id.c_str());
		if (observers.size() != 0)
		{
			dbg("found %d observers for %s\n", observers.size(), id.c_str());
			for (auto& observer : observers)
			{
				observer.Update(data, indices);
			}
		}

		const FieldTableEntry* searchResult = SearchFieldTable(id.c_str());
		// no matching key found
		if (!searchResult)
		{
			verbose("no matching key found for %s\n", id.c_str());
			return;
		}
		const ReceivedDataEvent rde = searchResult->val;
		verbose("event: %s(%d) data '%s'\n", searchResult->key, searchResult->val, data);
		switch (rde)
		{
		// M409 section
		// TODO: Uncomment stuff below related to UI/OM
		case rcvKey: {
			// try a quick check otherwise search for key
			if (currentReqSeq && (strcasecmp(data, currentReqSeq->key) == 0))
			{
				currentRespSeq = currentReqSeq;
			}
			else
			{
				currentRespSeq = FindSeqByKey(data);
			}

			if (currentRespSeq == nullptr)
			{
				break;
			}
		}
		break;

		// Seqs section
		case rcvSeqsBoards:
		case rcvSeqsDirectories:
		case rcvSeqsFans:
		case rcvSeqsHeat:
		case rcvSeqsInputs:
		case rcvSeqsJob:
		case rcvSeqsMove:
		case rcvSeqsNetwork:
		case rcvSeqsReply:
		case rcvSeqsScanner:
		case rcvSeqsSensors:
		case rcvSeqsSpindles:
		case rcvSeqsState:
		case rcvSeqsTools:
		case rcvSeqsVolumes: {
			int32_t ival;

			if (GetInteger(data, ival))
			{
				UpdateSeq(rde, ival);
			}
			break;
		}

		case rcvM36Filename:
			thumbnailRequestInProgress = true;
			thumbnailContext.filename.copy(data);
			break;

		case rcvM36GeneratedBy:
			// The thumbnail field is not present if no thumbnail is in the file so we need to use this field which is
			// after the thumbnail key
			if (thumbnail.imageFormat == Thumbnail::ImageFormat::Invalid)
			{
				thumbnailRequestInProgress = false;
			}
			break;

		case rcvM36ThumbnailsFormat:
			info("thumbnail format %s", data);
			if (thumbnail.SetImageFormat(data))
			{
				thumbnailContext.state = ThumbnailState::Header;
			}
			break;
		case rcvM36ThumbnailsHeight:
			uint32_t height;
			if (GetUnsignedInteger(data, height))
			{
				info("thumbnail height %d", height);
				thumbnail.height = height;
			}
			break;
		case rcvM36ThumbnailsOffset:
			uint32_t offset;
			if (GetUnsignedInteger(data, offset))
			{
				info("thumbnail offset %d", offset);
				thumbnailContext.next = offset;
				dbg("receive initial offset %d.\n", offset);
			}
			break;
		case rcvM36ThumbnailsSize:
			uint32_t size;
			if (GetUnsignedInteger(data, size))
			{
				info("thumbnail size %d", size);
				thumbnailContext.size = size;
			}
			break;
		case rcvM36ThumbnailsWidth:
			uint32_t width;
			if (GetUnsignedInteger(data, width))
			{
				info("thumbnail width %d", width);
				thumbnail.width = width;
			}
			break;

		case rcvM361ThumbnailData:
			thumbnailRequestInProgress = true;
			if (stopThumbnailRequest)
			{
				warn("Thumbnail request cancelled");
				thumbnailContext.state = ThumbnailState::Init;
				thumbnailRequestInProgress = false;
				break;
			}
			info("thumbnail data %d", strlen(data));
			thumbnailData.size = std::min(strlen(data), sizeof(thumbnailData.buffer));
			memcpy(thumbnailData.buffer, data, thumbnailData.size);
			thumbnailContext.state = ThumbnailState::Data;
			break;
		case rcvM361ThumbnailErr:
			if (!GetInteger(data, thumbnailContext.err))
			{
				warn("thumbnail err %s", data);
				thumbnailContext.parseErr = -1;
			}
			break;
		case rcvM361ThumbnailFilename:
			if (!thumbnailContext.filename.Equals(data))
			{
				warn("thumbnail filename error \"%s\"", data);
				thumbnailContext.parseErr = -2;
			}
			break;
		case rcvM361ThumbnailNext:
			if (!GetUnsignedInteger(data, thumbnailContext.next))
			{
				warn("thumbnail next error \"%s\"", data);
				thumbnailContext.parseErr = -3;
				break;
			}
			dbg("thumbnail receive next offset %d.", thumbnailContext.next);
			break;
		case rcvM361ThumbnailOffset:
			if (!GetUnsignedInteger(data, thumbnailContext.offset))
			{
				warn("thumbnail offset error \"%s\"", data);
				thumbnailContext.parseErr = -4;
				break;
			}
			dbg("thumbnail receive current offset %d.", thumbnailContext.offset);
			break;

		case rcvControlCommand: {
			const ControlCommandMapEntry key = (ControlCommandMapEntry){data, ControlCommand::invalid};
			const ControlCommandMapEntry* controlCommandFromMap =
				(ControlCommandMapEntry*)bsearch(&key,
												 controlCommandMap,
												 ARRAY_SIZE(controlCommandMap),
												 sizeof(ControlCommandMapEntry),
												 compareKey<ControlCommandMapEntry>);
			const ControlCommand controlCommand =
				(controlCommandFromMap != nullptr) ? controlCommandFromMap->val : ControlCommand::invalid;
			switch (controlCommand)
			{
			case ControlCommand::eraseAndReset:
				EraseAndReset(); // Does not return
				break;
			case ControlCommand::reset:
				Reset(); // Does not return
				break;
			default:
				// Invalid command. Just ignore.
				break;
			}
			break;
		}

		default:
			break;
		}
	}

	// Public function called when the serial I/O module finishes receiving an array of values
	void JsonDecoder::ProcessArrayEnd(const char id[], const size_t indices[])
	{
		// search for key in observerMap
		auto observers = UI::observerMapArrayEnd.GetObservers(id);
		if (observers.size() != 0)
		{
			for (auto& observer : observers)
			{
				observer.Update(indices);
			}
		}
	}

	void JsonDecoder::ParserErrorEncountered(int currentState, const char* id, int errors)
	{
		(void)currentState;

		if (errors > parserMinErrors)
		{
			UI::CONSOLE->AddResponse(utils::format("Warning: received %d malformed responses.", errors).c_str());
			error("Warning: received %d malformed responses for id \"%s\"", errors, id);
		}
		if (currentRespSeq == nullptr)
		{
			return;
		}

		currentRespSeq->state = SeqStateError;
	}

	void JsonDecoder::RemoveLastId()
	{
		verbose("%s, len: %d", fieldId.c_str(), fieldId.strlen());
		size_t index = fieldId.strlen();
		while (index != 0 && fieldId[index - 1] != '^' && fieldId[index - 1] != ':')
		{
			--index;
		}
		fieldId.Truncate(index);

		verbose("RemoveLastId: %s, len: %d", fieldId.c_str(), fieldId.strlen());
	}

	void JsonDecoder::RemoveLastIdChar()
	{
		verbose();

		if (fieldId.strlen() != 0)
		{
			fieldId.Truncate(fieldId.strlen() - 1);
		}
	}

	bool JsonDecoder::InArray()
	{
		verbose();

		return fieldId.strlen() > 0 && fieldId[fieldId.strlen() - 1] == '^';
	}

	void JsonDecoder::ProcessField()
	{
		if (state == jsCharsVal)
		{
			if (fieldVal.Equals("null"))
			{
				fieldVal.Clear(); // so that we can distinguish null from an empty string
			}
		}
		dbg("%s: %s", fieldId.c_str(), fieldVal.c_str());
		ProcessReceivedValue(fieldId.GetRef(), fieldVal.c_str(), arrayIndices);
		fieldVal.Clear();
	}

	void JsonDecoder::EndArray()
	{
		verbose();

		ProcessArrayEnd(fieldId.c_str(), arrayIndices);

		if (arrayDepth != 0)
		{ // should always be true
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
	void JsonDecoder::ConvertUnicode()
	{
		unsigned int numContinuationBytesLeft = 0;
		uint32_t charVal;
		for (size_t i = 0; i < fieldVal.strlen();)
		{
			const unsigned char c = fieldVal[i++];
			if (numContinuationBytesLeft == 0)
			{
				if (c >= 0x80)
				{
					if ((c & 0xE0) == 0xC0)
					{
						charVal = (uint32_t)(c & 0x1F);
						numContinuationBytesLeft = 1;
					}
					else if ((c & 0xF0) == 0xE0)
					{
						charVal = (uint32_t)(c & 0x0F);
						numContinuationBytesLeft = 2;
					}
					else if ((c & 0xF8) == 0xF0)
					{
						charVal = (uint32_t)(c & 0x07);
						numContinuationBytesLeft = 3;
					}
					else if ((c & 0xFC) == 0xF8)
					{
						charVal = (uint32_t)(c & 0x03);
						numContinuationBytesLeft = 4;
					}
					else if ((c & 0xFE) == 0xFC)
					{
						charVal = (uint32_t)(c & 0x01);
						numContinuationBytesLeft = 5;
					}
				}
			}
			else if ((c & 0xC0) == 0x80)
			{
				charVal = (charVal << 6) | (c & 0x3F);
				--numContinuationBytesLeft;
				if (numContinuationBytesLeft == 0)
				{
					const char* _ecv_array trtab;
					switch (charVal)
					{
					case 0x0300: // grave accent
						trtab = trGrave;
						break;
					case 0x0301: // acute accent
						trtab = trAcute;
						break;
					case 0x0302: // circumflex
						trtab = trCircumflex;
						break;
					case 0x0303: // tilde
						trtab = trTilde;
						break;
					case 0x0306: // breve
						trtab = trBreve;
						break;
					case 0x0308: // umlaut
						trtab = trUmlaut;
						break;
					case 0x030A: // small circle
						trtab = trCircle;
						break;
					case 0x030C: // caron
						trtab = trCaron;
						break;
					case 0x0327: // cedilla
						trtab = trCedilla;
						break;
					default:
						trtab = nullptr;
						break;
					}

					// If it is a diacritical mark that we handle, try to combine it with the previous character.
					// The diacritical marks are in the range 03xx so they are encoded as 2 UTF8 bytes.
					if (trtab != nullptr && i > 2)
					{
						const char c2 = fieldVal[i - 3];
						while (*trtab != 0 && *trtab != c2)
						{
							trtab += 2;
						}
						if (*trtab != 0)
						{
							// Get the translated character and encode it as 2 UTF8 bytes
							uint16_t c3 = (uint16_t)(uint8_t)trtab[1];
							if (c3 < 0x80)
							{
								c3 |= 0x0100;
							}
							fieldVal[i - 3] = (c3 >> 6) | 0xC0;
							fieldVal[i - 2] = (c3 & 0x3F) | 0x80;
							fieldVal.Erase(i - 1);
							--i;
						}
					}
				}
			}
			else
			{
				// Bad UTF8 state
				numContinuationBytesLeft = 0;
			}
		}
	}

	// Check whether the incoming character signals the end of the value. If it does, process it and return true.
	bool JsonDecoder::CheckValueCompleted(char c, bool doProcess)
	{
		verbose();

		switch (c)
		{
		case ',':
			if (doProcess)
			{
				ProcessField();
			}
			if (InArray())
			{
				++arrayIndices[arrayDepth - 1];
				fieldVal.Clear();
				state = jsVal;
			}
			else
			{
				RemoveLastId();
				state = jsExpectId;
			}
			return true;

		case ']':
			if (InArray())
			{
				if (doProcess)
				{
					ProcessField();
				}
				++arrayIndices[arrayDepth - 1];
				EndArray();
				state = jsEndVal;
			}
			else
			{
				state = jsError;

				jserror("CheckValueCompleted: ]");
			}
			return true;

		case '}':
			if (InArray())
			{
				state = jsError;

				jserror("CheckValueCompleted: }");
			}
			else
			{
				if (doProcess)
				{
					ProcessField();
				}
				RemoveLastId();
				if (fieldId.strlen() == 0 || fieldId.Equals(fieldPrefix.c_str()))
				{
					serialIoErrors = 0;

					EndReceivedMessage();

					state = jsBegin;
				}
				else
				{
					RemoveLastIdChar();
					state = jsEndVal;
				}
			}
			return true;

		default:
			return false;
		}
	}

	// This is the JSON parser state machine
	void JsonDecoder::CheckInput(const unsigned char* rxBuffer, unsigned int len)
	{
		nextOut = 0;
		dbg("CheckInput[%d]: %s", len, rxBuffer);
		while (len != nextOut)
		{
			char c = rxBuffer[nextOut];
			verbose("char %d: %c", nextOut, c);
			nextOut = (nextOut + 1) % (len + 1);
			if (c == '\n')
			{
				if (state == jsError)
				{
					error("ParserErrorEncountered @ %d", nextOut);

					serialIoErrors++;

					ParserErrorEncountered(lastState,
										   fieldId.c_str(),
										   serialIoErrors); // Notify the consumer that we ran into an error
					error("rxBuffer: %s", rxBuffer);
					lastState = jsBegin;
				}
				state = jsBegin; // abandon current parse (if any) and start again
			}
			else
			{
				lastState = state;

				switch (state)
				{
				case jsBegin: // initial state, expecting '{'
					if (c == '{')
					{
						StartReceivedMessage();
						state = jsExpectId;
						fieldVal.Clear();
						fieldId.Clear();
						if (!fieldPrefix.IsEmpty())
						{
							fieldId.copy(fieldPrefix.c_str());
						}
						arrayDepth = 0;
					}
					break;

				case jsExpectId: // expecting a quoted ID
					switch (c)
					{
					case ' ':
						break;
					case '"':
						state = jsId;
						break;
					case '}': // empty object, or extra comma at end of field list
						RemoveLastId();
						if (fieldId.strlen() == 0 || fieldId.Equals(fieldPrefix.c_str()))
						{
							serialIoErrors = 0;
							EndReceivedMessage();
							state = jsBegin;
						}
						else
						{
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

				case jsId: // expecting an identifier, or in the middle of one
					switch (c)
					{
					case '"':
						state = jsHadId;
						break;
					default:
						if (c < ' ')
						{
							state = jsError;

							jserror("jsId 1, expected \" but got \"%c\"", c);
						}
						else if (c != ':' && c != '^')
						{
							if (fieldId.cat(c))
							{
								state = jsError;

								jserror("jsId 2, id not finished, received \"%c\"", c);
							}
						}
						break;
					}
					break;

				case jsHadId: // had a quoted identifier, expecting ':'
					switch (c)
					{
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

				case jsVal: // had ':' or ':[', expecting value
					switch (c)
					{
					case ' ':
						break;
					case '"':
						fieldVal.Clear();
						state = jsStringVal;
						break;
					case '[':
						if (arrayDepth < MaxArrayNesting && !fieldId.cat('^'))
						{
							arrayIndices[arrayDepth] = 0; // start an array
							++arrayDepth;
						}
						else
						{
							state = jsError;

							jserror("[, could not start array, current depth: %d", arrayDepth);
						}
						break;
					case ']':
						if (InArray())
						{
							EndArray(); // empty array
							state = jsEndVal;
						}
						else
						{
							state = jsError; // ']' received without a matching '[' first

							jserror("], not in array");
						}
						break;
					case '-':
						fieldVal.Clear();
						fieldVal.cat(c);
						state = jsNegIntVal;
						break;
					case '{': // start of a nested object
						state = (!fieldId.cat(':')) ? jsExpectId : jsError;

						if (state == jsError)
						{
							jserror("{, failed to start nested object");
						}
						break;
					default:
						if (c >= '0' && c <= '9')
						{
							fieldVal.Clear();
							fieldVal.cat(c); // must succeed because we just cleared fieldVal
							state = jsIntVal;
						}
						else if (c >= 'a' && c <= 'z')
						{
							fieldVal.Clear();
							fieldVal.cat(c); // must succeed because we just cleared fieldVal
							state = jsCharsVal;
						}
						else
						{
							state = jsError;

							jserror("jsVal default, expected [a-z0-9] but got \"%c\"", c);
						}
					}
					break;

				case jsStringVal: // just had '"' and expecting a string value
					switch (c)
					{
					case '"':
						ConvertUnicode();
						ProcessField();
						state = jsEndVal;
						break;
					case '\\':
						state = jsStringEscape;
						break;
					default:
						if (c < ' ')
						{
							state = jsError;

							jserror("jsStringVal, got \"%c\"", c);
						}
						else
						{
							fieldVal.cat(c); // ignore any error so that long string parameters just get truncated
						}
						break;
					}
					break;

				case jsStringEscape: // just had backslash in a string
					if (!fieldVal.IsFull())
					{
						switch (c)
						{
						case '"':
						case '\\':
						case '/':
							if (fieldVal.cat(c))
							{
								state = jsError;

								jserror("jsStringEscape 1, failed to append %c", c);
							}
							break;
						case 'n':
						case 't':
							if (fieldVal.cat(' '))
							{ // replace newline and tab by space
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

				case jsNegIntVal: // had '-' so expecting a integer value
					state = (c >= '0' && c <= '9' && !fieldVal.cat(c)) ? jsIntVal : jsError;

					if (state == jsError)
					{
						jserror("jsNegIntVal, expected negative int but got %c", c);
					}
					break;

				case jsIntVal: // receiving an integer value
					if (CheckValueCompleted(c, true))
					{
						break;
					}

					if (c == '.')
					{
						state = (!fieldVal.cat(c)) ? jsFracVal : jsError;

						if (state == jsError)
						{
							jserror("jsIntVal, failed to append %c", c);
						}
					}
					else if (!(c >= '0' && c <= '9' && !fieldVal.cat(c)))
					{
						state = jsError;

						jserror("jsIntVal, expected [0-9] but got \"%c\", or failed to append to fieldVal", c);
					}
					break;

				case jsFracVal: // receiving a fractional value
					if (CheckValueCompleted(c, true))
					{
						break;
					}

					if (!(c >= '0' && c <= '9' && !fieldVal.cat(c)))
					{
						state = jsError;

						jserror("jsFracVal, expected [0-9] but got \"%c\", or failed to append to fieldVal", c);
					}
					break;

				case jsCharsVal:
					if (CheckValueCompleted(c, true))
					{
						break;
					}

					if (!(c >= 'a' && c <= 'z' && !fieldVal.cat(c)))
					{
						state = jsError;

						jserror("jsCharsVal, expected [a-z] but got \"%c\", or failed to append to fieldVal", c);
					}
					break;

				case jsEndVal: // had the end of a string or array value, expecting comma or ] or }
					if (CheckValueCompleted(c, false))
					{
						break;
					}

					state = jsError;

					jserror("jsEndVal, expected comma or ] or }");
					break;

				case jsError:
					// Ignore all characters. State will be reset to jsBegin at the start of this function when we
					// receive a newline.
					break;
				}

				if (lastState != state)
					verbose("state %d -> %d", lastState, state);
#if DEBUG
#endif
			}
		}
	}

	// Called by the ISR to signify an error. We wait for the next end of line.
	void JsonDecoder::receiveError()
	{
		inError = true;
	}
} // namespace Comm

/*
 * UserInterface.hpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc & Andy Everitt
 */

#ifndef JNI_LOGIC_UI_USERINTERFACE_HPP_
#define JNI_LOGIC_UI_USERINTERFACE_HPP_

#include <map>
#include <vector>
#include "Communication.hpp"
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringFunctions.h>

#define ELEMENT_FLOAT_ARGS const float &val, const size_t indices[]
#define ELEMENT_INT_ARGS const int32_t &val, const size_t indices[]
#define ELEMENT_UINT_ARGS const uint32_t &val, const size_t indices[]
#define ELEMENT_BOOL_ARGS const bool &val, const size_t indices[]

#define ELEMENT_TEMPLATE(key, callback, type, convertor) \
	UI::Element{ \
		key, \
		[&](const char *data, const size_t indices[]) \
		{ \
			type val; \
			if (convertor(data, val)) \
			{ \
				callback(val, indices); \
			} \
		} \
	}

#define ELEMENT_FLOAT(key, callback) ELEMENT_TEMPLATE(key, callback, float, Comm::GetFloat)
#define ELEMENT_INT(key, callback) ELEMENT_TEMPLATE(key, callback, int32_t, Comm::GetInteger)
#define ELEMENT_UINT(key, callback) ELEMENT_TEMPLATE(key, callback, uint32_t, Comm::GetUnsignedInteger)
#define ELEMENT_BOOL(key, callback) ELEMENT_TEMPLATE(key, callback, bool, Comm::GetBool)

#define ELEMENT_IF_CHANGED_TEMPLATE(key, callback, type, convertor) \
	ELEMENT_TEMPLATE(key, \
		([](const type val, const size_t indices[]) \
		{ \
			int32_t packedIndex = 0; \
			packedIndex |= (static_cast<int32_t>(indices[0]) & 0xFF) << 24; \
			packedIndex |= (static_cast<int32_t>(indices[1]) & 0xFF) << 16; \
			packedIndex |= (static_cast<int32_t>(indices[2]) & 0xFF) << 8; \
			packedIndex |= (static_cast<int32_t>(indices[3]) & 0xFF); \
			static std::map<int32_t, type> prevValMap; \
			auto &prevVal = prevValMap[packedIndex]; \
			if (val != prevVal) \
			{ \
				prevVal = val; \
				callback(val, indices); \
			} \
			else \
			{ \
				LOGD("key %s[%d|%d|%d|%d] hasn't changed, skipping update", key, indices[0], indices[1], indices[2], indices[3]); \
			} \
		}), type, convertor \
	)

#define ELEMENT_FLOAT_IF_CHANGED(key, callback) ELEMENT_IF_CHANGED_TEMPLATE(key, callback, float, Comm::GetFloat)
#define ELEMENT_INT_IF_CHANGED(key, callback) ELEMENT_IF_CHANGED_TEMPLATE(key, callback, int32_t, Comm::GetInteger)
#define ELEMENT_UINT_IF_CHANGED(key, callback) ELEMENT_IF_CHANGED_TEMPLATE(key, callback, uint32_t, Comm::GetUnsignedInteger)
#define ELEMENT_BOOL_IF_CHANGED(key, callback) ELEMENT_IF_CHANGED_TEMPLATE(key, callback, bool, Comm::GetBool)

typedef void (*ui_update_cb)(const char *data, const size_t arrayIndices[]);

// Custom comparator for string literals at compile time
struct ConstCharComparator
{
	constexpr bool operator()(const char* a, const char* b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

namespace UI
{
	class Element
	{
	public:
		Element(const char *key, ui_update_cb cb);
		void Init();
		void Update(const char data[], const size_t arrayIndices[]);

		Element *next;
		static Element *head;
	private:
		const char *key;
		ui_update_cb cb;
	};

	class ElementMap
	{
	public:
		void RegisterElement(const char *key, const Element& element); // Method to add a pair to the data store
		const std::vector<Element>& GetElements(const char* key) const;	// Method to retrieve the elements associated with a key

	private:
		std::map<const char*, std::vector<Element>, ConstCharComparator> elementsMap;
	};

	extern ElementMap elementMap;
}

#endif /* JNI_LOGIC_UI_USERINTERFACE_HPP_ */

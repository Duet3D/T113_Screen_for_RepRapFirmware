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
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringFunctions.h>

typedef void (*ui_update_cb)(const char *data, const size_t arrayIndices);

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
		void Update(const char data[], const size_t arrayIndices);

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

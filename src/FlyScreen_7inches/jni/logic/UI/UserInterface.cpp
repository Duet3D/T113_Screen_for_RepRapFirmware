/*
 * UserInterface.cpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc & Andy Everitt
 */

#include <UI/UserInterface.hpp>

#include <ObjectModel/Axis.hpp>
#include <ObjectModel/Utils.hpp>

#define DEBUG (1)
#include "Debug.hpp"

namespace UI
{
	ElementMap elementMap;
	Element *Element::head = nullptr;

	Element::Element(const char *key, ui_update_cb cb) :
			key(key), cb(cb)
	{
		if (head == nullptr)
		{
			head = this;
		}
		else
		{
			Element *current = head;
			while (current->next != nullptr)
			{
				current = current->next;
			}
			current->next = this;
		}
	}

	void Element::Init()
	{
		dbg("Registering element against key %s", key);
		elementMap.RegisterElement(key, *this);
#if DEBUG
		int size = elementMap.GetElements(key).size();
		dbg("%d elements registered against key %s", size, key);
#endif
	}

	void Element::Update(const char data[], const size_t arrayIndices)
	{
		if (cb != nullptr)
		{
			cb(data, arrayIndices);
		}
	}

	void ElementMap::RegisterElement(const char *key, const Element& element)
	{
		auto& elementList = elementsMap[key];
		elementList.push_back(element);
	}

	// Method to retrieve the elements associated with a key
	const std::vector<Element>& ElementMap::GetElements(const char* key) const
	{
		static const std::vector<Element> emptyVector; // Return an empty vector if key not found
		auto it = elementsMap.find(key);
		return (it != elementsMap.end()) ? it->second : emptyVector;
	}
}

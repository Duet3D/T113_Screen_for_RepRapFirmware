/*
 * ListHelpers.h
 *
 *  Created on: 17 Feb 2021
 *      Author: manuel
 */

#ifndef SRC_OBJECTMODEL_LISTHELPERS_HPP_
#define SRC_OBJECTMODEL_LISTHELPERS_HPP_

#include "Debug.h"

//#include <cstdint>
#include <Duet3D/General/function_ref.h>
#include <sys/types.h>

template <typename L, typename T>
T* GetOrCreate(L& list, const size_t index, const bool create, const bool silent = false)
{
	const size_t count = list.Size();
	for (size_t i = 0; i < count; ++i)
	{
		if (list[i]->index == index)
		{
			verbose("Getting index=%d", index);
			return list[i];
		}
	}

	if (create && !list.Full())
	{
		verbose("Creating index=%d", index);
		T* elem = new T;
		elem->Reset();
		elem->index = index;
		list.Add(elem);
		list.Sort([] (T* e1, T* e2) { return e1->index > e2->index; });
		return elem;
	}

	if (!silent)
		error("Failed to get%s index=%d", create ? " or create" : "", index);
	return nullptr;
}

template<typename L, typename T>
T* Find(L& list, function_ref<bool(T*)> filter)
{
	const size_t count = list.Size();
	for (size_t i = 0; i < count; ++i)
	{
		if (filter(list[i]))
		{
			return list[i];
		}
	}
	return nullptr;
}

template<typename L, typename T>
size_t Remove(L& list, const size_t index, const bool allFollowing)
{
	// Nothing to do on an empty list or
	// if the last element is already smaller than what we look for
	if (list.IsEmpty() || list[list.Size()-1]->index < index)
	{
		// dbg("Nothing removed");
		return 0;
	}

	size_t removed = 0;
	for (size_t i = list.Size(); i != 0;)
	{
		--i;
		T* elem = list[i];
		if (elem->index == index || (allFollowing && elem->index > index))
		{
			// dbg("Removing index=%d", i);
			list.Erase(i);
			delete elem;
			++removed;
			if (!allFollowing)
			{
				break;
			}
		}
	}
	return removed;
}

#endif /* SRC_OBJECTMODEL_LISTHELPERS_HPP_ */

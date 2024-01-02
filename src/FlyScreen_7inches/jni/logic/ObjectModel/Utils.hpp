/*
 * ObjectModel.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: manuel
 */

#ifndef SRC_OBJECTMODEL_UTILS_HPP_
#define SRC_OBJECTMODEL_UTILS_HPP_

//#include <cstdint>
#include <sys/types.h>
#include <string>
#include <Duet3D/General/Vector.hpp>
#include <UI/UserInterfaceConstants.hpp>

// Custom comparator for string literals at compile time
struct ConstCharComparator
{
	constexpr bool operator()(const char* a, const char* b) const
	{
		return strcmp(a, b) < 0;
	}
};

template <typename T>
int compareKey(const void *lp, const void *rp)
{
	return strcasecmp(((T *)lp)->key, ((T *)rp)->key);
}

template <typename T>
int compareValue(const void *lp, const void *rp)
{
	if (((T *)lp)->val < ((T *)rp)->val)
		return -1;
	else if (((T *)lp)->val > ((T *)rp)->val)
		return 1;
	else
		return 0;
}

namespace OM {
}

#endif /* SRC_OBJECTMODEL_UTILS_HPP_ */

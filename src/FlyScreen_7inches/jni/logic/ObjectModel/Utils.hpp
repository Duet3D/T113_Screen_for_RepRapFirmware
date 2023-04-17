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
#include <Duet3D/General/Vector.hpp>
#include <UI/UserInterfaceConstants.hpp>

namespace OM {
	typedef Vector<uint8_t, MaxSlots> Slots;

	void GetHeaterSlots(
			const size_t heaterIndex,
			Slots& slots,
			const bool addTools = true,
			const bool addBeds = true,
			const bool addChambers = true);
}

#endif /* SRC_OBJECTMODEL_UTILS_HPP_ */

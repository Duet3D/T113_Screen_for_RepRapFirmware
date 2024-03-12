/*
 * Heat.h
 *
 *  Created on: 19 Dec 2023
 *      Author: Andy Everitt
 */

#ifndef JNI_OBJECTMODEL_FAN_HPP_
#define JNI_OBJECTMODEL_FAN_HPP_

#include <sys/types.h>
#include <Duet3D/General/Bitmap.h>
#include <Duet3D/General/FreelistManager.h>
#include <Duet3D/General/String.h>
#include <Duet3D/General/StringRef.h>
#include <Duet3D/General/function_ref.h>
#include <UI/UserInterfaceConstants.h>

namespace OM
{
	class Fan
	{
	public:
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<Fan>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<Fan>(p); }

		uint8_t index;	// This is the fan number
		float actualValue;
		float requestedValue;
		int32_t rpm;

		void Reset();
	};

	Fan* GetFan(const size_t fanIndex);
	Fan* GetFanBySlot(const size_t index);
	Fan* GetOrCreateFan(const size_t fanIndex);
	const size_t GetFanCount();
	bool IterateFansWhile(function_ref<bool(Fan *&, size_t)> func, const size_t startAt = 0);
	bool UpdateFanActualVal(const size_t fanIndex, const float val);
	bool UpdateFanRequestedVal(const size_t fanIndex, const float val);
	bool UpdateFanRpm(const size_t fanIndex, const int32_t val);
	size_t RemoveFan(const size_t index, const bool allFollowing);
}

#endif /* JNI_OBJECTMODEL_FAN_HPP_ */

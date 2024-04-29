/*
 * Job.h
 *
 *  Created on: 10 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_OBJECTMODEL_JOB_HPP_
#define JNI_OBJECTMODEL_JOB_HPP_

#include "Duet3D/General/FreelistManager.h"
#include "Duet3D/General/StringRef.h"
#include "Duet3D/General/function_ref.h"
#include <string>
#include <sys/types.h>

namespace OM
{
	struct JobObject
	{
		void* operator new(size_t) noexcept { return FreelistManager::Allocate<JobObject>(); }
		void operator delete(void* p) noexcept { FreelistManager::Release<JobObject>(p); }

		struct Point
		{
			int32_t x[2];
			int32_t y[2];
		};

		size_t index;
		bool cancelled;
		std::string name;
		Point bounds;

		void Reset();
	};

	enum class RemainingTimeType
	{
		filament = 0,
		file,
		slicer,
		simulated
	};

	void SetJobName(const char* name);
	const std::string& GetJobName();

	void SetLastJobName(const char* name);
	const std::string& GetLastJobName();

	void SetPrintTime(const uint32_t printTime);
	const uint32_t GetPrintTime();

	void SetPrintDuration(const uint32_t printDuration);
	const uint32_t GetPrintDuration();

	void SetPrintRemaining(RemainingTimeType type, const uint32_t printRemaining);
	const uint32_t GetPrintRemaining(RemainingTimeType type);

	void SetCurrentJobObject(int8_t index);
	const int8_t GetCurrentJobObjectIndex();

	JobObject* GetJobObject(const size_t index);
	JobObject* GetOrCreateJobObject(const size_t index);
	size_t GetJobObjectCount();
	bool IterateJobObjectsWhile(function_ref<bool(JobObject*&, size_t)> func, const size_t startAt = 0);
	size_t RemoveJobObject(const size_t index, const bool allFollowing);
	size_t ClearJobObjects();

	bool IsJobObjectActive(const size_t index);
	void SetJobObjectActive(const size_t index, const bool active);
	void CancelCurrentJobObject();
}

#endif /* JNI_OBJECTMODEL_JOB_HPP_ */

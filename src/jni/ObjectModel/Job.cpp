/*
 * Job.cpp
 *
 *  Created on: 10 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Configuration.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/Vector.h"
#include "Hardware/Duet.h"
#include "Job.h"
#include "ListHelpers.h"

#define ATTR_SETTR_GETTR(funcName, type, varName)                                                                      \
    void Set##funcName(const type value)                                                                               \
    {                                                                                                                  \
        varName = value;                                                                                               \
    }                                                                                                                  \
    const type Get##funcName()                                                                                         \
    {                                                                                                                  \
        return varName;                                                                                                \
    }

namespace OM
{
	typedef Vector<JobObject*, MAX_TRACKED_OBJECTS> JobObjectList;
	static JobObjectList s_jobObjects;
	static int8_t s_currentJobObjectIndex = -1;

	static String<MAX_FILENAME_LENGTH> s_jobName;
	static uint32_t s_printTime = 0;
	static uint32_t s_printDuration = 0;
	static uint32_t s_printRemaining = 0;

	ATTR_SETTR_GETTR(PrintTime, uint32_t, s_printTime)
	ATTR_SETTR_GETTR(PrintDuration, uint32_t, s_printDuration)
	ATTR_SETTR_GETTR(PrintRemaining, uint32_t, s_printRemaining)

	void SetJobName(const char* name)
	{
		s_jobName.copy(name);
	}

	const StringRef GetJobName()
	{
		return s_jobName.GetRef();
	}

	void JobObject::Reset()
	{
		index = 0;
		cancelled = false;
		name.clear();
		bounds.x[0] = 0;
		bounds.x[1] = 0;
		bounds.y[0] = 0;
		bounds.y[1] = 0;
	}

	void SetCurrentJobObject(const int8_t index)
	{
		if (index >= (int)MAX_TRACKED_OBJECTS)
		{
			warn("JobObject index %d out of range\n", index);
			return;
		}
		verbose("Setting current job object index to %d", index);
		s_currentJobObjectIndex = index;
	}

	const int8_t GetCurrentJobObjectIndex()
	{
		return s_currentJobObjectIndex;
	}

	JobObject* GetJobObject(const size_t index)
	{
		if (index < 0 || index >= MAX_TRACKED_OBJECTS)
		{
			warn("JobObject index %d out of range\n", index);
			return nullptr;
		}
		return GetOrCreate<JobObjectList, JobObject>(s_jobObjects, index, false);
	}

	JobObject* GetOrCreateJobObject(const size_t index)
	{
		if (index < 0 || index >= MAX_TRACKED_OBJECTS)
		{
			warn("JobObject index %d out of range\n", index);
			return nullptr;
		}
		return GetOrCreate<JobObjectList, JobObject>(s_jobObjects, index, true);
	}

	size_t GetJobObjectCount()
	{
		return s_jobObjects.Size();
	}

	bool IterateJobObjectsWhile(function_ref<bool(JobObject*&, size_t)> func, const size_t startAt)
	{
		return s_jobObjects.IterateWhile(func, startAt);
	}

	size_t RemoveJobObject(const size_t index, const bool allFollowing)
	{
		return Remove<JobObjectList, JobObject>(s_jobObjects, index, allFollowing);
	}

	size_t ClearJobObjects()
	{
		size_t count = GetJobObjectCount();
		s_jobObjects.Clear();
		return count - GetJobObjectCount();
	}

	bool IsJobObjectActive(const size_t index)
	{
		JobObject* jobObject = GetJobObject(index);
		return jobObject != nullptr && !jobObject->cancelled;
	}

	void SetJobObjectActive(const size_t index, const bool active)
	{
		JobObject* jobObject = GetJobObject(index);
		if (jobObject == nullptr)
		{
			warn("Failed to get job object %d\n", index);
			return;
		};
		Comm::DUET.SendGcodef("M486 %c%d", active ? 'U' : 'P', index);
	}

	void CancelCurrentJobObject()
	{
		Comm::DUET.SendGcode("M486 C");
	}
} // namespace OM

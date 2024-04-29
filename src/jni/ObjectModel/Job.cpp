/*
 * Job.cpp
 *
 *  Created on: 10 Jan 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/Logic/ObjectCancel.h"

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

	static std::string s_jobName;
	static std::string s_lastJobName;
	static uint32_t s_printTime = 0;
	static uint32_t s_printDuration = 0;
	static struct
	{
		uint32_t filament = 0;
		uint32_t file = 0;
		uint32_t slicer = 0;
		uint32_t simulated = 0;
	} s_printRemaining;

	ATTR_SETTR_GETTR(PrintTime, uint32_t, s_printTime)
	ATTR_SETTR_GETTR(PrintDuration, uint32_t, s_printDuration)

	void SetPrintRemaining(RemainingTimeType type, const uint32_t printRemaining)
	{
		switch (type)
		{
		case RemainingTimeType::filament:
			s_printRemaining.filament = printRemaining;
			break;
		case RemainingTimeType::file:
			s_printRemaining.file = printRemaining;
			break;
		case RemainingTimeType::slicer:
			s_printRemaining.slicer = printRemaining;
			break;
		case RemainingTimeType::simulated:
			s_printRemaining.simulated = printRemaining;
			break;
		default:
			warn("Unknown RemainingTimeType %d\n", (int)type);
			break;
		}
	}

	const uint32_t GetPrintRemaining(RemainingTimeType type)
	{
		switch (type)
		{
		case RemainingTimeType::filament:
			return s_printRemaining.filament;
		case RemainingTimeType::file:
			return s_printRemaining.file;
		case RemainingTimeType::slicer:
			return s_printRemaining.slicer;
		case RemainingTimeType::simulated:
			return s_printRemaining.simulated;
		default:
			warn("Unknown RemainingTimeType %d\n", (int)type);
			return 0;
		}
	}

	void SetJobName(const char* name)
	{
		s_jobName = name;
	}

	const std::string& GetJobName()
	{
		return s_jobName;
	}

	void SetLastJobName(const char* name)
	{
		s_lastJobName = name;
	}

	const std::string& GetLastJobName()
	{
		return s_lastJobName;
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

	void SetCurrentJobObject(int8_t index)
	{
		if (index >= (int)MAX_TRACKED_OBJECTS)
		{
			warn("JobObject index %d out of range\n", index);
			index = -1;
		}
		verbose("Setting current job object index to %d", index);
		s_currentJobObjectIndex = index;
		UI::GetUIControl<ZKButton>(ID_MAIN_CancelCurrentObjectBtn)->setInvalid(s_currentJobObjectIndex < 0);
		UI::GetUIControl<ZKListView>(ID_MAIN_ObjectCancelObjectsList)->refreshListView();
		UI::ObjectCancel::RenderObjectMap();
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

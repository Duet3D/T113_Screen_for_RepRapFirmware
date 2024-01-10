/*
 * Job.cpp
 *
 *  Created on: 10 Jan 2024
 *      Author: Andy Everitt
 */

#include "Job.hpp"
#include "Duet3D/General/String.h"
#include "UI/UserInterfaceConstants.hpp"

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
    static String<MaxFileNameLength> jobName;
    static uint32_t printTime = 0;
    static uint32_t printDuration = 0;
    static uint32_t printRemaining = 0;

    ATTR_SETTR_GETTR(PrintTime, uint32_t, printTime)
    ATTR_SETTR_GETTR(PrintDuration, uint32_t, printDuration)
    ATTR_SETTR_GETTR(PrintRemaining, uint32_t, printRemaining)

    void SetJobName(const char* name)
    {
        jobName.copy(name);
    }

    const StringRef GetJobName()
    {
        return jobName.GetRef();
    }
} // namespace OM

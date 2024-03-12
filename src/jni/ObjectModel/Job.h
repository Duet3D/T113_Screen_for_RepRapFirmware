/*
 * Job.h
 *
 *  Created on: 10 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_OBJECTMODEL_JOB_HPP_
#define JNI_LOGIC_OBJECTMODEL_JOB_HPP_

#include <sys/types.h>
#include "Duet3D/General/StringRef.h"

namespace OM
{
	void SetJobName(const char* name);
	const StringRef GetJobName();

	void SetPrintTime(const uint32_t printTime);
	const uint32_t GetPrintTime();

	void SetPrintDuration(const uint32_t printDuration);
	const uint32_t GetPrintDuration();

	void SetPrintRemaining(const uint32_t printRemaining);
	const uint32_t GetPrintRemaining();
}



#endif /* JNI_LOGIC_OBJECTMODEL_JOB_HPP_ */

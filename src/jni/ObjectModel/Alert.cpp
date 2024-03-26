#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Alert.h"

namespace OM
{
	Alert currentAlert;
	uint32_t lastAlertSeq = 0;

	void Alert::Reset()
	{
		verbose("Resetting alert");
		mode = Mode::None;
		flags.Clear();
		for (size_t i = 0; i < ARRAY_SIZE(choices); i++)
		{
			choices[i].Clear();
		}
		limits.numberInt.min = 0;
		limits.numberInt.max = INT32_MAX;
		limits.numberFloat.min = 0;
		limits.numberFloat.max = FLT_MAX;
		limits.text.min = 1;
		limits.text.max = 10;
	}
} // namespace OM

/*
 * Colors.h
 *
 *  Created on: 8 Mar 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_UI_COLORS_H_
#define JNI_LOGIC_UI_COLORS_H_

#include <sys/types.h>

namespace UI
{
	enum Colors : uint32_t
	{
		Clear = 0x00000000,
		Black = 0xFF000000,
		White = 0xFFFFFFFF,
		LightGray = 0xFFE0E0E0,
		LighterGray = 0xFF808080,
		Gray = 0xFFC0C0C0,
		DarkGray = 0xFFA0A0A0,
		DarkerGray = 0xFF808080,
		Red = 0xFFFF0000,
		Blue = 0xFF0000FF,
		Button
	};
} // namespace UI

#endif /* JNI_LOGIC_UI_COLORS_H_ */

/*
 * Display.hpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */

#ifndef OBJ_LOGIC_DISPLAY_HPP_
#define OBJ_LOGIC_DISPLAY_HPP_

#include <sys/types.h>
#include <cmath>

constexpr float epsilon = 0.001f;

// Class to display an optional label, a floating point value, and an optional units string
class FloatField
{
	const char* label;
	const char* units;
	float val;
	uint8_t numDecimals;
public:
	FloatField(uint8_t pd, const char * pl = nullptr, const char * pu = nullptr) : label(pl), units(pu), val(0.0), numDecimals(pd)
	{
	}

	float GetValue() const noexcept { return val; }

	const char* GetLabel() const noexcept { return label; }

	void SetValue(float v)
	{
		if (fabsf(val - v) < epsilon)
		{
			return;
		}
		val = v;
	}

	void SetLabel(const char* s)
	{
		if (strcmp(label, s) == 0)
		{
			return;
		}
		label = s;
	}
};


#endif /* OBJ_LOGIC_DISPLAY_HPP_ */

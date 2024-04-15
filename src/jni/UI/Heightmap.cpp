/*
 * Heightmap.cpp
 *
 *  Created on: 15 Apr 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Heightmap.h"

#include "ObjectModel/Axis.h"
#include "Themes.h"
#include "UserInterface.h"
#include "control/ZKListView.h"
#include "control/ZKPainter.h"
#include "control/ZKTextView.h"

namespace UI
{
	static uint32_t GetColorForHeight(const OM::Heightmap& heightmap, double height)
	{
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();
		const double min = heightmap.GetMinError();
		const double max = heightmap.GetMaxError();
		const double range = max - min;
		const double percent = (height - min) / range;

		uint32_t minColor = theme->colors->heightmap.min;
		uint32_t maxColor = theme->colors->heightmap.max;

		// Calculate the color
		uint8_t r = (uint8_t)(minColor >> 16) + (uint8_t)((maxColor >> 16) - (minColor >> 16)) * percent;
		uint8_t g = (uint8_t)(minColor >> 8) + (uint8_t)((maxColor >> 8) - (minColor >> 8)) * percent;
		uint8_t b = (uint8_t)minColor + (uint8_t)(maxColor - minColor) * percent;

		uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
		dbg("Height %.3f, Min %.3f, Max %.3f, Range %.3f, Percent %.3f, Color %08X", min, max, range, percent, color);
		return color;
	}

	bool RenderHeightmap(const OM::Heightmap& heightmap)
	{
		// Render the heightmap
		static ZKPainter* canvas = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapPainter);
		static ZKPainter* scale = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapScale);
		static ZKListView* scaleText = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapScaleList);
		static ZKTextView* statsNumPoints = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsNumPoints);
		static ZKTextView* statsArea = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsArea);
		static ZKTextView* statsMax = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMax);
		static ZKTextView* statsMin = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMin);
		static ZKTextView* statsMean = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMean);
		static ZKTextView* statsRms = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsRMS);

		if (!canvas || !scale || !scaleText || !statsNumPoints || !statsArea || !statsMax || !statsMin || !statsMean ||
			!statsRms)
		{
			error("Failed to get UI controls");
			return false;
		}

		static LayoutPosition canvasPos = canvas->getPosition();
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();

		info("Rendering heightmap %s (%u, %u) using theme %s",
			 heightmap.GetFileName().c_str(),
			 heightmap.GetWidth(),
			 heightmap.GetHeight(),
			 theme->id.c_str());

		// Clear the canvas
		canvas->setSourceColor(theme->colors->heightmap.bgDefault);
		canvas->fillRect(0, 0, canvasPos.mWidth, canvasPos.mHeight, 0);

		// Get the printer limits
		const float axisMinX = heightmap.meta.GetAxis(0)->minPosition;
		const float axisMaxX = heightmap.meta.GetAxis(0)->maxPosition;
		const float axisMinY = heightmap.meta.GetAxis(1)->minPosition;
		const float axisMaxY = heightmap.meta.GetAxis(1)->maxPosition;

		// Render the heightmap
		for (size_t x = 0; x < heightmap.GetWidth(); x++)
		{
			for (size_t y = 0; y < heightmap.GetHeight(); y++)
			{
				const OM::Heightmap::Point* const point = heightmap.GetPoint(x, y);
				if (point == nullptr)
				{
					error("Failed to get point %u, %u", x, y);
					continue;
				}

				uint32_t color = GetColorForHeight(heightmap, point->z);
				canvas->setSourceColor(color);
				canvas->fillRect(x * 5, y * 5, 5, 5, 0);
			}
		}

		return true;
	}
} // namespace UI

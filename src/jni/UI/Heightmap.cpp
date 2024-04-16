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
#include "manager/LanguageManager.h"

namespace UI
{
	static OM::Heightmap s_currentHeightmap;
	static HeightmapRenderMode s_heightmapRenderMode = HeightmapRenderMode::Fixed;

	struct HeightmapRange
	{
		double min;
		double max;

		double operator()() const { return max - min; }
	};

	void SetHeightmapRenderMode(HeightmapRenderMode mode)
	{
		info("Setting heightmap render mode to %d", (int)mode);
		s_heightmapRenderMode = mode;
	}

	HeightmapRenderMode GetHeightmapRenderMode()
	{
		return s_heightmapRenderMode;
	}

	static HeightmapRange GetHeightmapRange(const OM::Heightmap& heightmap)
	{
		HeightmapRange range;
		switch (s_heightmapRenderMode)
		{
		case HeightmapRenderMode::Deviation:
			range.min = heightmap.GetMinError();
			range.max = heightmap.GetMaxError();
			break;
		case HeightmapRenderMode::Fixed:
		default:
			range.min = HEIGHTMAP_FIXED_MIN;
			range.max = HEIGHTMAP_FIXED_MAX;
			break;
		}
		return range;
	}

	double GetHeightmapScaleAt(int index)
	{
		static ZKListView* scaleText = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapScaleList);

		if (!scaleText)
		{
			error("Failed to get UI controls");
			return 0.0f;
		}
		HeightmapRange range = GetHeightmapRange(s_currentHeightmap);

		return range.min + (range() * (1.0 - (double)index / (scaleText->getRows() - 1)));
	}

	static uint32_t BlendColors(uint32_t color1, uint32_t color2, double ratio)
	{
		uint8_t r1 = (uint8_t)(color1 >> 16);
		uint8_t g1 = (uint8_t)(color1 >> 8);
		uint8_t b1 = (uint8_t)color1;

		uint8_t r2 = (uint8_t)(color2 >> 16);
		uint8_t g2 = (uint8_t)(color2 >> 8);
		uint8_t b2 = (uint8_t)color2;

		uint8_t r = (uint8_t)(r1 + (r2 - r1) * ratio);
		uint8_t g = (uint8_t)(g1 + (g2 - g1) * ratio);
		uint8_t b = (uint8_t)(b1 + (b2 - b1) * ratio);

		return (0xFF << 24) | (r << 16) | (g << 8) | b;
	}

	static uint32_t GetColorForHeight(const OM::Heightmap& heightmap, double height)
	{
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();

		HeightmapRange range = GetHeightmapRange(heightmap);

		const double percent = utils::bound<double>((height - range.min) / range(), 0.0f, 1.0f);

		uint32_t minColor = theme->colors->heightmap.min;
		uint32_t midColor = theme->colors->heightmap.mid;
		uint32_t maxColor = theme->colors->heightmap.max;

		uint32_t color;
		// Calculate the color
		if (percent >= 0.5f)
		{
			color = BlendColors(midColor, maxColor, (percent - 0.5f) * 2.0f);
		}
		else
		{
			color = BlendColors(minColor, midColor, percent * 2.0f);
		}

		dbg("Height %.3f, Min %.3f, Max %.3f, Range %.3f, Percent %.3f, Color %08X",
			height,
			range.min,
			range.max,
			range(),
			percent,
			color);
		return color;
	}

	static void RenderScale(const OM::Heightmap& heightmap)
	{
		static ZKPainter* scale = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapScale);
		static ZKListView* scaleText = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapScaleList);

		if (!scale || !scaleText)
		{
			error("Failed to get UI controls");
			return;
		}

		static LayoutPosition scalePos = scale->getPosition();
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();
		const HeightmapRange range = GetHeightmapRange(heightmap);

		info("Rendering scale %s using theme %s, range = %.3fmm -> %.3fmm",
			 heightmap.GetFileName().c_str(),
			 theme->id.c_str(),
			 range.min,
			 range.max);

		int pixelSize = scalePos.mHeight / 50;
		for (int pos = 0; pos < scalePos.mHeight; pos += pixelSize)
		{
			double value = range.min + (range() * (1.0 - (double)pos / scalePos.mHeight));
			uint32_t color = GetColorForHeight(heightmap, value);
			scale->setSourceColor(color);
			scale->fillRect(0, pos, scalePos.mWidth, pixelSize, 0);
		}

		scaleText->refreshListView();
	}

	static void RenderStatistics(const OM::Heightmap& heightmap)
	{
		static ZKTextView* statsNumPoints = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsNumPoints);
		static ZKTextView* statsArea = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsArea);
		static ZKTextView* statsMax = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMax);
		static ZKTextView* statsMin = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMin);
		static ZKTextView* statsMean = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMean);
		static ZKTextView* statsRms = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsRMS);

		if (!statsNumPoints || !statsArea || !statsMax || !statsMin || !statsMean || !statsRms)
		{
			error("Failed to get UI controls");
			return;
		}
	}

	bool RenderHeightmap(const OM::Heightmap& heightmap)
	{
		// Render the heightmap
		static ZKPainter* canvas = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapPainter);

		if (!canvas)
		{
			error("Failed to get UI controls");
			return false;
		}

		static LayoutPosition canvasPos = canvas->getPosition();
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();

		// Save the heightmap for scale text rendering
		s_currentHeightmap = heightmap;

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
				canvas->fillRect(x * 20, y * 20, 20, 20, 0);
			}
		}

		RenderScale(heightmap);

		return true;
	}
} // namespace UI

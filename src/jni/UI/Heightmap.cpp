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

#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "Storage.h"
#include "Themes.h"
#include "UserInterface.h"
#include "control/ZKListView.h"
#include "control/ZKPainter.h"
#include "control/ZKTextView.h"
#include "manager/LanguageManager.h"
#include "storage/StoragePreferences.h"
#include <cmath>

namespace UI
{
	static std::string s_currentHeightmap;
	static HeightmapRenderMode s_heightmapRenderMode = HeightmapRenderMode::Fixed;

	struct HeightmapRange
	{
		double min;
		double max;

		double operator()() const { return max - min; }
	};

	const std::string& GetVisibleHeightmapName()
	{
		return s_currentHeightmap;
	}

	void SetHeightmapRenderMode(HeightmapRenderMode mode)
	{
		info("Setting heightmap render mode to %d", (int)mode);
		s_heightmapRenderMode = mode;
		StoragePreferences::putInt(ID_HEIGHTMAP_RENDER_MODE, (int)mode);
		if (s_currentHeightmap.empty())
		{
			return;
		}
		RenderHeightmap(s_currentHeightmap);
	}

	HeightmapRenderMode GetHeightmapRenderMode()
	{
		return s_heightmapRenderMode;
	}

	std::string GetHeightmapRenderModeText(HeightmapRenderMode mode)
	{
		switch (mode)
		{
		case HeightmapRenderMode::Fixed:
			return LANGUAGEMANAGER->getValue("hm_render_mode_fixed");
		case HeightmapRenderMode::Deviation:
			return LANGUAGEMANAGER->getValue("hm_render_mode_deviation");
		default:
			return "Unknown";
		}
	}

	std::string GetHeightmapXAxisText(int index)
	{
		static ZKListView* axisList = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapXAxis);

		if (axisList == nullptr)
		{
			error("Failed to get axis list");
			return "";
		}

		OM::Heightmap heightmap = OM::GetHeightmapData(s_currentHeightmap.c_str());
		OM::Move::Axis* axis = heightmap.meta.GetAxis(0);
		if (axis == nullptr)
		{
			error("Failed to get axis");
			return "";
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		return utils::format("%.0f", min + (max - min) * (float)index / (axisList->getCols() - 1));
	}

	std::string GetHeightmapYAxisText(int index)
	{
		static ZKListView* axisList = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapYAxis);

		if (axisList == nullptr)
		{
			error("Failed to get axis list");
			return "";
		}

		OM::Heightmap heightmap = OM::GetHeightmapData(s_currentHeightmap.c_str());
		OM::Move::Axis* axis = heightmap.meta.GetAxis(1);
		if (axis == nullptr)
		{
			error("Failed to get axis");
			return "";
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		return utils::format("%.0f", max - (max - min) * (float)index / (axisList->getRows() - 1));
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

	std::string GetHeightmapScaleAt(int index)
	{
		static ZKListView* scaleText = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapScaleList);

		if (!scaleText)
		{
			error("Failed to get UI controls");
			return "";
		}
		HeightmapRange range = GetHeightmapRange(OM::GetHeightmapData(s_currentHeightmap.c_str()));

		return utils::format("%.2f mm", range.min + (range() * (1.0 - (double)index / (scaleText->getRows() - 1))));
	}

#if 0
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
#endif

	static uint32_t GetColorForPercent(double percent)
	{
		// Convert the height to a color on a HSV colorbar from blue to red
		double hue = (1.0 - percent) * 240.0; // Map the percent to the hue range (blue to red)
		double saturation = 1.0;			  // Set the saturation to maximum
		double value = 1.0;					  // Set the value to maximum

		// Convert HSV to RGB
		double c = value * saturation;
		double x = c * (1.0 - std::abs(std::fmod(hue / 60.0, 2.0) - 1.0));
		double m = value - c;

		double r, g, b;
		if (hue >= 0 && hue < 60)
		{
			r = c;
			g = x;
			b = 0;
		}
		else if (hue >= 60 && hue < 120)
		{
			r = x;
			g = c;
			b = 0;
		}
		else if (hue >= 120 && hue < 180)
		{
			r = 0;
			g = c;
			b = x;
		}
		else if (hue >= 180 && hue < 240)
		{
			r = 0;
			g = x;
			b = c;
		}
		else if (hue >= 240 && hue < 300)
		{
			r = x;
			g = 0;
			b = c;
		}
		else
		{
			r = c;
			g = 0;
			b = x;
		}

		// Scale the RGB values to the range 0-255
		uint8_t red = static_cast<uint8_t>((r + m) * 255);
		uint8_t green = static_cast<uint8_t>((g + m) * 255);
		uint8_t blue = static_cast<uint8_t>((b + m) * 255);

		// Combine the RGB values into a single color
		uint32_t color = (0xFF << 24) | (red << 16) | (green << 8) | blue;

		return color;
	}

	static uint32_t GetColorForHeight(const OM::Heightmap& heightmap, double height)
	{
		HeightmapRange range = GetHeightmapRange(heightmap);
		const double percent = utils::bound<double>((height - range.min) / range(), 0.0f, 1.0f);
		uint32_t color = GetColorForPercent(percent);

		verbose("Height %.3f, Min %.3f, Max %.3f, Range %.3f, Percent %.3f, Color %08X",
				height,
				range.min,
				range.max,
				range(),
				percent,
				color);
		return color;
	}

	void RenderScale()
	{
		static ZKPainter* scale = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapScale);
		static ZKListView* scaleText = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapScaleList);
		static bool rendered = false;

		scaleText->refreshListView();

		if (rendered)
		{
			return;
		}
		rendered = true;

		if (!scale || !scaleText)
		{
			error("Failed to get UI controls");
			return;
		}

		static LayoutPosition scalePos = scale->getPosition();

		int pixelSize = scalePos.mHeight / HEIGHTMAP_COLORBAR_SAMPLES;
		for (int pos = 0; pos < scalePos.mHeight; pos += pixelSize)
		{
			double percent = (1.0f - (double)pos / scalePos.mHeight);
			uint32_t color = GetColorForPercent(percent);
			scale->setSourceColor(color);
			scale->fillRect(0, pos, scalePos.mWidth, pixelSize, 0);
		}
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

		statsNumPoints->setTextTrf("hm_num_points", heightmap.GetPointCount());
		statsArea->setTextTrf("hm_area", heightmap.GetArea() / 100);
		statsMax->setTextTrf("hm_max", heightmap.GetMaxError());
		statsMin->setTextTrf("hm_min", heightmap.GetMinError());
		statsMean->setTextTrf("hm_mean", heightmap.GetMeanError());
		statsRms->setTextTrf("hm_rms", heightmap.GetStdDev());
	}

	bool RenderHeightmap(const std::string& heightmapName)
	{
		// Render the heightmap
		static ZKPainter* canvas = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapPainter);
		static ZKTextView* infoText = UI::GetUIControl<ZKTextView>(ID_MAIN_HeightMapInfoText);

		OM::Heightmap heightmap = OM::GetHeightmapData(heightmapName.c_str());
		RenderScale();
		RenderStatistics(heightmap);

		if (!canvas || !infoText)
		{
			error("Failed to get UI controls");
			return false;
		}

		if (Comm::duet.GetCommunicationType() != Comm::Duet::CommunicationType::network)
		{
			infoText->setTextTr("hm_not_supported_in_current_mode");
			infoText->setVisible(true);
			canvas->setVisible(false);
			return false;
		}
		infoText->setVisible(false);
		canvas->setVisible(true);

		static LayoutPosition canvasPos = canvas->getPosition();
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();

		// Clear the canvas
		ClearHeightmap();

		// Save the heightmap for scale text rendering
		s_currentHeightmap = heightmapName;
		if (heightmap.GetPointCount() <= 0)
		{
			error("Heightmap %s has no points", heightmapName.c_str());
			return false;
		}

		info("Rendering heightmap %s (%u, %u) using theme %s",
			 heightmap.GetFileName().c_str(),
			 heightmap.GetWidth(),
			 heightmap.GetHeight(),
			 theme->id.c_str());

		// Get the printer limits
		OM::Move::Axis* axisX = heightmap.meta.GetAxis(0);
		OM::Move::Axis* axisY = heightmap.meta.GetAxis(1);

		if (!axisX || !axisY)
		{
			error("Failed to get axes");
			return false;
		}

		const float axisMinX = axisX->minPosition;
		const float axisMaxX = axisX->maxPosition;
		const float axisMinY = axisY->minPosition;
		const float axisMaxY = axisY->maxPosition;
		double pixXSpacing = heightmap.meta.GetSpacing(0) * canvasPos.mWidth / (axisMaxX - axisMinX);
		double pixYSpacing = heightmap.meta.GetSpacing(1) * canvasPos.mHeight / (axisMaxY - axisMinY);

		canvas->setSourceColor(theme->colors->heightmap.gridColor);
		for (int x = 0; x < canvasPos.mWidth; x += pixXSpacing)
		{
			canvas->fillRect(x, 0, 1, canvasPos.mHeight, 0);
		}

		for (int y = 0; y < canvasPos.mHeight; y += pixYSpacing)
		{
			canvas->fillRect(0, y, canvasPos.mWidth, 1, 0);
		}

		// Render the heightmap
		for (size_t x = 0; x < heightmap.GetWidth(); x++)
		{
			for (size_t y = 0; y < heightmap.GetHeight(); y++)
			{
				const OM::Heightmap::Point* point = heightmap.GetPoint(x, y);
				if (point == nullptr)
				{
					error("Failed to get point %u, %u", x, y);
					continue;
				}
				if (point->isNull)
				{
					continue;
				}

				uint32_t color = GetColorForHeight(heightmap, point->z);
				canvas->setSourceColor(color);

				// Calculate the position of the point on the canvas
				double xPos = canvasPos.mWidth * (point->x - axisMinX) / (axisMaxX - axisMinX) - pixXSpacing / 2;
				double yPos =
					canvasPos.mHeight * (1.0f - (point->y - axisMinY) / (axisMaxY - axisMinY)) - pixYSpacing / 2;

				if (xPos < -pixXSpacing || yPos < -pixYSpacing)
				{
					warn("Point %u, %u out of bounds pos=(%.2f, %.2f)", x, y, xPos, yPos);
					continue;
				}

				xPos = utils::bound<double>(xPos, 0, (double)canvasPos.mWidth - pixXSpacing);
				yPos = utils::bound<double>(yPos, 0, (double)canvasPos.mHeight - pixYSpacing);
				verbose("Drawing point %u, %u at pixel (%d, %d) size=(%d, %d), point (%.2f, %.2f)",
						x,
						y,
						static_cast<int>(xPos),
						static_cast<int>(yPos),
						static_cast<int>(pixXSpacing),
						static_cast<int>(pixYSpacing),
						point->x,
						point->y);
				canvas->fillRect(static_cast<int>(xPos),
								 static_cast<int>(yPos),
								 static_cast<int>(pixXSpacing) + 1,
								 static_cast<int>(pixYSpacing) + 1,
								 0);
			}
		}
		return true;
	}

	void ClearHeightmap()
	{
		static ZKPainter* canvas = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapPainter);
		static LayoutPosition canvasPos = canvas->getPosition();
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();

		canvas->setSourceColor(theme->colors->heightmap.bgDefault);
		canvas->fillRect(0, 0, canvasPos.mWidth, canvasPos.mHeight, 0);
		s_currentHeightmap.clear();
	}
} // namespace UI

/*
 * Heightmap.cpp
 *
 *  Created on: 15 Apr 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "UI/UserInterface.h"

#include "Heightmap.h"

#include "Hardware/Duet.h"
#include "ObjectModel/Axis.h"
#include "Storage.h"
#include "UI/Themes.h"
#include "control/ZKListView.h"
#include "control/ZKPainter.h"
#include "control/ZKTextView.h"
#include "manager/LanguageManager.h"
#include "storage/StoragePreferences.h"
#include <cmath>

namespace UI::Heightmap
{
	static std::string s_currentHeightmap;
	static HeightmapRenderMode s_heightmapRenderMode = HeightmapRenderMode::Fixed;
	static ZKPainter* s_canvas;
	static ZKPainter* s_scale;
	static ZKListView* s_scaleText;
	static ZKTextView* s_statsNumPoints;
	static ZKTextView* s_statsArea;
	static ZKTextView* s_statsMax;
	static ZKTextView* s_statsMin;
	static ZKTextView* s_statsMean;
	static ZKTextView* s_statsRms;
	static ZKTextView* s_infoText;

	struct HeightmapRange
	{
		double min;
		double max;

		double operator()() const { return max - min; }
	};

	void Init()
	{
		info("Initialising heightmap UI");
		s_canvas = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapPainter);
		s_scale = UI::GetUIControl<ZKPainter>(ID_MAIN_HeightMapScale);
		s_scaleText = UI::GetUIControl<ZKListView>(ID_MAIN_HeightMapScaleList);
		s_statsNumPoints = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsNumPoints);
		s_statsArea = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsArea);
		s_statsMax = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMax);
		s_statsMin = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMin);
		s_statsMean = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsMean);
		s_statsRms = UI::GetUIControl<ZKTextView>(ID_MAIN_HMStatisticsRMS);
		s_infoText = UI::GetUIControl<ZKTextView>(ID_MAIN_HeightMapInfoText);

		RenderScale();
		SetHeightmapRenderMode(HeightmapRenderMode(StoragePreferences::getInt(ID_HEIGHTMAP_RENDER_MODE, 0)));
	}

	void Open()
	{
		OM::RequestHeightmapFiles();
		UI::WINDOW.OpenWindow(ID_MAIN_HeightMapWindow);
		RenderHeightmap(OM::GetCurrentHeightmap());
	}

	const std::string& GetVisibleHeightmapName()
	{
		return s_currentHeightmap;
	}

	size_t GetHeightmapCount()
	{
		return OM::GetHeightmapFiles().size();
	}

	void SetHeightmapListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		ZKListView::ZKListSubItem* pLoad = pListItem->findSubItemByID(ID_MAIN_HeightmapLoad);

		std::string filename = OM::GetHeightmapNameAt(index);

		bool shown = filename == GetVisibleHeightmapName().c_str();
		bool loaded = filename == OM::GetCurrentHeightmap().c_str();

		verbose("Heightmap %d: %s, shown: %d (%s), loaded: %d (%s)",
				index,
				filename.c_str(),
				shown,
				GetVisibleHeightmapName().c_str(),
				loaded,
				OM::GetCurrentHeightmap().c_str());

		pListItem->setText(filename);
		pListItem->setSelected(shown);
		pLoad->setTextTr(loaded ? "unload_heightmap" : "load_heightmap");
	}

	void HeightmapListItemCallback(const int index, const int id)
	{
		dbg("Selected heightmap %d", index);
		std::string filename = OM::GetHeightmapNameAt(index);
		dbg("heightmap = %s", filename.c_str());
		if (filename.empty())
		{
			return;
		}
		if (id == ID_MAIN_HeightmapLoad)
		{
			OM::ToggleHeightmap(filename.c_str());
		}
		RenderHeightmap(filename);
	}

	void Refresh()
	{
		OM::RequestHeightmapFiles();
		ClearHeightmap();
	}

	void SetHeightmapRenderMode(HeightmapRenderMode mode)
	{
		info("Setting heightmap render mode to %d", (int)mode);
		s_heightmapRenderMode = mode;
		StoragePreferences::putInt(ID_HEIGHTMAP_RENDER_MODE, (int)mode);
		if (s_currentHeightmap.empty())
		{
			ClearHeightmap();
			return;
		}
		RenderHeightmap(
			s_currentHeightmap.c_str()); // Can't pass s_currentHeightmap directly as it gets cleared in the function
	}

	HeightmapRenderMode GetHeightmapRenderMode()
	{
		return s_heightmapRenderMode;
	}

	void SetHeightmapRenderModeListItem(ZKListView::ZKListItem* pListItem, const int index)
	{
		HeightmapRenderMode mode = HeightmapRenderMode(index);
		switch (mode)
		{
		case HeightmapRenderMode::Fixed:
			pListItem->setTextTr("hm_render_mode_fixed");
			break;
		case HeightmapRenderMode::Deviation:
			pListItem->setTextTr("hm_render_mode_deviation");
			break;
		default:
			break;
		}
		pListItem->setSelected(mode == s_heightmapRenderMode);
	}

	void SetHeightmapXAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index)
	{
		OM::Heightmap heightmap = OM::GetHeightmapData(s_currentHeightmap.c_str());
		OM::Move::Axis* axis = heightmap.meta.GetAxis(0);
		if (axis == nullptr)
		{
			error("Failed to get axis");
			pListItem->setText("");
			return;
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		pListItem->setTextf("%.0f", min + (max - min) * (float)index / (pListView->getCols() - 1));
	}

	void SetHeightmapYAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index)
	{
		OM::Heightmap heightmap = OM::GetHeightmapData(s_currentHeightmap.c_str());
		OM::Move::Axis* axis = heightmap.meta.GetAxis(1);
		if (axis == nullptr)
		{
			error("Failed to get axis");
			pListItem->setText("");
			return;
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		return pListItem->setTextf("%.0f", max - (max - min) * (float)index / (pListView->getRows() - 1));
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

	void SetHeightmapScaleAt(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index)
	{
		HeightmapRange range = GetHeightmapRange(OM::GetHeightmapData(s_currentHeightmap.c_str()));

		return pListItem->setTextf("%.2f mm",
								   range.min + (range() * (1.0 - (double)index / (pListView->getRows() - 1))));
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
		static bool rendered = false;

		s_scaleText->refreshListView();

		if (rendered)
		{
			return;
		}
		rendered = true;

		if (!s_scale || !s_scaleText)
		{
			error("Failed to get UI controls");
			return;
		}

		static LayoutPosition scalePos = s_scale->getPosition();

		int pixelSize = scalePos.mHeight / HEIGHTMAP_COLORBAR_SAMPLES;
		for (int pos = 0; pos < scalePos.mHeight; pos += pixelSize)
		{
			double percent = (1.0f - (double)pos / scalePos.mHeight);
			uint32_t color = GetColorForPercent(percent);
			s_scale->setSourceColor(color);
			s_scale->fillRect(0, pos, scalePos.mWidth, pixelSize, 0);
		}
	}

	static void RenderStatistics(const OM::Heightmap& heightmap)
	{
		s_statsNumPoints->setTextTrf("hm_num_points", heightmap.GetPointCount());
		s_statsArea->setTextTrf("hm_area", heightmap.GetArea() / 100);
		s_statsMax->setTextTrf("hm_max", heightmap.GetMaxError());
		s_statsMin->setTextTrf("hm_min", heightmap.GetMinError());
		s_statsMean->setTextTrf("hm_mean", heightmap.GetMeanError());
		s_statsRms->setTextTrf("hm_rms", heightmap.GetStdDev());
	}

	bool RenderHeightmap(const std::string& heightmapName)
	{
		// Clear the canvas
		ClearHeightmap();

		// Render the heightmap
		OM::Heightmap heightmap = OM::GetHeightmapData(heightmapName.c_str());
		RenderScale();
		RenderStatistics(heightmap);

		if (Comm::DUET.GetCommunicationType() != Comm::Duet::CommunicationType::network)
		{
			s_infoText->setTextTr("hm_not_supported_in_current_mode");
			s_infoText->setVisible(true);
			s_canvas->setVisible(false);
			return false;
		}
		s_infoText->setVisible(false);
		s_canvas->setVisible(true);

		static LayoutPosition canvasPos = s_canvas->getPosition();
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();

		if (theme == nullptr)
		{
			error("Failed to get theme");
			return false;
		}

		// Save the heightmap for scale text rendering
		s_currentHeightmap = heightmapName.c_str();
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

		s_canvas->setSourceColor(theme->colors->heightmap.gridColor);
		for (int x = 0; x < canvasPos.mWidth; x += pixXSpacing)
		{
			s_canvas->fillRect(x, 0, 1, canvasPos.mHeight, 0);
		}

		for (int y = 0; y < canvasPos.mHeight; y += pixYSpacing)
		{
			s_canvas->fillRect(0, y, canvasPos.mWidth, 1, 0);
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
				s_canvas->setSourceColor(color);

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
				s_canvas->fillRect(static_cast<int>(xPos),
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
		dbg("Clearing Heightmap");

		if (!s_canvas)
		{
			warn("s_canvas not set");
			return;
		}
		static LayoutPosition canvasPos = s_canvas->getPosition();
		const UI::Theme::Theme* const theme = UI::Theme::GetCurrentTheme();
		if (!theme)
		{
			error("Failed to get theme");
			return;
		}

		s_canvas->setSourceColor(theme->colors->heightmap.bgDefault);
		s_canvas->fillRect(0, 0, canvasPos.mWidth, canvasPos.mHeight, 0);
		s_currentHeightmap.clear();
		RenderStatistics(OM::Heightmap());
	}
} // namespace UI

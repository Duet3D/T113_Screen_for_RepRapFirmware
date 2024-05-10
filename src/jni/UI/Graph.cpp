/*
 * Graph.cpp
 *
 *  Created on: 20 Mar 2024
 *      Author: Andy Everitt
 */

#include "Debug.h"

#include "Graph.h"
#include "Hardware/Duet.h"
#include "ObjectModel/Sensor.h"
#include "UI/Themes.h"
#include "uart/CommDef.h"
#include "utils/TimeHelper.h"

namespace UI
{
	Graph::Graph() : m_xRange(DEFAULT_TEMP_GRAPH_TIME_RANGE), m_yMax(DEFAULT_TEMP_GRAPH_MAX) {}

	void Graph::Init(ZKDiagram* diagram, ZKListView* xLabels, ZKListView* yLabels, ZKListView* legend)
	{
		info("Initialising Graph UI...");
		const Theme::Theme* theme = Theme::GetCurrentTheme();
		if (theme == nullptr)
		{
			warn("Failed to get current theme, not initialising graph");
		}

		m_diagram = diagram;
		m_xLabels = xLabels;
		m_yLabels = yLabels;
		m_legend = legend;

		for (size_t i = 0; i < MAX_SENSORS; i++)
		{
			// TODO each line takes ~245KB of memory, can this be made more efficient?
			// There does not appear to be a way to delete lines once they have been created
			m_diagram->addDiagramInfo(
				2,
				theme ? theme->colors->diagram.colors[i]
					  : 0xFFFFFFFF, // if theme is null, use white (can only happen if all theme file have been deleted)
				ZKDiagram::E_DIAGRAM_STYLE_CURVE,
				1.0,
				1.0,
				1.0,
				1,
				false);
			m_waveVisible[i] = true;
			m_waveCount++;
		}

		SetTimeRange(DEFAULT_TEMP_GRAPH_TIME_RANGE);
		ScaleYAxis(DEFAULT_TEMP_GRAPH_MAX);
	}

	void Graph::AddData(int index, float value)
	{

		long long now = TimeHelper::getCurrentTime();
		DataPoint dp = {now, value};
		verbose("Adding data point (%lld, %f) to sensor %d", dp.time, dp.value, index);
		m_data[index].Push(dp);

		if (m_diagram->isVisible())
			UpdateWave(index);
	}

	void Graph::RefreshLegend()
	{
		m_legend->refreshListView();
	}

	void Graph::ClearAll()
	{
		info("Clearing graph");
		for (size_t i = 0; i < m_waveCount; i++)
		{
			Clear(i);
		}
	}

	void Graph::Clear(int index)
	{
		if (index >= (int)m_waveCount || index >= (int)ARRAY_SIZE(m_data))
		{
			warn("index %d out of range", index);
			return;
		}
		verbose("Clearing graph data for sensor %d", index);
		m_data[index].Reset();
		m_diagram->clear(index);
	}

	void Graph::UpdateWave(int index)
	{
		if (index >= (int)ARRAY_SIZE(m_data))
		{
			warn("index %d out of range", index);
			return;
		}

		if (!m_waveVisible[index])
		{
			return;
		}

		long long now = TimeHelper::getCurrentTime();
		verbose("Updating wave for sensor %d, time=%lld", index, now);
		SZKPoint points[GRAPH_DATAPOINTS];

		float maxVal = DEFAULT_TEMP_GRAPH_MAX;
		verbose("m_data[%d].size=%d, head=%d, tail=%d",
				index,
				m_data[index].GetFilled(),
				m_data[index].GetHead(),
				m_data[index].GetTail());
		for (size_t i = 0; i < GRAPH_DATAPOINTS; i++)
		{

			if (m_data[index].Empty())
			{
				m_diagram->clear(index);
				return;
			}

			// points[i].x = i;
			// points[i].y = i % 100;
			if (i >= m_data[index].GetFilled())
			{
				points[i].x = points[i - 1].x;
				points[i].y = points[i - 1].y;
				continue;
			}

			DataPoint dp = m_data[index].GetItem(i);
			if (dp.value > maxVal)
			{
				maxVal = dp.value;
			}

			// If the X distance between 2 points is greater than ~900,000, zkgui will crash
			// This can happen if the system time is updated and there is already data in the graph
			if (dp.time < now - m_xRange * 1000 || dp.time > now)
			{
				// Remove old data
				verbose("Removing old data point (%lld, %f)", dp.time, dp.value);
				m_data[index].Pop();
				i--;
				continue;
			}

			points[i].x = static_cast<float>(dp.time - now) / 1000;
			points[i].y = (dp.value);
			verbose("points[%d] = (%f, %f)", i, points[i].x, points[i].y);
		}

		verbose("Setting data for index %d", index);
		ScaleYAxis(maxVal);
		m_diagram->setData(index, points, GRAPH_DATAPOINTS);
	}

	void Graph::SetTimeRange(int range)
	{
		if (range <= 0)
		{
			range = DEFAULT_TEMP_GRAPH_TIME_RANGE;
		}

		m_xRange = range;
		float scale = 100.0 / m_xRange;
		for (size_t i = 0; i < MAX_SENSORS; i++)
		{
			m_diagram->setXScale(i, scale);
		}
	}

	bool Graph::IsWaveVisible(int index)
	{
		if (index >= (int)m_waveCount)
		{
			return false;
		}

		return m_waveVisible[index];
	}

	void Graph::SetWaveVisible(int index, bool visible)
	{
		if (index >= (int)m_waveCount)
		{
			return;
		}

		m_waveVisible[index] = visible;
		if (!visible)
		{
			m_diagram->clear(index);
			return;
		}
		UpdateWave(index);
	}

	void Graph::ScaleYAxis(float max)
	{
		if (max <= 0)
		{
			max = DEFAULT_TEMP_GRAPH_MAX;
		}

#if 1
		// Scale the Y axis only when the max value is greater than the default
		if (max > DEFAULT_TEMP_GRAPH_MAX)
		{
			max += TEMP_GRAPH_Y_AXIS_PADDING;
		}

		m_yMax = max;
#else
		// Round up to the nearest 50
		m_yMax = ((((int)max - 1) / 50) + 1) * 50;
#endif
		float scale = 100.0 / m_yMax;
		verbose("Setting Y scale to %f", scale);
		for (size_t i = 0; i < MAX_SENSORS; i++)
		{
			m_diagram->setYScale(i, scale);
		}

		m_yLabels->refreshListView();
	}
} // namespace UI

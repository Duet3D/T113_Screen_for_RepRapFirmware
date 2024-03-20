/*
 * Graph.cpp
 *
 *  Created on: 20 Mar 2024
 *      Author: Andy Everitt
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "Graph.h"
#include "uart/CommDef.h"
#include "utils/TimeHelper.h"

namespace UI
{
	Graph TemperatureGraph;

	Graph::Graph() : m_xRange(DEFAULT_TEMP_GRAPH_TIME_RANGE), m_yMax(DEFAULT_TEMP_GRAPH_MAX) {}

	void Graph::Init(ZKDiagram* diagram, ZKListView* xLabels, ZKListView* yLabels, ZKListView* legend)
	{
		m_diagram = diagram;
		m_xLabels = xLabels;
		m_yLabels = yLabels;
		m_legend = legend;

		SetTimeRange(DEFAULT_TEMP_GRAPH_TIME_RANGE);
		ScaleYAxis(DEFAULT_TEMP_GRAPH_MAX);

		for (size_t i = 0; i < MAX_SENSORS; i++)
		{
			m_diagram->addDiagramInfo(2, 0xFFFFFFFF, ZKDiagram::E_DIAGRAM_STYLE_CURVE, 1.0, 1.0, 1.0, 1, false);
		}
	}

	void Graph::AddData(int index, float value)
	{

		long long now = TimeHelper::getCurrentTime();
		DataPoint dp = {now, value};
		m_data[index].Push(dp);

		UpdateDiagram(index);
	}

	void Graph::RefreshLegend()
	{
		m_legend->refreshListView();
	}

	void Graph::UpdateDiagram(int index)
	{
		if (index >= (int)ARRAY_SIZE(m_data))
		{
			warn("index %d out of range", index);
			return;
		}

		if (m_data[index].Empty())
		{
			m_diagram->clear(index);
		}

		long long now = TimeHelper::getCurrentTime();
		SZKPoint points[GRAPH_DATAPOINTS];

		float maxVal = DEFAULT_TEMP_GRAPH_MAX;
		dbg("m_data[%d].size = %d", index, m_data[index].GetFilled());
		for (size_t i = 0; i < GRAPH_DATAPOINTS; i++)
		{
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

			points[i].x = static_cast<float>(dp.time - now) / 1000;
			points[i].y = (dp.value);
			verbose("points[%d] = (%f, %f)", i, points[i].x, points[i].y);
		}

		dbg("Setting data for index %d", index);
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
		for (size_t i = 0; i < MAX_SENSORS; i++)
		{
			m_diagram->setYScale(i, scale);
		}

		m_yLabels->refreshListView();
	}

} // namespace UI

/*
 * Graph.cpp
 *
 *  Created on: 20 Mar 2024
 *      Author: Andy Everitt
 */

#include "Graph.h"

namespace UI
{
	Graph TemperatureGraph;

	Graph::Graph() {}

	void Graph::Init(ZKDiagram* diagram, ZKListView* xLabels, ZKListView* yLabels, ZKListView* legend)
	{
		m_diagram = diagram;
		m_xLabels = xLabels;
		m_yLabels = yLabels;
		m_legend = legend;

		for (size_t i = 0; i < MAX_SENSORS; i++)
		{
			m_diagram->addDiagramInfo(2, 0xFFFFFFFF, ZKDiagram::E_DIAGRAM_STYLE_CURVE, 1.0, 1.0, 10.0, 20, true);
		}
	}

} // namespace UI

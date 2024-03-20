/*
 * Graph.h
 *
 *  Created on: 20 Mar 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_GRAPH_H_
#define JNI_UI_GRAPH_H_

#include "control/ZKDiagram.h"
#include "control/ZKListView.h"
#include "control/ZKTextView.h"

#include "Configuration.h"
#include "Duet3D/General/CircularBuffer.h"

namespace UI
{
	class Graph
	{
	  public:
		Graph();
		void Init(ZKDiagram* diagram, ZKListView* xLabels, ZKListView* yLabels, ZKListView* legend);

		float GetYMax() const { return m_yMax; }
		void SetTimeRange(int range);
		int GetTimeRange() const { return m_xRange; }

		void RefreshLegend();

		void AddData(int index, float value);

	  private:
		struct DataPoint
		{
			long long time;
			float value;
		};

		void UpdateDiagram(int index);
		void ScaleYAxis(float max);

		ZKDiagram* m_diagram;
		ZKListView* m_xLabels;
		ZKListView* m_yLabels;
		ZKListView* m_legend;

		int m_xRange;
		float m_yMax;

		CircularBuffer<DataPoint, GRAPH_DATAPOINTS> m_data[MAX_SENSORS];
	};

	extern Graph TemperatureGraph;
} // namespace UI

#endif /* JNI_UI_GRAPH_H_ */

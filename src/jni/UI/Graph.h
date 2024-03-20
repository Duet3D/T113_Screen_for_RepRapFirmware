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

		void SetXRange(int min, int max);
		void SetXLabels(const char* labels[], size_t count);
		void SetYRange(int min, int max);
		void SetYLabels(const char* labels[], size_t count);

		void RefreshLegend();

		size_t GetLineCount() const;
		void AddLine();
		void AddData(int index, float value);

	  private:
		struct DataPoint
		{
			long long time;
			float value;
		};

		ZKDiagram* m_diagram;
		ZKListView* m_xLabels;
		ZKListView* m_yLabels;
		ZKListView* m_legend;

		CircularBuffer<DataPoint, 100> m_data[MAX_SENSORS];
	};

	extern Graph TemperatureGraph;
} // namespace UI

#endif /* JNI_UI_GRAPH_H_ */

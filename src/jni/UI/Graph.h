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

		size_t GetWaveCount() const { return m_waveCount; }
		float GetYMax() const { return m_yMax; }
		void SetTimeRange(int range);
		bool IsWaveVisible(int index);
		void SetWaveVisible(int index, bool visible);
		int GetTimeRange() const { return m_xRange; }
		void AddData(int index, float value);
		void RefreshLegend();
		void ClearAll();
		void Clear(int index);

	  private:
		struct DataPoint
		{
			long long time;
			float value;
		};

		void UpdateWave(int index);
		void ScaleYAxis(float max);

		ZKDiagram* m_diagram;
		ZKListView* m_xLabels;
		ZKListView* m_yLabels;
		ZKListView* m_legend;

		int m_xRange;
		float m_yMax;
		size_t m_waveCount = 0;
		bool m_waveVisible[MAX_SENSORS];

		CircularBuffer<DataPoint, GRAPH_DATAPOINTS> m_data[MAX_SENSORS];
	};
} // namespace UI

#endif /* JNI_UI_GRAPH_H_ */

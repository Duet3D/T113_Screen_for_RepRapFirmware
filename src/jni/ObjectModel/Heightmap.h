/*
 * Heightmap.h
 *
 *  Created on: 26 Mar 2024
 *      Author: andy
 */

#ifndef JNI_OBJECTMODEL_HEIGHTMAP_H_
#define JNI_OBJECTMODEL_HEIGHTMAP_H_

#include "Axis.h"
#include <string>
#include <vector>

namespace OM
{
	class HeightmapMeta
	{
	  public:
		HeightmapMeta();
		~HeightmapMeta();

		Move::Axis* GetAxis(size_t index);
		float GetMin(size_t index);
		float GetMax(size_t index);
		float GetSpacing(size_t index);
		float GetSamples(size_t index);
		float GetRadius();

	  private:
		char m_axis0[2];
		char m_axis1[2];
		float m_min[2];
		float m_max[2];
		float m_radius;
		float m_spacing[2];
		float m_samples[2];
	};

	class Heightmap
	{
	  public:
		Heightmap(const char* filename);

		HeightmapMeta meta;

	  private:
		std::string m_fileName;
		float m_minError;
		float m_maxError;
		float m_meanError;
		float m_stdDev;
		std::vector<std::vector<float>> m_heightmap;
	};
} // namespace OM

#endif /* JNI_OBJECTMODEL_HEIGHTMAP_H_ */

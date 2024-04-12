/*
 * Heightmap.h
 *
 *  Created on: 26 Mar 2024
 *      Author: andy
 */

#ifndef JNI_OBJECTMODEL_HEIGHTMAP_H_
#define JNI_OBJECTMODEL_HEIGHTMAP_H_

#include "Files.h"

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

		void Reset();
		void Parse(const std::string& meta);

		Move::Axis* GetAxis(size_t index) const;
		double GetMin(size_t index) const { return m_min[index]; }
		double GetMax(size_t index) const { return m_max[index]; }
		double GetSpacing(size_t index) const { return m_spacing[index]; }
		size_t GetSamples(size_t index) const { return m_samples[index]; }
		double GetRadius() const { return m_radius; }

	  private:
		std::string m_axis[2];
		double m_min[2];
		double m_max[2];
		double m_radius;
		double m_spacing[2];
		size_t m_samples[2];
	};

	class Heightmap
	{
	  public:
		Heightmap(const char* filename);

		struct Point
		{
			double x;
			double y;
			double z = 0.0f;
			bool isNull = false;
		};

		HeightmapMeta meta;

	  private:
		void ParseMeta(const std::string& csvContents);
		void ParseData(const std::string& csvContents);

		std::string m_fileName;
		double m_minError;
		double m_maxError;
		double m_meanError;
		double m_stdDev;
		std::vector<std::vector<Point>> m_heightmap;
	};

	void RequestHeightMapFiles();
	std::vector<FileSystem::FileSystemItem*> GetHeightMapFiles();
} // namespace OM

#endif /* JNI_OBJECTMODEL_HEIGHTMAP_H_ */

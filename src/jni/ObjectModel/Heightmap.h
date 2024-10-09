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
		Heightmap();
		Heightmap(const char* filename);

		struct Point
		{
			double x;
			double y;
			double z = 0.0f;
			bool isNull = false;
		};

		void Reset();

		bool LoadFromDuet(const char* filename);

		const std::string& GetFileName() const { return m_fileName; }
		size_t GetHeight() const { return m_heightmap.size(); }
		size_t GetWidth() const { return m_heightmap.empty() ? 0 : m_heightmap[0].size(); }
		const Point* GetPoint(size_t x, size_t y) const;
		size_t GetPointCount() const;
		double GetArea() const { return m_area; }
		double GetMinError() const { return m_minError; }
		double GetMaxError() const { return m_maxError; }
		double GetMeanError() const { return m_meanError; }
		double GetStdDev() const { return m_stdDev; }

		HeightmapMeta meta;

	  private:
		bool ParseMeta(const std::string& csvContents);
		bool ParseData(const std::string& csvContents);

		std::string m_fileName;
		double m_minError = 0.0f;
		double m_maxError = 0.0f;
		double m_meanError = 0.0f;
		double m_stdDev = 0.0f;
		double m_area = 0.0f;
		std::vector<std::vector<Point>> m_heightmap;
	};

	const std::string& GetHeightmapNameAt(int index);
	void SetCurrentHeightmap(const std::string& filename);
	void ClearCurrentHeightmap();
	const std::string& GetCurrentHeightmap();

	void LoadHeightmap(const char* filename);
	void UnloadHeightmap();
	void ToggleHeightmap(const char* filename);

	const Heightmap& GetHeightmapData(const char* filename);
	size_t ClearHeightmapCache();

	void RequestHeightmapFiles();
	std::vector<FileSystem::FileSystemItem*> GetHeightmapFiles();
} // namespace OM

#endif /* JNI_OBJECTMODEL_HEIGHTMAP_H_ */

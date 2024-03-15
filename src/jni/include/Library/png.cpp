#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "png.h"
#include <algorithm>
#include <stdio.h>

PNG::PNG() : m_imageFileName(nullptr), m_imageFile(nullptr) {}

PNG::PNG(const char* imageFileName) : m_imageFileName(imageFileName)
{
	Open();
}

PNG::~PNG()
{
	Close();
}

bool PNG::New(const char* imageFileName)
{
	Close();
	m_imageFileName = imageFileName;
	dataSize = 0;
	return Open();
}

bool PNG::Open()
{
	m_imageFile = fopen(m_imageFileName, "wb");
	if (m_imageFile == nullptr)
	{
		error("Failed to open file %s", m_imageFileName);
	}
	return m_imageFile != nullptr;
}

bool PNG::Close()
{
	if (m_imageFile == nullptr)
	{
		return true;
	}
	info("Closing file %s", m_imageFileName);
	if (!fclose(m_imageFile) == 0)
	{
		error("Failed to close file %s", m_imageFileName);
		return false;
	}
	m_imageFile = nullptr;
	return true;
}

size_t PNG::appendData(unsigned char data[], int size)
{
	if (!IsOpen())
	{
		warn("File %s not open", m_imageFileName);
		return 0;
	}
	dataSize += size;
	return fwrite(data, 1, size, m_imageFile);
}

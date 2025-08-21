#pragma once
#include <filesystem>

#include "Utils/Utils.h"

struct Texture2DParameters
{
	std::filesystem::path sourcePath;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t mipLevels = 1;
	bool generateMipMaps = false;
	bool sRGB = false; 
};

class Texture2D
{
public:
	static SharedPtr<Texture2D> Create(std::filesystem::path path);

	virtual void Bind(uint32_t binding = 0) = 0;
	virtual void SetData(void* data, uint32_t size) = 0;
};
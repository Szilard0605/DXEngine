#pragma once

#include "Texture2D.h"
#include "Utils/Utils.h"

#include <filesystem>
#include <stdint.h>

enum class TextureCubeFaceIndex : uint32_t
{
	Right = 0,
	Left  = 1,
	Top   = 2,
	Bottom= 3,
	Front = 4,
	Back  = 5
};

struct TextureCubeParameters
{
	std::filesystem::path faces[6];
	int widths[6];
	int heights[6];
	int colorChannels[6];
};

class TextureCube
{
public:
	static SharedPtr<TextureCube> Create(TextureCubeParameters parameters);

	virtual void Bind(uint32_t binding = 0) = 0;
	virtual void SetData(void* data, uint32_t size) = 0;
};

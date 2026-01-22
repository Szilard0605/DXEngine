#pragma once
#include <filesystem>

#include "Utils/Utils.h"

enum TextureWrapMode
{
	Repeat = 1,
	Mirror = 2,
	Clamp = 3,
	Border = 4,
	Mirror_Once = 5
};

enum TextureFormat
{
	RGBA8,
	RGBA16F,
	DEPTH24_STENCIL8
};

struct Texture2DProperties
{
	std::filesystem::path sourcePath;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t mipLevels = 1;
	bool generateMipMaps = false;
	bool sRGB = false;

	TextureWrapMode wrappingU = TextureWrapMode::Repeat;
	TextureWrapMode wrappingV = TextureWrapMode::Repeat;
	TextureWrapMode wrappingW = TextureWrapMode::Repeat;

	TextureFormat format = TextureFormat::RGBA8;
};

class Texture2D
{
public:
	static SharedPtr<Texture2D> Create(Texture2DProperties& properties);

	virtual void Bind(uint32_t binding = 0) = 0;
	virtual void SetData(void* data, uint32_t size) = 0;
	virtual Texture2DProperties GetProperties() = 0;

	virtual intptr_t GetRendererID() = 0;
};
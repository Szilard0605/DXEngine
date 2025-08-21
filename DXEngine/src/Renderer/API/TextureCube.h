#pragma once

#include "Texture2D.h"
#include "Utils/Utils.h"

#include <filesystem>
#include <stdint.h>

class TextureCube
{
public:
	static SharedPtr<TextureCube> Create(Texture2DParameters parameters);

	virtual void Bind(uint32_t binding = 0) = 0;
	virtual void SetData(void* data, uint32_t size) = 0;
};

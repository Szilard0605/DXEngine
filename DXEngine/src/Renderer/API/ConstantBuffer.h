#pragma once

#include <stdint.h>

#include "Utils/Utils.h"

class ConstantBuffer
{
public:

	enum class Type : uint8_t
	{
		Vertex = 0, 
		Pixel = 1
	};

	static SharedPtr<ConstantBuffer> Create(const void* data, uint32_t size, uint32_t slot, Type type = Type::Vertex);

	virtual void SetData(const void* data, uint32_t size) = 0;
	virtual void Bind() = 0;
};


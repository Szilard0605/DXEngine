#pragma once

#include <stdint.h>

#include "Utils/Utils.h"

class IndexBuffer
{
public:
	static SharedPtr<IndexBuffer> Create(uint32_t* data, uint32_t count);

	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual uint32_t GetCount() = 0;
};


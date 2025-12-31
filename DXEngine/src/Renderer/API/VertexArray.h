#pragma once


#include <stdint.h>
#include "VertexBuffer.h"
#include "Utils/Utils.h"

class VertexArray
{
	public:
		static SharedPtr<VertexArray> Create();

		virtual void AddVertexBuffer(SharedPtr<VertexBuffer> vbuffer) = 0;
		virtual void DrawIndexed(uint32_t count) = 0;
};
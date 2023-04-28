#pragma once

#include <stdint.h>
#include "VertexBuffer.h"

class VertexArray
{
	public:
		static VertexArray* Create();

		virtual void AddVertexBuffer(VertexBuffer* vbuffer) = 0;
		virtual void DrawIndexed(uint32_t count) = 0;
};
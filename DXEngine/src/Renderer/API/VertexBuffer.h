#pragma once

#include <stdint.h>

#include "Renderer/API/BufferLayout.h"

class VertexBuffer
{
	public:
		enum class BufferUsage
		{
			STATIC,
			DYNAMIC
		};

		static VertexBuffer* Create(BufferUsage usage);

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		virtual void SetLayout(BufferLayout layout) = 0;
		virtual void Resize(uint32_t size) = 0;
};
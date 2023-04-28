#include "IndexBuffer.h"

#include "Renderer.h"

#include "Renderer/Platform/D3D11/D3D11IndexBuffer.h"

IndexBuffer* IndexBuffer::Create(uint32_t* data, uint32_t count)
{
	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
			return new D3D11IndexBuffer(data, count);
		default:
			return nullptr;
	}
	return nullptr;
}
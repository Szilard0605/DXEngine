#include "VertexArray.h"

#include "Renderer.h"

#include "Renderer/Platform/D3D11/D3D11VertexArray.h"

VertexArray* VertexArray::Create()
{
	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
			return new D3D11VertexArray();
		default:
			return nullptr;
	}
	return nullptr;
}
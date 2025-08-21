#include "VertexArray.h"

#include "Renderer.h"

#include "Renderer/Backend/D3D11/D3D11VertexArray.h"

SharedPtr<VertexArray> VertexArray::Create()
{
	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
			return MakeShared<D3D11VertexArray>();
		default:
			return nullptr;
	}
	return nullptr;
}
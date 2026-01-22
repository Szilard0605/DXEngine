#include "Texture2D.h"
#include "Renderer.h"

#include "Renderer/Backend/D3D11/D3D11Texture2D.h"

SharedPtr<Texture2D> Texture2D::Create(Texture2DProperties& properties)
{
	switch (Renderer::GetAPI())
	{
	case Renderer::API::D3D11:
		return MakeShared<D3D11Texture2D>(properties);
	default:
		return nullptr;
	}
	return nullptr;
}

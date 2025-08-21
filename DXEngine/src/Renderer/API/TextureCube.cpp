#include "TextureCube.h"

#include "Renderer.h"

#include "Renderer/Backend/D3D11/D3D11TextureCube.h"	

SharedPtr<TextureCube> TextureCube::Create(Texture2DParameters parameters)
{
	switch (Renderer::GetAPI())
	{
	case Renderer::API::D3D11:
		return MakeShared<D3D11TextureCube>(parameters);
	default:
		return nullptr;
	}
	return nullptr;
}
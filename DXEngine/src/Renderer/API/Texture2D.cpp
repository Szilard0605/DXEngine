#include "Texture2D.h"
#include "Renderer.h"

#include "Renderer/Platform/D3D11/D3D11Texture2D.h"

Texture2D* Texture2D::Create(std::filesystem::path path)
{
	switch (Renderer::GetAPI())
	{
	case Renderer::API::D3D11:
		return new D3D11Texture2D(path);
	default:
		return nullptr;
	}
	return nullptr;
}

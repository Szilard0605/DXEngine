
#include "Renderer.h"

#include "Renderer/Platform/D3D11/D3D11Renderer.h"

Renderer::API Renderer::s_API = Renderer::API::D3D11;

Renderer* Renderer::Create(Window& window)
{
	switch (s_API)
	{
		case Renderer::API::D3D11: 
			return new D3D11Renderer(window);
		default:
			return nullptr;
	}
	return nullptr;
}
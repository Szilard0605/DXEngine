#include "RenderContext.h"

#include "Core/Window.h"

#include "Renderer/Platform/D3D11/D3D11Context.h"

RenderContext* RenderContext::Create(Window& window)
{
	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
			return new D3D11Context(window);
		default:
			return nullptr;
	}
	return nullptr;
}
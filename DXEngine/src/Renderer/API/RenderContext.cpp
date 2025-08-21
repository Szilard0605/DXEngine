#include "RenderContext.h"

#include "Core/Window.h"

#include "Renderer/Backend/D3D11/D3D11Context.h"

SharedPtr<RenderContext> RenderContext::Create(Window& window)
{
	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
			return MakeShared<D3D11Context>(window);
		default:
			return nullptr;
	}
	return nullptr;
}
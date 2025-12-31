#include "RenderTarget.h"

#include "Renderer.h"

#include "Renderer/Backend/D3D11/D3D11RenderTarget.h"

SharedPtr<RenderTarget> RenderTarget::Create(const RenderTargetDesc& desc)
{
	switch (Renderer::GetAPI())
	{
	case Renderer::API::D3D11:
		return MakeShared<D3D11RenderTarget>(desc); // D3D11RenderTarget needs to be implemented
	default:
		return nullptr;
	}
	return nullptr;
}

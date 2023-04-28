#include "D3D11Renderer.h"

D3D11Renderer::D3D11Renderer(Window& window)
{
	m_Context = new D3D11Context(window);
}

D3D11Renderer::~D3D11Renderer()
{
}

void D3D11Renderer::Clear(const glm::vec4& color)
{
	
	ID3D11DepthStencilView* dsv = m_Context->GetDepthStencilView();
	ID3D11RenderTargetView* rtv = m_Context->GetRenderTargetView();

	float fcolor[4] = {color.x, color.y, color.b, color.a};

	if(rtv)
		m_Context->GetDeviceContext()->ClearRenderTargetView(rtv, fcolor);
	if(dsv)
		m_Context->GetDeviceContext()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3D11Renderer::Resize(const float width, const float height)
{
}


void D3D11Renderer::Present()
{
	m_Context->Present();
}

void D3D11Renderer::UseDepthTesting(bool enable)
{
	// Probabaly needs rework, it recreates the depth stencil state
	// Maybe have to create different states and switch between them
	D3D11Context* context = D3D11Context::Get();
	if (!enable)
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = false;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		desc.StencilEnable = true;
		desc.StencilReadMask = 0xff;
		desc.StencilWriteMask = 0xff;

		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

		ID3D11DepthStencilState* state = context->GetDepthStencilState();
		HRESULT hr = context->GetDevice()->CreateDepthStencilState(&desc, &state);
		if (FAILED(hr))
		{
			printf("Couldn't create depth stencil state (enable DepthTesting)\n");
			return;
		}
	}
	else
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.StencilEnable = true;
		desc.StencilReadMask = 0xff;
		desc.StencilWriteMask = 0xff;

		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

		ID3D11DepthStencilState* state = context->GetDepthStencilState();
		HRESULT hr = context->GetDevice()->CreateDepthStencilState(&desc, &state);
		if (FAILED(hr))
		{
			printf("Couldn't create depth stencil state (disable DepthTesting)\n");
			return;
		}
	}
}

void D3D11Renderer::BindViewport()
{
	m_Context->BindRenderTargets();
}

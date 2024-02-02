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


void D3D11Renderer::BindViewport()
{
	m_Context->BindRenderTargets();
}

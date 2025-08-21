#include "D3D11Renderer.h"

#include "D3D11RenderTarget.h"

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

	m_Context->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
}


void D3D11Renderer::Present(SharedPtr<RenderTarget> renderTarget)
{
	if (renderTarget)
	{
		ID3D11Texture2D* backBuffer;
		HRESULT hr = m_Context->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

		if (FAILED(hr))
		{
			printf("Failed to get the back buffer from swapchain\n");
			return;
		}

		SharedPtr<D3D11RenderTarget> rt = std::dynamic_pointer_cast<D3D11RenderTarget>(renderTarget);
		ID3D11RenderTargetView* rtv = rt->GetRenderTargetView();

		hr = m_Context->GetDevice()->CreateRenderTargetView(backBuffer, NULL, &rtv);

		backBuffer->Release();
	}

	m_Context->Present();
}


void D3D11Renderer::BindViewport()
{
	m_Context->BindViewport();
}


void D3D11Renderer::BindBackBuffer()
{
	ID3D11RenderTargetView* rtv = m_Context->GetRenderTargetView();
	ID3D11DepthStencilView* dsv = m_Context->GetDepthStencilView();
	m_Context->GetDeviceContext()->OMSetRenderTargets(1, &rtv, dsv);
}
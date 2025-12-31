#include "D3D11RenderTarget.h"

#include "D3D11Context.h"

D3D11RenderTarget::D3D11RenderTarget(const RenderTargetDesc& desc)
	: m_Desc(desc)
{
	if(desc.Attachments.empty())
	{
		printf("D3D11RenderTarget: No RenderTarget attachments provided.\n");
		return;
	}

	for (auto attachment : desc.Attachments)
	{
		if (attachment == RenderTargetType::Color)
		{
			// Create color texture
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = desc.width;            // must be > 0
			textureDesc.Height = desc.height;          // must be > 0
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;

			HRESULT hr = D3D11Context::Get()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &m_ColorTexture);
			if (FAILED(hr))
			{
				printf("Failed to create color texture for render target.\n");
				return;
			}

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			hr = D3D11Context::Get()->GetDevice()->CreateRenderTargetView(m_ColorTexture, &rtvDesc, &m_RenderTargetView);
			if (FAILED(hr))
			{
				printf("Failed to create render target view.\n");
				return;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

			hr = D3D11Context::Get()->GetDevice()->CreateShaderResourceView(m_ColorTexture, &srvDesc, &m_ColorTextureSRV);

			if (FAILED(hr))
			{
				printf("Failed to create shader resource view for color texture. Error code: %l\n", hr);
			}



		}
		else if (attachment == RenderTargetType::Depth)
		{
			// Create depth texture
			D3D11_TEXTURE2D_DESC depthDesc = {};
			depthDesc.Width = desc.width;
			depthDesc.Height = desc.height;
			depthDesc.MipLevels = 1;
			depthDesc.ArraySize = 1;
			depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			depthDesc.SampleDesc.Count = 1;
			depthDesc.Usage = D3D11_USAGE_DEFAULT;
			depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;


			HRESULT hr = D3D11Context::Get()->GetDevice()->CreateTexture2D(&depthDesc, nullptr, &m_DepthTexture);
			if (FAILED(hr))
			{
				printf("Failed to create depth texture for render target.\n");
				return;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			hr = D3D11Context::Get()->GetDevice()->CreateDepthStencilView(m_DepthTexture, &dsvDesc, &m_DepthStencilView);
			if (FAILED(hr))
			{
				printf("Failed to create depth stencil view.\n");
				return;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			hr = D3D11Context::Get()->GetDevice()->CreateShaderResourceView(m_DepthTexture, &srvDesc, & m_DepthTextureSRV);


			if (FAILED(hr))
			{
				printf("Failed to create shader resource view for depth texture. Error code: %d\n", hr);
			}
		}

	}
}

D3D11RenderTarget::~D3D11RenderTarget()
{
}

void D3D11RenderTarget::Bind()
{
	// Bind the render target view
	ID3D11DeviceContext* context = D3D11Context::Get()->GetDeviceContext();
	D3D11Context::Get()->GetDeviceContext()->OMSetRenderTargets(m_RenderTargetView ? 1 : 0, &m_RenderTargetView, m_DepthStencilView);
}

void D3D11RenderTarget::Unbind()
{
	ID3D11DeviceContext* context = D3D11Context::Get()->GetDeviceContext();
	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, nullptr);
	
	if (m_DepthStencilView)
	{
		ID3D11DepthStencilView* nullDSV = nullptr;
		context->OMSetRenderTargets(0, nullptr, nullDSV);
	}
}

void D3D11RenderTarget::Resize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0)
	{
		printf("D3D11RenderTarget: Invalid dimensions for resizing render target.\n");
		return;
	}
	m_Desc.width = width;
	m_Desc.height = height;
	// Resize color texture
	if (m_ColorTexture)
	{
		m_ColorTexture->Release();
		m_ColorTexture = nullptr;
	}
	if (m_DepthTexture)
	{
		m_DepthTexture->Release();
		m_DepthTexture = nullptr;
	}
	// Recreate textures and views
	D3D11RenderTarget::D3D11RenderTarget(m_Desc);
}

void D3D11RenderTarget::Clear(float r, float g, float b, float a)
{
	ID3D11DeviceContext* context = D3D11Context::Get()->GetDeviceContext();
	if (m_RenderTargetView)
	{
		float clearColor[4] = { r, g, b, a };
		context->ClearRenderTargetView(m_RenderTargetView, clearColor);
	}
}

void D3D11RenderTarget::ClearDepth(float depth)
{
	ID3D11DeviceContext* context = D3D11Context::Get()->GetDeviceContext();
	if (m_DepthStencilView && m_Desc.UseDepthBuffer)
	{
		context->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void D3D11RenderTarget::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = static_cast<float>(x);
	viewport.TopLeftY = static_cast<float>(y);
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	ID3D11DeviceContext* context = D3D11Context::Get()->GetDeviceContext();
	context->RSSetViewports(1, &viewport);
}

/*void D3D11RenderTarget::RenderTargetDescToD3D11Desc(const RenderTargetDesc& desc, D3D11_TEXTURE2D_DESC& d3d11Desc)
{

}*/

#include "D3D11Context.h"

#include <d3dcompiler.h>

D3D11Context* D3D11Context::s_Context = nullptr;


//-----------------------
#define ReleaseCom(x)	\
	if (x)				\
	{					\
		x->Release();	\
		x = nullptr;	\
	}
//----------------------

D3D11Context::D3D11Context(Window& window)
	: m_Window(window)
{
	s_Context = this;

	uint32_t m_MSAAQuality;
	
	D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &m_Device, &FeatureLevelsRequested, &m_DeviceContext);
	m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_MSAAQuality);

	if (FAILED(hr))
	{
		printf("D3D11 Device creation failed\n");
		return;
	}

	DXGI_SWAP_CHAIN_DESC1 scDesc;
	ZeroMemory(&scDesc, sizeof(scDesc));
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Width = window.GetProperties().width;
	scDesc.Height = window.GetProperties().height;
	scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferCount = 3;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Scaling = DXGI_SCALING_NONE;

	IDXGIFactory2* dxgiFactory = 0;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	
	if (FAILED(hr))
	{
		printf("Couldn't create DXGIFactory\n");
		return;
	}
	
	hr = dxgiFactory->CreateSwapChainForHwnd(m_Device, window.GetWindowHandle(), &scDesc, NULL, NULL, &m_SwapChain);
	
	if (FAILED(hr))
	{
		printf("Couldn't create DXGIFactory\n");
		return;
	}

	dxgiFactory->Release();

	
	//Depth Stencil State 
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = true; // enable depth testing by default
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

		m_Device->CreateDepthStencilState(&desc, &m_DepthStencilState);
	}

	//m_Device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_DebugLayer));
	//m_DebugLayer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	// Rasterizer
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FrontCounterClockwise = true;

	m_Device->CreateRasterizerState(&rasterDesc, &m_RasterizerState);
	m_DeviceContext->RSSetState(m_RasterizerState);
	m_RasterizerState->Release();

	ID3D11Texture2D* backBuffer;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	if (FAILED(hr))
	{
		printf("Failed to get the back buffer from swapchain\n");
		return;
	}

	hr = m_Device->CreateRenderTargetView(backBuffer, NULL, &m_RenderTargetView);

	backBuffer->Release();

	if (FAILED(hr))
	{
		printf("Failed to create RenderTargetView\n");
		return;
	}
	
	Resize(window.GetProperties().width, window.GetProperties().height);
}

D3D11Context::~D3D11Context()
{
	ReleaseCom(m_Device);
	ReleaseCom(m_DeviceContext);
	ReleaseCom(m_RenderTargetView);
	ReleaseCom(m_SwapChain)
	ReleaseCom(m_RenderTargetView);
	ReleaseCom(m_DepthStencilBuffer);
	ReleaseCom(m_DepthStencilView);
	ReleaseCom(m_DepthStencilState);
	ReleaseCom(m_RasterizerState);
}

void D3D11Context::Resize(uint32_t width, uint32_t height)
{
	ReleaseCom(m_RenderTargetView);

	m_DeviceContext->Flush();

	HRESULT hr = m_SwapChain->ResizeBuffers(3, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	
	ID3D11Texture2D* backBuffer;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	if (FAILED(hr))
	{
		printf("Failed to get the back buffer from swapchain\n");
		return;
	}

	hr = m_Device->CreateRenderTargetView(backBuffer, NULL, &m_RenderTargetView);

	backBuffer->Release();


	if (FAILED(hr))
	{
		printf("Couldn't resize swapchain buffers\n");
		return;
	}

	m_ViewPort.TopLeftX = (m_Window.GetProperties().width - width);
	m_ViewPort.TopLeftY = (m_Window.GetProperties().height - height);


	printf("windowWidth: %d, windowHeight: %d\n", m_Window.GetProperties().width, m_Window.GetProperties().height);
	printf("tpx: %f\ntpy: %f\n", m_ViewPort.TopLeftX, m_ViewPort.TopLeftY);

	m_ViewPort.Width = static_cast<float>(width);
	m_ViewPort.Height = static_cast<float>(height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_DeviceContext->RSSetViewports(1, &m_ViewPort);

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	m_Device->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer);

	if (m_DepthStencilBuffer)
		hr = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView);

	BindRenderTargets();

	if (FAILED(hr))
	{
		printf("Failed to create depth stencil view\n");
		return;
	}

	ID3D11Resource* pResource = nullptr;
	m_RenderTargetView->GetResource(&pResource);

	D3D11_TEXTURE2D_DESC desc;
	ID3D11Texture2D* pTexture2D = nullptr;
	if (SUCCEEDED(pResource->QueryInterface(&pTexture2D)))
	{
		pTexture2D->GetDesc(&desc);
		pTexture2D->Release();
	}

	UINT rtvwidth = desc.Width;
	UINT rtvheight = desc.Height;

	pResource->Release();

	printf("w: %d\nh: %d\n", rtvwidth, rtvheight);
}

void D3D11Context::BindRenderTargets()
{
	m_DeviceContext->RSSetViewports(1, &m_ViewPort);
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, NULL);
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
}

void D3D11Context::Present()
{
	if (FAILED(m_SwapChain->Present(0, 0)))
	{
		printf("Couldn't present to D3D11 SwapChain\n");
	}
}

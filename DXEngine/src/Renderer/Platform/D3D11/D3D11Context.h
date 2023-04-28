#pragma once
#include <d3d11.h>

#include "Core/Window.h"
#include "Renderer/API/RenderContext.h"

#include <dxgi1_2.h>
 
class D3D11Context : public RenderContext
{
public:
	D3D11Context(Window& Window);
	~D3D11Context();

	virtual void Present() override;
	virtual void Resize(uint32_t width, uint32_t height) override;

	void BindRenderTargets();

	ID3D11Device* GetDevice() { return m_Device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContext; }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_RenderTargetView; }
	IDXGISwapChain* GetSwapChain() { return m_SwapChain; }

	ID3D11DepthStencilState* GetDepthStencilState() { return m_DepthStencilState; }
	ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView; }

	void ResizeSwapChain(float x, float y, float width, float height);
	D3D11_VIEWPORT* GetViewport() { return &m_ViewPort; }

	ID3D11RasterizerState* GetRasterizerState() { return m_RasterizerState; }

	static D3D11Context* Get() 
	{
		return s_Context;
	}

private:
	static D3D11Context* s_Context;

	Window m_Window;

	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;
	ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	IDXGISwapChain1* m_SwapChain = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView;
	ID3D11DepthStencilState* m_DepthStencilState;
	ID3D11Texture2D* m_DepthStencilBuffer;
	D3D11_VIEWPORT m_ViewPort;
	ID3D11RasterizerState* m_RasterizerState;

	ID3D11Debug* m_DebugLayer;
};


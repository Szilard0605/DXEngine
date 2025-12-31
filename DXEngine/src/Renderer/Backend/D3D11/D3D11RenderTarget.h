#pragma once


#include "Utils/Utils.h"
#include "Renderer/API/RenderTarget.h"

#include <d3d11.h>

class D3D11RenderTarget : public RenderTarget
{
public:
	D3D11RenderTarget(const RenderTargetDesc& desc);
	~D3D11RenderTarget();

	virtual void Bind() override;
	virtual void Unbind() override;
	virtual void Resize(uint32_t width, uint32_t height) override;
	virtual void Clear(float r, float g, float b, float a = 1.0f) override;
	virtual void ClearDepth(float depth = 1.0f) override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void SetData(void* data, uint32_t size) override {};

	ID3D11RenderTargetView* GetRenderTargetView() const { return m_RenderTargetView; }

private:
	RenderTargetDesc		  m_Desc = {};
	ID3D11Texture2D*	      m_ColorTexture = nullptr;
	ID3D11SamplerState*		  m_ColorSamplerState = nullptr;
	ID3D11ShaderResourceView* m_ColorTextureSRV = nullptr;

	ID3D11Texture2D*	      m_DepthTexture = nullptr;
	ID3D11SamplerState*		  m_DepthSamplerState = nullptr;
	ID3D11ShaderResourceView* m_DepthTextureSRV = nullptr;
	
	ID3D11RenderTargetView*		  m_RenderTargetView = nullptr;
	ID3D11DepthStencilView*		  m_DepthStencilView = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC m_RenderTargetViewDesc = {};
};


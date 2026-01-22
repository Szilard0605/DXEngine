#pragma once

#include "Renderer/API/Texture2D.h"

#include <d3d11.h>

class D3D11Texture2D : public Texture2D
{
public:
	D3D11Texture2D(Texture2DProperties& properties);

	virtual void Bind(uint32_t binding = 0) override;
	virtual void SetData(void* data, uint32_t size) override;
	virtual Texture2DProperties GetProperties() override { return m_Properties; }

	virtual intptr_t GetRendererID() override { return reinterpret_cast<intptr_t>(m_TextureSRV); }

	static D3D11_TEXTURE_ADDRESS_MODE GetD3D11WrapMode(TextureWrapMode mode);
	static DXGI_FORMAT GetDXGIFormat(TextureFormat format);
private:
	ID3D11Texture2D* m_Texture = nullptr;
	ID3D11ShaderResourceView* m_TextureSRV = nullptr;
	ID3D11SamplerState* m_SamplerState = nullptr;

	Texture2DProperties m_Properties;
};


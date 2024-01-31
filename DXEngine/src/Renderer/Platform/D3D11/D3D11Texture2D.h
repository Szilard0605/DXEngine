#pragma once

#include "Renderer/API/Texture2D.h"

#include <d3d11.h>

class D3D11Texture2D : public Texture2D
{
public:
	D3D11Texture2D(std::filesystem::path path);

	virtual void Bind(uint32_t binding = 0) override;
	virtual void SetData(void* data, uint32_t size) override;

private:
	ID3D11Texture2D* m_Texture = nullptr;
	ID3D11ShaderResourceView* m_TextureSRV = nullptr;
	ID3D11SamplerState* m_SamplerState = nullptr;
};


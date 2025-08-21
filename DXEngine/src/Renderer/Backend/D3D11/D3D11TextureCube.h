#pragma once

#include "Renderer/API/TextureCube.h"
#include <d3d11.h>

class D3D11TextureCube : public TextureCube
{
public:
	D3D11TextureCube(Texture2DParameters parameters);
	~D3D11TextureCube();
	virtual void Bind(uint32_t binding = 0) override;
	virtual void SetData(void* data, uint32_t size) override;
private:
	ID3D11Texture2D*		  m_Texture;
	ID3D11ShaderResourceView* m_TextureSRV;
	ID3D11SamplerState*	      m_SamplerState;
};


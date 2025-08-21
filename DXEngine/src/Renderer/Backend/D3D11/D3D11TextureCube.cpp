
#include "D3D11Context.h"
#include "D3D11TextureCube.h"

#include "stb_image.h"

D3D11TextureCube::D3D11TextureCube(Texture2DParameters parameters)
{
	int image_width, image_height, image_numComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(parameters.sourcePath.string().c_str(), &image_width, &image_height, &image_numComponents, 4);

	if (!data)
	{
		printf("[D3D11Texture2D] Couldn't load texture %s\n", parameters.sourcePath.string().c_str());
		return;
	}


	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = parameters.mipLevels;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.SampleDesc.Quality = 0;
	desc.ArraySize = 6;

	uint32_t stride = 4;// GetStrideFromFormat(m_Parameters.format);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = stride * image_width;
	initData.SysMemSlicePitch = 0;

	HRESULT result = D3D11Context::Get()->GetDevice()->CreateTexture2D(&desc, &initData, &m_Texture);

	if (FAILED(result))
	{
		printf("[D3D11Texture2D] error %d: Couldn't load texture %s.\n", result, parameters.sourcePath.string().c_str());
	}

	if (!m_Texture)
	{
		printf("[D3D11Texture2D] m_Texture is null\n");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Texture2D.MipLevels = parameters.mipLevels;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

	D3D11Context::Get()->GetDevice()->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureSRV);

	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11Context::Get()->GetDevice()->CreateSamplerState(&SamplerDesc, &m_SamplerState);
}

D3D11TextureCube::~D3D11TextureCube()
{
	if (m_Texture)
	{
		m_Texture->Release();
	}
	if (m_TextureSRV)
	{
		m_TextureSRV->Release();
	}
}

void D3D11TextureCube::Bind(uint32_t slot)
{
	if (m_TextureSRV)
	{
		ID3D11DeviceContext* deviceCtx = D3D11Context::Get()->GetDeviceContext();
		deviceCtx->PSSetShaderResources(slot, 1, &m_TextureSRV);
	}
}

void D3D11TextureCube::SetData(void* data, uint32_t size)
{
	if (m_Texture)
	{
		ID3D11DeviceContext* deviceCtx = D3D11Context::Get()->GetDeviceContext();
		deviceCtx->UpdateSubresource(m_Texture, 0, nullptr, data, size, 0);
	}
}

#include "D3D11Context.h"
#include "D3D11TextureCube.h"

#include "stb_image.h"

D3D11TextureCube::D3D11TextureCube(TextureCubeParameters parameters)
{
	// load faces into memory
	unsigned char* imageDatas[6];
	for (size_t i = 0; i < 6; i++)
	{
		stbi_set_flip_vertically_on_load(false);
		imageDatas[i] = stbi_load(parameters.faces[i].string().c_str(), &parameters.widths[i], &parameters.heights[i], &parameters.colorChannels[i], 4);
		
		if (!imageDatas[i])
		{
			printf("[D3D11TextureCube] Couldn't load image: %s\n", parameters.faces[i].string().c_str());
			return;
		}

		printf("[D3D11TextureCube] Loaded face %u: %s\n", i, parameters.faces[i].string().c_str());
	}

	

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = parameters.widths[0];
	desc.Height = parameters.heights[0];
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.CPUAccessFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	uint32_t stride = 4;// GetStrideFromFormat(m_Parameters.format);

	D3D11_SUBRESOURCE_DATA resourceDatas[6];
	
	for(int i = 0; i < 6; i++)
	{
		resourceDatas[i].pSysMem = imageDatas[i];
		resourceDatas[i].SysMemPitch = stride * parameters.widths[i];
		resourceDatas[i].SysMemSlicePitch = parameters.widths[i] * parameters.heights[i] * 4;
	}

	HRESULT result = D3D11Context::Get()->GetDevice()->CreateTexture2D(&desc, resourceDatas, &m_Texture);

	if (FAILED(result))
	{
		printf("[D3D11TextureCube] error %d: Couldn't load cube texture.\n", result);
		return;
	}

	if (!m_Texture)
	{
		printf("[D3D11TextureCube] m_Texture is null\n");
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

	D3D11Context::Get()->GetDevice()->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureSRV);

	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MinLOD = 0.0f;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 1;


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
		deviceCtx->PSSetSamplers(slot, 1, &m_SamplerState);
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
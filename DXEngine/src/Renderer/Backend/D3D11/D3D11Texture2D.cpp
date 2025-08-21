
#include "D3D11Texture2D.h"
#include "D3D11Context.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "D3D11TextureCube.h"

D3D11Texture2D::D3D11Texture2D(std::filesystem::path path)
{
	int image_width, image_height, image_numComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.string().c_str(), &image_width, &image_height, &image_numComponents, 4);
	
	if (!data)
	{
		printf("[D3D11Texture2D] Couldn't load texture %s\n", path.string().c_str());
		return;
	}


	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.SampleDesc.Quality = 0;
	desc.ArraySize = 1;

	uint32_t stride = 4;// GetStrideFromFormat(m_Parameters.format);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = stride * image_width;
	initData.SysMemSlicePitch = 0;

	HRESULT result = D3D11Context::Get()->GetDevice()->CreateTexture2D(&desc, &initData, &m_Texture);
	
	if (FAILED(result))
	{
		printf("[D3D11Texture2D] error %d: Couldn't load texture %s.\n", result, path.string().c_str());
	}

	if (!m_Texture)
	{
		printf("[D3D11Texture2D] m_Texture is null\n");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	D3D11Context::Get()->GetDevice()->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureSRV);

	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = 1;
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 0;
	HRESULT hr = D3D11Context::Get()->GetDevice()->CreateSamplerState(&SamplerDesc, &m_SamplerState);


	if (FAILED(hr))
	{
		printf("[D3D11Texture2D] Failed to create sampler state for texture %s.\n", path.string().c_str());
		// Find exact error code
		switch (hr)
		{
			case E_OUTOFMEMORY:
				printf("Out of memory.\n");
				break;
			case E_INVALIDARG:
				printf("Invalid argument.\n");
				break;
			default:
				printf("Unknown error.\n");
				break;
		}
	}
	
}

void D3D11Texture2D::Bind(uint32_t binding)
{
	D3D11Context::Get()->GetDeviceContext()->PSSetShaderResources(binding, 1, &m_TextureSRV);
	D3D11Context::Get()->GetDeviceContext()->PSSetSamplers(binding, 1, &m_SamplerState);
}

void D3D11Texture2D::SetData(void* data, uint32_t size)
{
	D3D11Context::Get()->GetDeviceContext()->UpdateSubresource(m_Texture, 0, nullptr, data, size, 0);
}

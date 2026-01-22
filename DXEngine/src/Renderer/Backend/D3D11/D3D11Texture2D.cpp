
#include "D3D11Texture2D.h"
#include "D3D11Context.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ENABLE_EXR
#include "stb_image.h"
#include "D3D11TextureCube.h"

D3D11Texture2D::D3D11Texture2D(Texture2DProperties& properties)
{
	int image_width, image_height, image_numComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(properties.sourcePath.string().c_str(), &image_width, &image_height, &image_numComponents, 4);

	m_Properties.width = image_width;
	m_Properties.height = image_height;
	m_Properties.mipLevels = 1;

	if (!data)
	{
		printf("[D3D11Texture2D] Couldn't load texture %s\n", m_Properties.sourcePath.string().c_str());
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
		printf("[D3D11Texture2D] error %d: Couldn't load texture %s.\n", result, m_Properties.sourcePath.string().c_str());
	}

	if (!m_Texture)
	{
		printf("[D3D11Texture2D] m_Texture is null\n");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = GetDXGIFormat(properties.format);
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	result = D3D11Context::Get()->GetDevice()->CreateShaderResourceView(m_Texture, &srvDesc, &m_TextureSRV);
	if (FAILED(result))
	{
		printf("[D3D11Texture2D] error %d: Couldn't create shader resource view for texture %s.\n", result, m_Properties.sourcePath.string().c_str());
	}

	D3D11_SAMPLER_DESC SamplerDesc;
	SamplerDesc.AddressU = GetD3D11WrapMode(properties.wrappingU);
	SamplerDesc.AddressV = GetD3D11WrapMode(properties.wrappingV);
	SamplerDesc.AddressW = GetD3D11WrapMode(properties.wrappingW);
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = 1;
	SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 16;
	HRESULT hr = D3D11Context::Get()->GetDevice()->CreateSamplerState(&SamplerDesc, &m_SamplerState);


	if (FAILED(hr))
	{
		printf("[D3D11Texture2D] Failed to create sampler state for texture %s.\n", m_Properties.sourcePath.string().c_str());
		printf("[D3D11Texture2D] Reason: ");
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

D3D11_TEXTURE_ADDRESS_MODE D3D11Texture2D::GetD3D11WrapMode(TextureWrapMode mode)
{
	switch (mode)
	{
	case Repeat:
		return D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case Mirror:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	case Clamp:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case Border:
		return D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	case Mirror_Once:
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		break;
	default:
		return D3D11_TEXTURE_ADDRESS_MODE();
		break;
	}

	return D3D11_TEXTURE_ADDRESS_MODE();
}

DXGI_FORMAT D3D11Texture2D::GetDXGIFormat(TextureFormat format)
{
	switch (format)
	{
	case RGBA8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case RGBA16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;
	case DEPTH24_STENCIL8:
		return DXGI_FORMAT_R24G8_TYPELESS;
		break;
	default:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}
	return DXGI_FORMAT_R8G8B8A8_UNORM;
}

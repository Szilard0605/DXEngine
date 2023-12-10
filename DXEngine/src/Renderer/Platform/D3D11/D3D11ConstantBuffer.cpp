#include "D3D11ConstantBuffer.h"

#include "D3D11Context.h"

D3D11ConstantBuffer::D3D11ConstantBuffer(const void* data, uint32_t size, uint32_t slot, ConstantBuffer::Type type)
	: m_Data(&data), m_Slot(slot), m_Type(type), m_Size(size)
{
	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = m_Size;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = data;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the buffer.
	HRESULT hr = D3D11Context::Get()->GetDevice()->CreateBuffer(&cbDesc, &InitData, &m_Buffer);

	if (FAILED(hr))
	{
		printf("Couldn't create constant buffer\n");
		return;
	}
}

D3D11ConstantBuffer::~D3D11ConstantBuffer()
{

}

void D3D11ConstantBuffer::SetData(const void* data, uint32_t size)
{
	m_Data = &data;
	m_Size = size;

	if (!data)
	{
		printf("ConstantBuffer: There is no data to write\n");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE msr;
	memset(&msr, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HRESULT result = D3D11Context::Get()->GetDeviceContext()->Map(m_Buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &msr);

	if (FAILED(result))
	{
		printf("Couldn't map VS constant buffer\n");
		return;
	}

	memcpy(msr.pData, data, m_Size);
	D3D11Context::Get()->GetDeviceContext()->Unmap(m_Buffer, NULL);
}

void D3D11ConstantBuffer::Bind()
{
	if (m_Type == ConstantBuffer::Type::Vertex)
		D3D11Context::Get()->GetDeviceContext()->VSSetConstantBuffers(m_Slot, 1, &m_Buffer);

	if (m_Type == ConstantBuffer::Type::Pixel)
		D3D11Context::Get()->GetDeviceContext()->PSSetConstantBuffers(m_Slot, 1, &m_Buffer);
}

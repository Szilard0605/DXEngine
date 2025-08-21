#include "D3D11IndexBuffer.h"

#include "D3D11Context.h"

D3D11IndexBuffer::D3D11IndexBuffer(uint32_t* data, uint32_t count)
{
	m_Count = count;

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * m_Count;  // size of the buffer in bytes
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;  // the buffer will not be updated
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;  // the buffer will be used as an index buffer
	indexBufferDesc.CPUAccessFlags = 0;  // the CPU will not access the buffer
	indexBufferDesc.MiscFlags = 0;  // no special options

	// Create the subresource data structure
	D3D11_SUBRESOURCE_DATA initialData = {};
	initialData.pSysMem = data;  // pointer to the index data
	
	D3D11Context* context = D3D11Context::Get();
	HRESULT result = context->GetDevice()->CreateBuffer(&indexBufferDesc, &initialData, &m_Buffer);
	
	if (FAILED(result))
	{
		printf("Couldn't create index buffer\n");
		return;
	}
}

D3D11IndexBuffer::~D3D11IndexBuffer()
{
}

void D3D11IndexBuffer::Bind()
{
	D3D11Context* context = D3D11Context::Get();
	context->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->GetDeviceContext()->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
}

void D3D11IndexBuffer::Unbind()
{
	m_Buffer = nullptr;

	D3D11Context* context = D3D11Context::Get();
	context->GetDeviceContext()->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
}

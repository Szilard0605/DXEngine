#include "D3D11VertexBuffer.h"

#include "D3D11Context.h"
#include "D3D11Shader.h"

#include <system_error>

D3D11VertexBuffer::D3D11VertexBuffer(BufferUsage bufferUsage)
{
	ZeroMemory(&m_BufferDesc, sizeof(m_BufferDesc));
	m_BufferDesc.Usage = D3D11BufferUsage(bufferUsage);
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
}

D3D11VertexBuffer::~D3D11VertexBuffer()
{

}

void D3D11VertexBuffer::Bind()
{
	uint32_t offset = 0;
	D3D11Context* context = D3D11Context::Get();
	uint32_t stride = m_BufferLayout.GetStride();
	context->GetDeviceContext()->IASetInputLayout(m_InputLayout);
	context->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_Buffer, &stride, &offset);
}

void D3D11VertexBuffer::Unbind()
{
	m_Buffer = nullptr;
	
	UINT stride = 0;
	UINT offset = 0;

	D3D11Context* context = D3D11Context::Get();
	context->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_Buffer, &stride, &offset);
}

void D3D11VertexBuffer::Resize(uint32_t size)
{
	m_Size = size;
	m_BufferDesc.ByteWidth = size;

	D3D11Context* context = D3D11Context::Get();
	context->GetDevice()->CreateBuffer(&m_BufferDesc, NULL, &m_Buffer);
}

void D3D11VertexBuffer::SetData(void* data, uint32_t size)
{
	if (m_Size < size)
		Resize(size);

	D3D11Context* context = D3D11Context::Get();
	context->GetDeviceContext()->Map(m_Buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &m_Map);
	memcpy(m_Map.pData, data, size);
	context->GetDeviceContext()->Unmap(m_Buffer, NULL);
}

DXGI_FORMAT ShaderDataTypeToD3D11(ShaderDataType type)
{
	switch (type)
		{
		case ShaderDataType::None: return DXGI_FORMAT_UNKNOWN;
		case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Mat3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Mat4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int: return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2: return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3: return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;
		case ShaderDataType::Bool: return DXGI_FORMAT_R32_UINT;
	}

	return DXGI_FORMAT_UNKNOWN;
}

void D3D11VertexBuffer::SetLayout(BufferLayout layout)
{
	m_BufferLayout = layout;


	const std::vector<BufferElement>& elements = layout.GetElements();
	size_t layoutSize = elements.size();

	D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC[layoutSize];

	for (uint32_t i = 0; i < layoutSize; i++)
	{
		desc[i] = { elements[i].Name.c_str(), 0, ShaderDataTypeToD3D11(elements[i].Type), 0, (UINT)elements[i].Offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	D3D11Context* context = D3D11Context::Get();
	SharedPtr<D3D11Shader> shader = D3D11Shader::GetCurrentlyBound();
	
	ID3D11Device* device = context->GetDevice();
	HRESULT result = device->CreateInputLayout(desc, layoutSize, shader->GetShaderData()->vsBlob->GetBufferPointer(), shader->GetShaderData()->vsBlob->GetBufferSize(), &m_InputLayout);
	if (FAILED(result))
	{
		printf("Failed to create VertexBuffer input layout\n");
		printf(std::system_category().message(result).c_str());
		return;
	}
}

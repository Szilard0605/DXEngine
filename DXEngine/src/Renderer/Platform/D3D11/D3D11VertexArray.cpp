#include "D3D11VertexArray.h"
#include "D3D11Context.h"

#include <d3d11.h>

D3D11VertexArray::D3D11VertexArray(SharedPtr<VertexBuffer> vbuffer)
{
	m_Buffers.push_back(vbuffer);
}

D3D11VertexArray::~D3D11VertexArray()
{
}

void D3D11VertexArray::AddVertexBuffer(SharedPtr<VertexBuffer> vbuffer)
{
	m_Buffers.push_back(vbuffer);
}

void D3D11VertexArray::DrawIndexed(uint32_t count)
{
	D3D11Context* ctx = D3D11Context::Get();

	for (uint32_t i = 0; i < m_Buffers.size(); i++)
	{
		D3D11Context::Get()->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D11Context::Get()->BindRenderTargets();

		m_Buffers[i]->Bind();
		D3D11Context::Get()->GetDeviceContext()->DrawIndexed(count, 0, 0);
	}
}

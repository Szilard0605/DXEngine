#pragma once

#include <d3d11.h>

#include "Renderer/API/BufferLayout.h"

#include "Renderer/API/VertexBuffer.h"



class D3D11VertexBuffer : public VertexBuffer
{
public:
	D3D11_USAGE D3D11BufferUsage(BufferUsage usage)
	{
		switch (usage)
		{
			case BufferUsage::STATIC:
				return D3D11_USAGE_IMMUTABLE;

			case BufferUsage::DYNAMIC:
				return D3D11_USAGE_DYNAMIC;

			default:
				return D3D11_USAGE_DEFAULT;
		}
	}

	D3D11VertexBuffer(BufferUsage bufferUsage = BufferUsage::DYNAMIC);
	~D3D11VertexBuffer();

	virtual void Bind() override;
	virtual void Unbind() override;
	 
	virtual void SetData(void* data, uint32_t size) override;
	virtual void SetLayout(BufferLayout layout) override;
	virtual void Resize(uint32_t size) override;

private:
	D3D11_BUFFER_DESC m_BufferDesc;
	ID3D11Buffer* m_Buffer = nullptr;
	D3D11_MAPPED_SUBRESOURCE m_Map;
	ID3D11InputLayout* m_InputLayout = nullptr;
	BufferLayout m_BufferLayout;

	uint32_t m_Size = 0;
};


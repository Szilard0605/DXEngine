#pragma once

#include <d3d11.h>

#include "D3D11VertexBuffer.h"

#include <vector>

#include "Renderer/API/VertexArray.h"

class D3D11VertexArray : public VertexArray
{
public:
	D3D11VertexArray() = default;
	D3D11VertexArray(VertexBuffer* vbuffer);
	~D3D11VertexArray();

	virtual void AddVertexBuffer(VertexBuffer* vbuffer) override;
	virtual void DrawIndexed(uint32_t count) override;

private:
	std::vector<VertexBuffer*> m_Buffers;
	
};


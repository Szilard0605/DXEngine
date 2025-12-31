#pragma once

#include <vector>

#include "Renderer/API/VertexArray.h"
#include "Utils/Utils.h"
class D3D11VertexArray : public VertexArray
{
public:
	D3D11VertexArray() = default;
	D3D11VertexArray(SharedPtr<VertexBuffer> vbuffer);
	~D3D11VertexArray();

	virtual void AddVertexBuffer(SharedPtr<VertexBuffer> vbuffer) override;
	virtual void DrawIndexed(uint32_t count) override;

private:
	std::vector<SharedPtr<VertexBuffer>> m_Buffers;
	
};


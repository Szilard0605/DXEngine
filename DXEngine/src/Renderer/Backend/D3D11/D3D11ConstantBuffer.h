#pragma once

#include <stdint.h>
#include <d3d11.h>

#include "Renderer/API/ConstantBuffer.h"

class D3D11ConstantBuffer : public ConstantBuffer
{
public:

	D3D11ConstantBuffer(const void* data, uint32_t size, uint32_t slot, ConstantBuffer::Type type = ConstantBuffer::Type::Vertex);
	~D3D11ConstantBuffer();

	virtual void SetData(const void* data, uint32_t size) override;
	virtual void Bind() override;
	

private:
	ID3D11Buffer* m_Buffer = nullptr;
	uint32_t m_Slot = 0;
	void* m_Data = nullptr;
	uint32_t m_Size = 0;
	ConstantBuffer::Type m_Type = ConstantBuffer::Type::Vertex;
};


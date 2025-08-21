#pragma once

#include <cinttypes>
#include <d3d11.h>

#include "Renderer/API/IndexBuffer.h"

class D3D11IndexBuffer : public IndexBuffer
{
public:
	D3D11IndexBuffer(uint32_t* data, uint32_t count);
	~D3D11IndexBuffer();

	virtual void Bind() override;
	virtual void Unbind() override;
	virtual uint32_t GetCount() override {return m_Count; }
private:
	ID3D11Buffer* m_Buffer = nullptr;
	uint32_t m_Count = 0;
};


#include "ConstantBuffer.h"

#include "Renderer.h"
#include "Renderer/Platform/D3D11/D3D11ConstantBuffer.h"

ConstantBuffer* ConstantBuffer::Create(const void* data, uint32_t size, uint32_t slot, Type type)
{
    switch (Renderer::GetAPI())
    {
        case Renderer::API::D3D11:
            return new D3D11ConstantBuffer(data, size, slot, type);
        default:
            return nullptr;
    }

    return nullptr;
}

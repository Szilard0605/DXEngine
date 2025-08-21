#include "ConstantBuffer.h"

#include "Renderer.h"
#include "Renderer/Backend/D3D11/D3D11ConstantBuffer.h"

SharedPtr<ConstantBuffer> ConstantBuffer::Create(const void* data, uint32_t size, uint32_t slot, Type type)
{
    switch (Renderer::GetAPI())
    {
        case Renderer::API::D3D11:
            return MakeShared<D3D11ConstantBuffer>(data, size, slot, type);
        default:
            return nullptr;
    }

    return nullptr;
}

#include "VertexBuffer.h"

#include "Renderer.h"
#include "Renderer/Platform/D3D11/D3D11VertexBuffer.h"

VertexBuffer* VertexBuffer::Create(BufferUsage usage)
{
    switch (Renderer::GetAPI())
    {
        case Renderer::API::D3D11:
            return new D3D11VertexBuffer(usage);
        default:
            return nullptr;
    }
    return nullptr;
}

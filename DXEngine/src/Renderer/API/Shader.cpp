#include "Shader.h"

#include "Renderer.h"

#include "Renderer/Platform/D3D11/D3D11Shader.h"

Shader* Shader::Create(const std::string& path)
{
	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
			return new D3D11Shader(path);
		default:
			return nullptr;
	}
	return nullptr;
}
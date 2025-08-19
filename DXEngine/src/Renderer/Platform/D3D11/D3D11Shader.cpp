#include "D3D11Shader.h"

#include <fstream>
#include <sstream>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <system_error>

SharedPtr<D3D11Shader> D3D11Shader::s_CurrentlyBoundShader = nullptr;

D3D11Shader::D3D11Shader(const std::string& path)
	: m_FilePath(path)
{
	m_Context = D3D11Context::Get();

	std::ifstream stream(path);
	if (!stream.is_open())
	{
		printf("Couldn't open shader file: %s\n", path.c_str());
		return;
	}

	std::string line;
	std::stringstream ss;

	while (getline(stream, line))
	{
		ss << line << "\n";
	}

	m_Data.vsBlob = Compile(ss.str(), "VS_Main", "vs_4_0");
	m_Data.psBlob = Compile(ss.str(), "PS_Main", "ps_4_0");

	if (!m_Data.vsBlob && !m_Data.psBlob)
	{
		printf("Couldn't compile shaders\n");
		return;
	}

	HRESULT result = m_Context->GetDevice()->CreateVertexShader(m_Data.vsBlob->GetBufferPointer(), m_Data.vsBlob->GetBufferSize(), NULL, &m_Data.m_VertexShader);
	if (FAILED(result))
	{
		printf("Couldn't create vertex shader");
		printf(std::system_category().message(result).c_str());
		return;
	}

	result = m_Context->GetDevice()->CreatePixelShader(m_Data.psBlob->GetBufferPointer(), m_Data.psBlob->GetBufferSize(), NULL, &m_Data.m_PixelShader);
	if (FAILED(result))
	{
		printf("Couldn't create pixel shader");
		printf(std::system_category().message(result).c_str());
		return;
	}
}

D3D11Shader::~D3D11Shader()
{
	m_Data.m_VertexShader->Release();
	m_Data.m_PixelShader->Release();
}

ID3DBlob* D3D11Shader::Compile(const std::string& source, const std::string& main, const std::string& target)
{
	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;
	HRESULT result = D3DCompile(source.c_str(), source.size(), NULL, NULL, NULL, main.c_str(), target.c_str(), D3DCOMPILE_DEBUG, 0, &shaderBlob, &errorBlob);
	if (FAILED(result))
	{
		if (errorBlob && errorBlob->GetBufferSize())
		{
			printf("Couldn't compile shader source:\n");
			printf((const char*)errorBlob->GetBufferPointer());
		}
		else
		{
			printf("Couldn't compile shader source.\n");
		}
		errorBlob->Release();
		return errorBlob;
	}
	return shaderBlob;

}

void D3D11Shader::Bind()
{
	s_CurrentlyBoundShader = shared_from_this();

	m_Context->GetDeviceContext()->VSSetShader(m_Data.m_VertexShader, NULL, 0);
	m_Context->GetDeviceContext()->PSSetShader(m_Data.m_PixelShader, NULL, 0);

	if (!m_ConstantBuffers.size())
		return;

	for (int i = 0; i < m_ConstantBuffers.size(); i++)
	{
		m_ConstantBuffers[i]->Bind();
	}
}

void D3D11Shader::Unbind()
{
	m_Context->GetDeviceContext()->VSSetShader(NULL, NULL, 0);
	m_Context->GetDeviceContext()->PSSetShader(NULL, NULL, 0);
}

void D3D11Shader::AddConstantBuffer(ConstantBuffer* buffer)
{
	m_ConstantBuffers.push_back(buffer);
}

#pragma once

#include "D3D11Context.h"

#include <d3d11.h>
#include <string>
#include <stdint.h>
#include <vector>

#include "Renderer/API/Shader.h"
#include "Renderer/API/ConstantBuffer.h"

class D3D11Shader : public Shader
{
public:
	D3D11Shader(const std::string& path);
	~D3D11Shader();
	
	ID3DBlob* Compile(const std::string& source, const std::string& main, const std::string& target);
	virtual void Bind() override;
	virtual void Unbind() override;
	
	static D3D11Shader* GetCurrentlyBound() { return s_CurrentlyBoundShader; }
	 
	virtual void AddConstantBuffer(ConstantBuffer* buffer) override;
	virtual std::vector<ConstantBuffer*> GetConstantBuffers() override { return m_ConstantBuffers; }

	struct ShaderData
	{
		ID3D11VertexShader* m_VertexShader;
		ID3D11PixelShader* m_PixelShader;

		ID3DBlob* vsBlob;
		ID3DBlob* psBlob;

		std::string m_VertexStr;
		std::string m_PixelStr;
	};


	ShaderData* GetShaderData() { return &m_Data; }

private:
	static D3D11Shader* s_CurrentlyBoundShader;

	std::vector<ConstantBuffer*> m_ConstantBuffers;

	D3D11Context* m_Context;
	std::string m_FilePath;
	ShaderData m_Data;
};


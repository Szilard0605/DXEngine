#pragma once

#include <string>
#include <vector>
#include <memory>

#include "ConstantBuffer.h"

class Shader
{
public:
	static std::shared_ptr<Shader> Create(const std::string& path);

	//ID3DBlob* Compile(const std::string& source, const std::string& main, const std::string& target);
	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual void AddConstantBuffer(ConstantBuffer* buffer) = 0;
	virtual std::vector<ConstantBuffer*> GetConstantBuffers() = 0;

};
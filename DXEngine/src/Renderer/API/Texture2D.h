#pragma once
#include <filesystem>

class Texture2D
{
public:
	static Texture2D* Create(std::filesystem::path path);

	virtual void Bind(uint32_t binding = 0) = 0;
	virtual void SetData(void* data, uint32_t size) = 0;
};
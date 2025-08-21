#pragma once

#include "Utils/Utils.h"
#include <stdint.h>
#include <vector>

enum class RenderTargetType
{
	None = 0,
	Color = 1, // Color buffer only
	Depth = 2 // Depth buffer only
};


struct RenderTargetDesc
{
	uint32_t width;
	uint32_t height;
	uint32_t samples = 1; // Default to no MSAA
	bool UseDepthBuffer = true; // Default to using a depth buffer

	std::vector<RenderTargetType> Attachments{};
};

class RenderTarget
{
public:
	static SharedPtr<RenderTarget> Create(const RenderTargetDesc& desc);

	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
	virtual void Clear(float r, float g, float b, float a = 1.0f) = 0;
	virtual void ClearDepth(float depth = 1.0f) = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void SetData(void* data, uint32_t size) = 0;


};


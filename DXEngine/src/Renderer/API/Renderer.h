#pragma once

#include "glm.hpp"

#include "Core/Window.h"

class Renderer
{
public:
	enum API
	{
		None, D3D11
	};

	static Renderer* Create(Window& window);

	virtual void Clear(const glm::vec4& color) = 0;
	virtual void Resize(const float width, const float height) = 0;
	virtual void Present() = 0;
	virtual void UseDepthTesting(bool enable) = 0;
	virtual void BindViewport() = 0;

	static API GetAPI() { return s_API; }
private:
	static API s_API;
};
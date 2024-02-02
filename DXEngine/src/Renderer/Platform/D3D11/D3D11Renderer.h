#pragma once

#include "Core/Window.h"

#include "D3D11Context.h"

#include "glm.hpp"

#include "Renderer/API/Renderer.h"

class D3D11Renderer : public Renderer
{
public:
	D3D11Renderer(Window& window);
	~D3D11Renderer();

	virtual void Clear(const glm::vec4& color) override;
	virtual void Resize(const float width, const float height) override;
	virtual void Present() override;
	virtual void BindViewport() override;

private:
	D3D11Context* m_Context;

};


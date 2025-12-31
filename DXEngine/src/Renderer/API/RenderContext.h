#pragma once

#include "Core/Window.h"

#include "Renderer.h"

#include "Utils/Utils.h"

class RenderContext
{
	public:
		static SharedPtr<RenderContext> Create(Window& window);

		virtual void Present() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
};


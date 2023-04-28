#pragma once


#include "Core/Window.h"

class ImGuiCore
{
public:
	static void Init(Window& window);
	static void Shutdown();
	static void NewFrame();
	static void EndFrame();

};


#pragma once

#include "Window.h"

#include <string>

#include "Renderer/Platform/D3D11/D3D11Context.h"
#include "Renderer/Platform/D3D11/D3D11Renderer.h"

#include "Renderer/API/API.h"


class Application
{
public:
	Application(const uint32_t width, const uint32_t height, const std::string title);
	~Application();

	void Run();

	static Application* GetInstance() { return s_Instance; }

	Renderer* GetRenderer() { return m_Renderer; }

	Window* GetWindow() { return m_Window; }
private:
	static Application* s_Instance;
	Window* m_Window;
	Renderer* m_Renderer;
};


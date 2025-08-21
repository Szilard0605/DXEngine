#pragma once

#include "Window.h"

#include <string>

#include "Renderer/Backend/D3D11/D3D11Context.h"
#include "Renderer/Backend/D3D11/D3D11Renderer.h"

#include "Renderer/API/API.h"

#include "Renderer/PBRRenderer.h"	
#include "Utils/Utils.h"

class Application : public std::enable_shared_from_this<Application>
{
public:

	Application(const uint32_t width, const uint32_t height, const std::string title);
	~Application();

	void Run();

	static Application* GetInstance() { return s_Instance; }
	SharedPtr<Renderer> GetRenderer() { return m_Renderer; }

	Window* GetWindow() { return m_Window; }


	void OnWindowResize(int width, int height);
private:
	static Application* s_Instance;
	Window* m_Window;
	SharedPtr<Renderer> m_Renderer;

	SharedPtr<PBRRenderer> m_PBRRenderer;
};


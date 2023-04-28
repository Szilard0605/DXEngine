#pragma once

#include <glfw3.h>
#include <string>
#include <windows.h>

struct WindowProperties
{
	uint32_t width;
	uint32_t height;
	std::string title;

	uint32_t TargetWidth;
	uint32_t TargetHeight;
	float TargetAspectRatio;
};


class Window
{
public:
	Window(WindowProperties properties);
	Window(const uint32_t width, const uint32_t height, const std::string& title);
	~Window();

	void Update();
	void Destroy();
	HWND GetWindowHandle();
	GLFWwindow* GetGLFWwindow() { return m_window; }
	bool ShouldClose();
	
	WindowProperties GetProperties() { return m_Properties; }

private:
	GLFWwindow* m_window;
	WindowProperties m_Properties;
};


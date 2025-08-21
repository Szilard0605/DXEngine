#pragma once

#include <glfw3.h>
#include <string>
#include <windows.h>

#include <functional>

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

	void SetResizeCallback(std::function<void(int, int)> callback) { m_ResizeCallback = callback; }
	
	WindowProperties GetProperties() { return m_Properties; }

private:
	std::function<void(int, int)> m_ResizeCallback;

	GLFWwindow* m_window;
	WindowProperties m_Properties;
};


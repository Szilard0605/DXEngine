#include "Window.h"
#include "Renderer/Platform/D3D11/D3D11Context.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>

Window::Window(WindowProperties properties)
{
	Window(properties.width, properties.height, properties.title);
}

Window::Window(const uint32_t width, const uint32_t height, const std::string& title)
{
	m_Properties.width = width;
	m_Properties.height = height;
	m_Properties.title = title;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


	m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	glfwSetWindowUserPointer(m_window, &m_Properties);

	if (!m_window)
	{
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return;
	}

	m_Properties.TargetAspectRatio = (float)m_Properties.width / (float)m_Properties.height;


	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
	{
			WindowProperties* nw = static_cast<WindowProperties*>(glfwGetWindowUserPointer(window));

			nw->width = width;
			nw->height = height;

			int aspect_width = width;
			int aspect_height = (int)((float)aspect_width / nw->TargetAspectRatio);
			if (aspect_height > height)
			{
				aspect_height = height;
				aspect_width = (int)((float)aspect_height * nw->TargetAspectRatio);
			}

			int vpX = (int)((float)width / 2.0f) - (int)((float)aspect_width / 2.0f);
			int vpY = (int)((float)height / 2.0f) - (int)((float)aspect_height / 2.0f);

			D3D11Context::Get()->Resize((uint32_t)aspect_width , (uint32_t)aspect_height);
		});
}

Window::~Window()
{
	glfwDestroyWindow(m_window);
}

void Window::Update()
{
	glfwPollEvents();
	//glfwSwapBuffers(m_window);
}

void Window::Destroy()
{
	glfwDestroyWindow(m_window);
}

HWND Window::GetWindowHandle()
{
	return glfwGetWin32Window(m_window);
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(m_window);
}

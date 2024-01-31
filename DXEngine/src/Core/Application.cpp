#include "Application.h"

#include <glfw3.h>

#include "Renderer/Platform/D3D11/D3D11Shader.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/Camera/PerspectiveCamera.h"

#include "Renderer/Debug/ImGui/ImGuiCore.h"

#include "imgui.h"
#include "gtc/type_ptr.hpp"
#include <Renderer/Platform/D3D11/D3D11Texture2D.h>


Application* Application::s_Instance = nullptr;

Application::Application(const uint32_t width, const uint32_t height, const std::string title)
{
	s_Instance = this;

	if (!glfwInit())
	{
		printf("Couldn't initialize GLFW\n");
		glfwTerminate();
		return;
	}

	m_Window = new Window(width, height, title);
	m_Renderer = Renderer::Create(*m_Window);
	Renderer2D::Init(m_Renderer);
	ImGuiCore::Init(*m_Window);
}

Application::~Application()
{
	ImGuiCore::Shutdown();
	m_Window->Destroy();
}

void Application::Run()
{
	
	PerspectiveCamera camera(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	//camera.SetFOV(60.f);
	camera.Translate({0, 0, 0});
	camera.UpdateView();

	glm::vec3 camPos = {0, 0, 5};
	float fov = 60.0f;
	float rot = 0.0f;
	glm::vec3 quadPos = {0, 0, 0};
	glm::vec4 color = glm::vec4(1);

	Texture2D* tex = Texture2D::Create("res/textures/test.jpg");

	while (!m_Window->ShouldClose())
	{
		m_Window->Update();

		Renderer2D::DrawQuad(quadPos, { 1.0f, 1.0f, 1.0f }, rot, color, tex, camera);

		//Renderer2D::DrawQuad({0, 0, 0.0f}, { 1.0f, 1.0f, 1.0f }, rot, color, camera);

		{
			ImGuiCore::NewFrame();
			ImGui::Begin("Teszt");

			ImGui::DragFloat3("camPos", glm::value_ptr(camPos));
			ImGui::DragFloat("FOV", &fov);
			ImGui::DragFloat3("quadPos", glm::value_ptr(quadPos));	
			ImGui::DragFloat("rotation", &rot);
			ImGui::ColorEdit4("quadCol", glm::value_ptr(color));

			ImGui::End();
			ImGuiCore::EndFrame();
		}

		camera.Translate(camPos);
		camera.SetFOV(fov);

		m_Renderer->Present();

		m_Renderer->BindViewport();
		
		m_Renderer->Clear(glm::vec4(0.4f, 0.0f, 0.0f, 1.0f));
		


	}
}

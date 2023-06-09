#include "Application.h"

#include <glfw3.h>

#include "Renderer/Platform/D3D11/D3D11Shader.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/Camera/PerspectiveCamera.h"

#include "Renderer/Debug/ImGui/ImGuiCore.h"

#include "imgui.h"

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
	Shader* shader = Shader::Create("res/shaders/testshader.hlsl");

	PerspectiveCamera camera(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	camera.Translate({0, 0, 0});
	camera.UpdateView();

	struct CamData
	{
		glm::mat4 ViewProjectionMatrix;
	} CamData;

	ConstantBuffer* cambuffer = ConstantBuffer::Create(&CamData, sizeof(CamData), 0);
	shader->AddConstantBuffer(cambuffer);


	glm::vec3 camPos = {0, 0, 0};
	float fov = 0.0f;
	float rot = 0.0f;
	glm::vec3 quadPos = {0, 0, 0};

	while (!m_Window->ShouldClose())
	{
		m_Window->Update();

		camera.Translate(glm::vec3(5000, 0, 0));

		//camera.Translate({ 0.0f, 0.0f, rot });

		Renderer2D::DrawQuad(quadPos, { 1.0f, 1.0f, 1.0f }, rot, {0.0f, 0.0f, 1.0f, 1.0f}, shader, camera);

		{
			ImGuiCore::NewFrame();
			ImGui::Begin("Teszt");

			float fcam[3] = { camPos.x, camPos.y, camPos.z };
			ImGui::DragFloat3("camPos", fcam);
			camPos = { fcam[0], fcam[1], fcam[2] };

			ImGui::DragFloat("FOV", &fov);


			float pos[3] = { quadPos.x, quadPos.y, quadPos.z };
			ImGui::DragFloat3("quadPos", pos);
			quadPos = { pos[0], pos[1], pos[2] };

			ImGui::DragFloat("rotation", &rot);

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

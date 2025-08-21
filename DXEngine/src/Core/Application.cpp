#include "Application.h"

#include <glfw3.h>

#include "Renderer/Backend/D3D11/D3D11Shader.h"
#include "Renderer/Backend/D3D11/D3D11Texture2D.h"

#include "Renderer/Camera/PerspectiveCamera.h"

#include "Renderer/Debug/ImGui/ImGuiCore.h"

#include "imgui.h"
#include "gtc/type_ptr.hpp"
#include <Renderer/Mesh.h>

#include "Renderer/PBRRenderer.h"

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

	WindowProperties properties;
	properties.title = title;
	properties.width = width;
	properties.height = height;

	m_Window = new Window(properties);
	m_Renderer = Renderer::Create(*m_Window);
	m_PBRRenderer = MakeShared<PBRRenderer>(width, height);
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

	glm::vec3 camPos = {0, 107, 215};
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	float fov = 60.0f;
	glm::vec3 rot = glm::vec3( 0.0f );
	float rotSpeed = 0.0001f;
	glm::vec4 color = glm::vec4(1);

	glm::vec3 bgColor = glm::vec3(1.0f);

	Math::Transform meshTransform;

	std::vector<SharedPtr<Mesh>> meshes = Mesh::ImportDynamicMesh("res/models//medieval_civilian_3/scene.gltf");

	for (int i = 0; i < meshes.size(); i++)
	{	
		m_PBRRenderer->AddMesh(meshes[i]);
		meshes[i]->SetTransform(meshTransform);
	}

	while (!m_Window->ShouldClose())
	{
		std::for_each(meshes.begin(), meshes.end(), [meshTransform](SharedPtr<Mesh> mesh) { mesh->SetTransform(meshTransform); });

		m_Window->Update();

		m_PBRRenderer->Render(camera);

		meshTransform.Rotation.y += rotSpeed;

		ImGuiCore::NewFrame();
		ImGui::Begin("Teszt");

		ImGui::DragFloat3("camPos", glm::value_ptr(camPos), 0.1f);
		ImGui::DragFloat("FOV", &fov, 1.0f);
		ImGui::DragFloat3("Mesh Translation", glm::value_ptr(meshTransform.Position), 0.1f);
		ImGui::DragFloat3("Mesh Scale", glm::value_ptr(meshTransform.Scale), 0.1f);
		ImGui::DragFloat3("Mesh Rotation", glm::value_ptr(meshTransform.Rotation), 0.1f);
		ImGui::ColorEdit4("quadCol", glm::value_ptr(color), 0.1f);
		ImGui::DragFloat("Mesh Rotation Speed", &rotSpeed, 0.0001f);

		ImGui::DragFloat("Cam NearClip", &nearPlane, 0.1f);
		ImGui::DragFloat("Cam FarClip", &farPlane, 0.1f);

		ImGui::ColorEdit3("Background Color", glm::value_ptr(bgColor), 0.1f);

		ImGui::End();
		ImGuiCore::EndFrame();


		m_Renderer->Present(); // for imgui

		Application::GetInstance()->GetRenderer()->Clear(glm::vec4(bgColor, 1.0f));

		camera.SetNearClip(nearPlane);
		camera.SetFarClip(farPlane);
		camera.Translate(camPos);
		camera.SetFOV(fov);		
	}
}

void Application::OnWindowResize(int width, int height)
{
	m_PBRRenderer->Resize(width, height);
}

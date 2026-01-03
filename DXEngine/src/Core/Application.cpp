#include "Application.h"

#include <glfw3.h>

#include "Renderer/Camera/PerspectiveCamera.h"

#include "Renderer/Debug/ImGui/ImGuiCore.h"

#include "imgui.h"
#include "gtc/type_ptr.hpp"
#include <Renderer/Mesh.h>

#include "Renderer/PBRRenderer.h"

#include "Renderer/MeshImporter.h"
#include "Renderer/API/TextureCube.h"

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
	
	m_camera = PerspectiveCamera(60.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);

	std::vector<SharedPtr<Mesh>> meshes = MeshImporter::ImportDynamicMesh("res/models//medieval_civilian_3/scene.gltf");

	for (int i = 0; i < meshes.size(); i++)
	{	
		m_PBRRenderer->AddMesh(meshes[i]);
		meshes[i]->SetTransform(m_MeshTransform);
	}

	SharedPtr<Texture2D> tex = Texture2D::Create("res/textures/horn-koppe_spring_4k.hdr");

	TextureCubeParameters cubeParams;
	cubeParams.faces[0] = "res/textures/skybox/right.jpg";
	cubeParams.faces[1] = "res/textures/skybox/left.jpg";
	cubeParams.faces[2] = "res/textures/skybox/top.jpg";
	cubeParams.faces[3] = "res/textures/skybox/bottom.jpg";
	cubeParams.faces[4] = "res/textures/skybox/front.jpg";
	cubeParams.faces[5] = "res/textures/skybox/back.jpg";


	SharedPtr<Texture2D> environmentMap = Texture2D::Create("res/textures/flamingo_pan_4k.hdr");
	SharedPtr<TextureCube> cubeTex  = m_PBRRenderer->EquirectangularToCubemap(environmentMap);
	m_PBRRenderer->SetSkyboxTexture(cubeTex);


	while (!m_Window->ShouldClose())
	{
		std::for_each(meshes.begin(), meshes.end(), [this](SharedPtr<Mesh> mesh) { mesh->SetTransform(m_MeshTransform); });

		m_Window->Update();


		m_PBRRenderer->Render(m_camera);
	
		DrawUI();

		m_MeshTransform.Rotation.y += m_MeshRotationSpeed;

		Application::GetInstance()->GetRenderer()->Clear(glm::vec4(m_BackgroundColor, 1.0f));

		HandleCameraMovement();
	}
}

void Application::OnWindowResize(int width, int height)
{
	m_PBRRenderer->Resize(width, height);
}

void Application::OnWindowMouseMove(double xpos, double ypos)
{
	if(!m_MovingCamera)
		return;

	float yaw = m_camera.GetYaw();
	float pitch = m_camera.GetPitch();
	yaw   += (xpos - m_LastMousePos.x) * m_MouseSensitivity;
	pitch += (ypos - m_LastMousePos.y) * m_MouseSensitivity;
	m_LastMousePos = { (float)xpos, (float)ypos };

	m_camera.SetPitch(pitch);
	m_camera.SetYaw(yaw);
}

void Application::HandleCameraMovement()
{

	if (ImGui::IsMouseDown(1) && !m_MovingCamera)
	{
		m_MovingCamera = true;

		m_LastMousePos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };

		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;

		m_Window->CaptureMouse(true);

	}
	if (ImGui::IsKeyDown(ImGuiKey_Escape))
	{
		m_MovingCamera = false;

		auto& io = ImGui::GetIO();
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		io.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;


		m_Window->CaptureMouse(false);
	}

	if(!m_MovingCamera)
		return;

	if (ImGui::IsKeyDown(ImGuiKey_W))
	{
		m_camera.Translate(m_camera.GetPosition() + (s_CamSpeed * m_camera.GetForwardDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_S))
	{
		m_camera.Translate(m_camera.GetPosition() - (s_CamSpeed * m_camera.GetForwardDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_D))
	{
		m_camera.Translate(m_camera.GetPosition() + (s_CamSpeed * m_camera.GetRightDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_A))
	{
		m_camera.Translate(m_camera.GetPosition() - (s_CamSpeed * m_camera.GetRightDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_E))
	{
		m_camera.Translate(m_camera.GetPosition() + (s_CamSpeed * m_camera.GetUpDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_Q))
	{
		m_camera.Translate(m_camera.GetPosition() - (s_CamSpeed * m_camera.GetUpDirection()));
	}
}

void Application::DrawUI()
{
	ImGuiCore::NewFrame();
	ImGui::Begin("Teszt");

	ImGui::DragFloat3("Mesh Translation", glm::value_ptr(m_MeshTransform.Position), 0.1f);
	ImGui::DragFloat3("Mesh Scale", glm::value_ptr(m_MeshTransform.Scale), 0.1f);
	ImGui::DragFloat3("Mesh Rotation", glm::value_ptr(m_MeshTransform.Rotation), 0.1f);
	ImGui::DragFloat("Mesh Rotation Speed", &m_MeshRotationSpeed, 0.0001f);

	if(ImGui::DragFloat("Cam FOV", &m_CamFOV, 0.1f))
		m_camera.SetFOV(m_CamFOV);

	if(ImGui::DragFloat("Cam NearPlane", &m_CamNearPlane, 0.1f))
		m_camera.SetNearClip(m_CamNearPlane);

	if(ImGui::DragFloat("Cam FarClip", &m_CamFarPlane, 0.1f))
		m_camera.SetFarClip(m_CamFarPlane);

	ImGui::ColorEdit3("Background Color", glm::value_ptr(m_BackgroundColor), 0.1f);
	ImGui::DragFloat("Mouse Sensitivity", &m_MouseSensitivity, 0.0001f);
	ImGui::DragFloat("Camera Speed", &s_CamSpeed, 0.1f);

	ImGui::End();
	ImGuiCore::EndFrame();
	m_Renderer->Present(); // for imgui
}

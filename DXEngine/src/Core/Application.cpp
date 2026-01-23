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

	m_Meshes = MeshImporter::ImportDynamicMesh("res/models/medieval_civilian_3/scene.gltf");

	for (int i = 0; i < m_Meshes.size(); i++)
	{	
		m_PBRRenderer->AddMesh(m_Meshes[i]);
		//m_Meshes[i]->SetTransform(m_MeshTransform);
	}


	Texture2DProperties hdrProps;
	hdrProps.sourcePath = "res/textures/horn-koppe_spring_4k.hdr";
	SharedPtr<Texture2D> environmentMap = Texture2D::Create(hdrProps);
	SharedPtr<TextureCube> cubeTex  = m_PBRRenderer->EquirectangularToCubemap(environmentMap);
	m_PBRRenderer->SetSkyboxTexture(cubeTex);

	m_DirectionalLight.Direction = glm::vec3(-0.5f, -1.0f, -0.3f);
	m_DirectionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_DirectionalLight.Intensity = 1.0f;

	m_DeltaTime = (float)glfwGetTime();


	while (!m_Window->ShouldClose())
	{
		float currentTime = (float)glfwGetTime();
		m_DeltaTime = currentTime - m_DeltaTime;

		m_Window->Update();
	

		m_PBRRenderer->AddDirectionalLight(m_DirectionalLight);
		m_PBRRenderer->Render(m_camera);

		DrawUI();
	
		m_Renderer->Clear(glm::vec4(m_BackgroundColor, 1.0f));

		HandleCameraMovement(currentTime);
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

void Application::HandleCameraMovement(float deltaTime)
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
		m_camera.Translate(m_camera.GetPosition() + (s_CamSpeed * deltaTime * m_camera.GetForwardDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_S))
	{
		m_camera.Translate(m_camera.GetPosition() - (s_CamSpeed * deltaTime * m_camera.GetForwardDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_D))
	{
		m_camera.Translate(m_camera.GetPosition() + (s_CamSpeed * deltaTime * m_camera.GetRightDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_A))
	{
		m_camera.Translate(m_camera.GetPosition() - (s_CamSpeed * deltaTime * m_camera.GetRightDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_E))
	{
		m_camera.Translate(m_camera.GetPosition() + (s_CamSpeed * deltaTime * m_camera.GetUpDirection()));
	}
	if (ImGui::IsKeyDown(ImGuiKey_Q))
	{
		m_camera.Translate(m_camera.GetPosition() - (s_CamSpeed * deltaTime * m_camera.GetUpDirection()));
	}
}

void Application::DrawUI()
{
	ImGuiCore::NewFrame();
	ImGui::Begin("Meshes");

	if (ImGui::Button("Recompile shaders"))
	{
		for (int i = 0; i < m_Meshes.size(); i++)
		{
			PBRMaterial& material = m_Meshes[i]->GetMaterial();
			material.Shader->Recompile();
		}
	}

	for (int i = 0; i < m_Meshes.size(); i++)
	{
		if (ImGui::CollapsingHeader(m_Meshes[i]->GetName().c_str()))
		{

			ImGui::PushID(i + m_Meshes.size());
			if (ImGui::Button("Jump to mesh", ImVec2(100, 20)))
			{
				m_camera.Translate(m_Meshes[i]->GetTransform().Position + glm::vec3(0.0f, 2.0f, 5.0f));
			}

			ImGui::DragFloat3("Mesh Translation", glm::value_ptr(m_Meshes[i]->GetTransform().Position), 0.1f);
			ImGui::DragFloat3("Mesh Scale", glm::value_ptr(m_Meshes[i]->GetTransform().Scale), 0.1f);
			ImGui::DragFloat3("Mesh Rotation", glm::value_ptr(m_Meshes[i]->GetTransform().Rotation), 0.1f);
			ImGui::PopID();

			ImGui::Separator();

			PBRMaterial& material = m_Meshes[i]->GetMaterial();
			std::string header = "Mesh " + std::to_string(i) + " Material";
			ImGui::PushID(i);
			if (ImGui::CollapsingHeader(header.c_str()))
			{
				ImGui::Image(material.BaseColorTexture ? (void*)material.BaseColorTexture->GetRendererID() : nullptr, ImVec2(64, 64));
				ImGui::ColorEdit3("BaseColor", glm::value_ptr(material.m_BaseColor));
				ImGui::DragFloat("Metallic", &material.m_Metallic, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Roughness", &material.m_Roughness, 0.01f, 0.0f, 1.0f);
				ImGui::Image(material.MetallicRoughnessTexture ? (void*)material.MetallicRoughnessTexture->GetRendererID() : nullptr, ImVec2(64, 64));

				ImGui::Image(material.NormalTexture ? (void*)material.NormalTexture->GetRendererID() : nullptr, ImVec2(64, 64));
				
				
			}
			ImGui::PopID();
		}
		
	}
	ImGui::End();

	ImGui::Begin("Light");

	if (ImGui::DragFloat("Cam FOV", &m_CamFOV, 0.1f))
		m_camera.SetFOV(m_CamFOV);

	if (ImGui::DragFloat("Cam NearPlane", &m_CamNearPlane, 0.1f))
		m_camera.SetNearClip(m_CamNearPlane);

	if (ImGui::DragFloat("Cam FarClip", &m_CamFarPlane, 0.1f))
		m_camera.SetFarClip(m_CamFarPlane);

	ImGui::DragFloat("Mouse Sensitivity", &m_MouseSensitivity, 0.0001f);
	ImGui::DragFloat("Camera Speed", &s_CamSpeed, 0.1f);

	float ambientIntensity = m_PBRRenderer->GetAmbientLightIntensity();
	if (ImGui::DragFloat("Ambient Light Intensity", &ambientIntensity, 0.01f))
		m_PBRRenderer->SetAmbientLightIntensity(ambientIntensity);

	ImGui::DragFloat3("Directional Light Direction", glm::value_ptr(m_DirectionalLight.Direction), 0.1f);
	ImGui::DragFloat3("Directional Light Color", glm::value_ptr(m_DirectionalLight.Color), 0.1f);
	ImGui::DragFloat("Directional Light Intensity", &m_DirectionalLight.Intensity, 0.1f);


	ImGui::End();

	ImGuiCore::EndFrame();
	m_Renderer->Present(); // for imgui
}

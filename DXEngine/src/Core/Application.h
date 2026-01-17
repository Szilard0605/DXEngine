#pragma once

#include "Window.h"

#include <string>

#include "Renderer/Backend/D3D11/D3D11Context.h"
#include "Renderer/Backend/D3D11/D3D11Renderer.h"

#include "Renderer/API/API.h"

#include "Renderer/PBRRenderer.h"	
#include "Utils/Utils.h"

#include "Renderer/Light.h"

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
	void OnWindowMouseMove(double xpos, double ypos);
	void HandleCameraMovement(float deltaTime);
	void DrawUI();
private:
	float m_DeltaTime = 0.0f;
	std::vector<SharedPtr<Mesh>> m_Meshes;
	DirectionalLight m_DirectionalLight;
	Math::Transform m_MeshTransform;
	glm::vec2 m_LastMousePos = glm::vec2(-1.0f);
	float m_MouseSensitivity = 0.0069f;
	bool m_MovingCamera = false;
	float s_CamSpeed = 1.301f;
	float m_CamNearPlane = 0.1f;
	float m_CamFarPlane = 4000.0f;
	float m_CamFOV = 60.0f;
	glm::vec3 m_MeshRotation = glm::vec3(0.0f);
	glm::vec3 m_BackgroundColor = glm::vec3(1.0f);
	static Application* s_Instance;
	PerspectiveCamera m_camera;
	Window* m_Window;
	SharedPtr<Renderer> m_Renderer;

	SharedPtr<PBRRenderer> m_PBRRenderer;
};


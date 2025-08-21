#include "ImGuiCore.h"

#include "imgui.h"

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_glfw.h"

#include "Core/Application.h"
#include "Renderer/API/Renderer.h"

#include "Renderer/Backend/D3D11/D3D11Context.h"

void ImGuiCore::Init(Window& window)
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.DisplaySize = ImVec2((float)window.GetProperties().width, (float)window.GetProperties().height);
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplGlfw_InitForOther(window.GetGLFWwindow(), true);

	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
		{
			ImGui_ImplDX11_Init(D3D11Context::Get()->GetDevice(), D3D11Context::Get()->GetDeviceContext());
			break;
		}
		case Renderer::API::None:
		{
			printf("[Error] ImGui: There is no Rendering API\n");
			break;
		}
	}
}

void ImGuiCore::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiCore::NewFrame()
{
	switch (Renderer::GetAPI())
	{
	case Renderer::API::D3D11:
		ImGui_ImplDX11_NewFrame();
		break;
	case Renderer::API::None:
		printf("[Error] ImGui: There is no Rendering API\n");
		break;
	}
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiCore::EndFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	Application* app = Application::GetInstance();
	float width = (float)app->GetWindow()->GetProperties().width;
	float height = (float)app->GetWindow()->GetProperties().height;
	io.DisplaySize = ImVec2(width, height);


	ImGui::Render();
	switch (Renderer::GetAPI())
	{
		case Renderer::API::D3D11:
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			break;

		case Renderer::API::None:
			printf("[Error] ImGui: There is no Rendering API\n");
			break;
	}

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

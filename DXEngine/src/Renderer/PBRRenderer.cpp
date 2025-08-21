#include "PBRRenderer.h"
#include <Core/Application.h>


struct RenderData
{
	glm::mat4 CamViewProjection;
	glm::mat4 MeshTransform;
} s_RenderData;

SharedPtr<Shader> s_TestShader = nullptr;

PBRRenderer::PBRRenderer(uint32_t width, uint32_t height)
{
	s_TestShader = Shader::Create("res/shaders/testshader.hlsl");

	RenderTargetDesc mainRenderTargetDesc;
	mainRenderTargetDesc.width = width;
	mainRenderTargetDesc.height = height;
	mainRenderTargetDesc.samples = 1; // No MSAA
	mainRenderTargetDesc.UseDepthBuffer = true; // Use depth buffer
	mainRenderTargetDesc.Attachments = { RenderTargetType::Color, RenderTargetType::Depth };

	m_MainRenderTarget = RenderTarget::Create(mainRenderTargetDesc);

	if (!m_MainRenderTarget)
	{
		printf("Failed to create main render target\n");
		return;
	}
	s_RenderData.CamViewProjection = glm::mat4(1.0f);
	s_RenderData.MeshTransform = glm::mat4(1.0f);
}

void PBRRenderer::AddMesh(SharedPtr<Mesh> mesh)
{
	m_Meshes.push_back(mesh);
}

void PBRRenderer::AddPointLight(PointLight pointLight, glm::vec3 position)
{
	m_PointLights.push_back(std::make_pair(pointLight, position));
}

void PBRRenderer::GeometryPass(PerspectiveCamera& camera)
{
	for (const auto& mesh : m_Meshes)
	{
		Math::Transform& meshTransform = mesh->GetTransform();

		SharedPtr<ConstantBuffer> renderDataBuffer = mesh->GetRenderDataBuffer();
		SharedPtr<VertexArray> meshVA = mesh->GetVertexArray();
		SharedPtr<IndexBuffer> meshIB = mesh->GetIndexBuffer();

		PBRMaterial& material = mesh->GetMaterial();

		glm::mat4 rotation = glm::toMat4(glm::quat(glm::vec4(meshTransform.Rotation, 0.0f)));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), meshTransform.Position) * rotation
			* glm::scale(glm::mat4(1.0f), meshTransform.Scale);

		s_RenderData.CamViewProjection = camera.GetViewProjection();
		s_RenderData.MeshTransform = model;

		material.Shader->Bind();

		renderDataBuffer->SetData(&s_RenderData, sizeof(s_RenderData));

		if (material.m_BaseColorTexture)
			material.m_BaseColorTexture->Bind(0);

		renderDataBuffer->Bind();
		meshIB->Bind();
		meshVA->DrawIndexed(meshIB->GetCount());
	}
}

void PBRRenderer::TestPass(PerspectiveCamera& camera)
{
	for (const auto& mesh : m_Meshes)
	{
		Math::Transform& meshTransform = mesh->GetTransform();

		SharedPtr<ConstantBuffer> renderDataBuffer = mesh->GetRenderDataBuffer();
		SharedPtr<VertexArray> meshVA = mesh->GetVertexArray();
		SharedPtr<IndexBuffer> meshIB = mesh->GetIndexBuffer();

		PBRMaterial& material = mesh->GetMaterial();

		glm::mat4 rotation = glm::toMat4(glm::quat(glm::vec4(meshTransform.Rotation, 0.0f)));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), meshTransform.Position) * rotation
			* glm::scale(glm::mat4(1.0f), meshTransform.Scale);

		s_RenderData.CamViewProjection = camera.GetViewProjection();
		s_RenderData.MeshTransform = model;

		//material.Shader->Bind();
		s_TestShader->Bind();

		renderDataBuffer->SetData(&s_RenderData, sizeof(s_RenderData));

		if (material.m_BaseColorTexture)
			material.m_BaseColorTexture->Bind(0);

		renderDataBuffer->Bind();
		meshIB->Bind();
		meshVA->DrawIndexed(meshIB->GetCount());
	}
}

void PBRRenderer::Render(PerspectiveCamera& camera)
{
	camera.UpdateView();

	Application::GetInstance()->GetRenderer()->BindViewport();


	m_MainRenderTarget->Clear(0.0f, 0.0f, 0.0f, 1.0f); 
	m_MainRenderTarget->ClearDepth(); 
	// Test pass for debugging
	{
		m_MainRenderTarget->Bind();
		TestPass(camera);
	}

	// Geometry pass
	{
		Application::GetInstance()->GetRenderer()->BindBackBuffer();
		//m_MainRenderTarget->Bind();
		GeometryPass(camera);
	}
}

void PBRRenderer::Resize(uint32_t width, uint32_t height)
{ 
	//m_MainRenderTarget->Resize(width, height);
	//m_MainRenderTarget->SetViewport(0, 0, width, height);
	Application::GetInstance()->GetRenderer()->Resize(width, height);
}
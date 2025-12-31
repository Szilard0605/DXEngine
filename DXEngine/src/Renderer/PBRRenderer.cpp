
#include "PBRRenderer.h"
#include "Core/Application.h"
#include "Renderer/API/Shader.h"
#include "Math/Transform.h"

struct RenderData
{
	glm::mat4 CamViewProjection;
	glm::mat4 MeshTransform;
} s_RenderData;


PBRRenderer::PBRRenderer(uint32_t width, uint32_t height)
{
	RenderTargetDesc mainRenderTargetDesc;
	mainRenderTargetDesc.width = width;
	mainRenderTargetDesc.height = height;
	mainRenderTargetDesc.samples = 1; // No MSAA
	mainRenderTargetDesc.UseDepthBuffer = true; // Use depth buffer
	mainRenderTargetDesc.Attachments = { RenderTargetType::Color, RenderTargetType::Depth};

	m_MainRenderTarget = RenderTarget::Create(mainRenderTargetDesc);

	if (!m_MainRenderTarget)
	{
		printf("Failed to create main render target\n");
		return;
	}

	s_RenderData.CamViewProjection = glm::mat4(1.0f);
	s_RenderData.MeshTransform = glm::mat4(1.0f);

	// Init Skybox
	m_SkyboxVA = VertexArray::Create();
	m_SkyboxVB = VertexBuffer::Create(VertexBuffer::BufferUsage::DYNAMIC);
	m_SkyboxIB = IndexBuffer::Create(m_SkyboxIndices, 36);

	m_SkyboxShader = Shader::Create("res/shaders/Skybox.hlsl");
	m_SkyboxShader->Bind();
	m_SkyboxVB->SetLayout({
		{ ShaderDataType::Float3, "a_position" }
	});
	m_SkyboxVB->SetData(m_SkyboxVertices, sizeof(m_SkyboxVertices));
	m_SkyboxVA->AddVertexBuffer(m_SkyboxVB);
	m_SkyboxRenderDataBuffer = ConstantBuffer::Create(&m_SkyboxRenderData, sizeof(m_SkyboxRenderData), ConstantBuffer::Type::Vertex);
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
	m_Camera = camera;

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

void PBRRenderer::Render(PerspectiveCamera& camera)
{
	camera.UpdateView();

	Application::GetInstance()->GetRenderer()->BindViewport();

	m_MainRenderTarget->Clear(0.0f, 0.0f, 0.0f, 1.0f); 
	m_MainRenderTarget->ClearDepth(); 

	// Geometry pass
	{
		Application::GetInstance()->GetRenderer()->BindBackBuffer();
		Application::GetInstance()->GetRenderer()->DisableDepthTesting(true);
		SkyboxPass();
		Application::GetInstance()->GetRenderer()->DisableDepthTesting(false);
		GeometryPass(camera);
	}
}

void PBRRenderer::Resize(uint32_t width, uint32_t height)
{ 
	Application::GetInstance()->GetRenderer()->Resize(width, height);
}

void PBRRenderer::SkyboxPass()
{
	if (!m_SkyboxTexture)
		return;

	m_SkyboxShader->Bind();

	m_SkyboxRenderData.ProjectionMatrix = m_Camera.GetProjectionMatrix();
	m_SkyboxRenderData.ViewMatrix = glm::mat4(glm::mat3(m_Camera.GetViewMatrix())); // Remove translation from the view matrix
	

	m_SkyboxRenderDataBuffer->SetData(&m_SkyboxRenderData, sizeof(m_SkyboxRenderData));
	m_SkyboxTexture->Bind(0);
	m_SkyboxIB->Bind();
	m_SkyboxRenderDataBuffer->Bind();
	m_SkyboxVB->Bind();
	m_SkyboxVA->DrawIndexed(m_SkyboxIB->GetCount());
}
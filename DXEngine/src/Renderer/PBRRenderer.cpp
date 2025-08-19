#include "PBRRenderer.h"


struct RenderData
{
	glm::mat4 CamViewProjection;
	glm::mat4 MeshTransform;
} s_RenderData;

PBRRenderer::PBRRenderer()
{// Initialize the PBR shade
}

void PBRRenderer::AddMesh(SharedPtr<Mesh> mesh)
{
	m_Meshes.push_back(mesh);
}

void PBRRenderer::AddPointLight(PointLight pointLight, glm::vec3 position)
{
	m_PointLights.push_back(std::make_pair(pointLight, position));
}

void PBRRenderer::Render(PerspectiveCamera& camera)
{
	for (const auto& mesh : m_Meshes)
	{
		Math::Transform meshTransform = mesh->GetTransform();

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
#pragma once

#include "glm.hpp"
#include <vector>
#include <filesystem>

#include "Utils/Utils.h"
#include "Renderer/API/API.h"
#include "Renderer/Camera/PerspectiveCamera.h"
#include "Math/Transform.h"

#include "PBRMaterial.h"

#include <iostream>

struct StaticMeshVertex
{
	glm::vec3 Position;
	/*glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;*/
	glm::vec2 TexCoords;
};

struct Material
{
	glm::vec3 BaseColor = glm::vec3{1};
	Texture2D* Texture = nullptr;
	SharedPtr<Shader> Shader = nullptr;
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(std::vector<StaticMeshVertex> vertices, std::vector<uint32_t> indices, PBRMaterial& material);
	~Mesh();

	static std::vector<SharedPtr<Mesh>> ImportDynamicMesh(std::filesystem::path path);
	static Mesh ImportStaticMesh(std::filesystem::path path);

	void SetTransform(Math::Transform transform) { m_Transform = transform; }
	Math::Transform& GetTransform() { return m_Transform; }

	void Render(PerspectiveCamera& camera);

	PBRMaterial& GetMaterial() { return m_Material; }
	void SetMaterial(PBRMaterial& material) { m_Material = material; }

	std::vector<StaticMeshVertex>& GetVertices()  { return m_Vertices; }
	std::vector<uint32_t>&		   GetIndices()   { return m_Indices;  }

	SharedPtr<VertexArray>    GetVertexArray()		{ return m_VertexArray;		 }
	SharedPtr<VertexBuffer>   GetVertexBuffer()		{ return m_VertexBuffer;	 }
	SharedPtr<IndexBuffer>    GetIndexBuffer()		{ return m_IndexBuffer;		 }
	SharedPtr<ConstantBuffer> GetRenderDataBuffer() { return m_RenderDataBuffer; }

private:
	Math::Transform m_Transform;

	SharedPtr<VertexArray>    m_VertexArray      = nullptr;
	SharedPtr<IndexBuffer>    m_IndexBuffer      = nullptr;
	SharedPtr<VertexBuffer>   m_VertexBuffer     = nullptr;
	SharedPtr<ConstantBuffer> m_RenderDataBuffer = nullptr;

	struct RenderData
	{
		glm::mat4 CamViewProjection;
		glm::mat4 MeshTransform;
	} m_RenderData;

	PBRMaterial m_Material;

	std::vector<StaticMeshVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
};

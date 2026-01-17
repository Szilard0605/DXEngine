#pragma once

#include "glm.hpp"
#include <vector>

#include "Utils/Utils.h"
#include "Renderer/API/API.h"
#include "Math/Math.h"

#include "PBRMaterial.h"

#include <iostream>

struct MeshVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
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
	Mesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices, PBRMaterial& material);
	~Mesh();

	void InitializeBuffers();

	void SetTransform(Math::Transform transform) { m_Transform = transform; }
	Math::Transform& GetTransform() { return m_Transform; }

	PBRMaterial& GetMaterial() { return m_Material; }
	void SetMaterial(PBRMaterial& material) { m_Material = material; }

	void SetVertices(const std::vector<MeshVertex>& vertices) { m_Vertices = vertices; }
	void SetIndices(const std::vector<uint32_t>& indices) { m_Indices = indices; }

	void SetName(const std::string& name) { m_Name = name; }
	std::string& GetName() { return m_Name; }

	std::vector<MeshVertex>& GetVertices()  { return m_Vertices; }
	std::vector<uint32_t>&	 GetIndices()   { return m_Indices;  }

	SharedPtr<VertexArray>    GetVertexArray()		{ return m_VertexArray;		 }
	SharedPtr<VertexBuffer>   GetVertexBuffer()		{ return m_VertexBuffer;	 }
	SharedPtr<IndexBuffer>    GetIndexBuffer()		{ return m_IndexBuffer;		 }
	SharedPtr<ConstantBuffer> GetRenderDataBuffer() { return m_RenderDataBuffer; }

private:

	Math::Transform m_Transform;

	std::string m_Name;

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

	std::vector<MeshVertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
};

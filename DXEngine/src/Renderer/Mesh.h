#pragma once

#include "glm.hpp"
#include <vector>
#include "Renderer/API/API.h"
#include "Renderer/Camera/PerspectiveCamera.h"

#include <filesystem>

struct Vertex
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
	Shader* Shader = nullptr;
};

class Mesh
{
public:



	Mesh() = default;
	//Mesh(GLVertexArray& vertexarray, GLIndexBuffer& indexbuffer, Material& material);
	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Material material);
	//Mesh(Vertex vertex, uint32_t* indices, Material& material);

	static std::vector<Mesh*> Import(std::filesystem::path path);


	~Mesh();

	void Render(PerspectiveCamera& camera, glm::vec3 Position, glm::vec3 Scale, glm::vec3 Rotation);

	VertexArray* GetVertexArray() { return m_VertexArray; }
	Material* GetMaterial() { return m_Material; }
	void SetMaterial(Material* material) { m_Material = material; }

private:
	VertexArray* m_VertexArray;
	IndexBuffer* m_IndexBuffer;
	VertexBuffer* m_VertexBuffer;
	ConstantBuffer* m_RenderDataBuffer;

	struct RenderData
	{
		glm::mat4 CamViewProjection;
		glm::mat4 MeshTransform;
	} m_RenderData;

	Material* m_Material;

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
};

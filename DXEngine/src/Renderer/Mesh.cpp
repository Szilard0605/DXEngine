#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <iostream>
#include <Core/Application.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Material material)
	: m_Vertices(vertices), m_Indices(indices)
{
	//m_VertexBuffer = VertexBuffer::Create(static_cast<uint32_t>(vertices.size() * sizeof(Vertex)));
	m_Material = new Material(material);
	m_Material->Shader = Shader::Create("res/shaders/testshader.hlsl");
	m_Material->Shader->Bind();
	
	m_VertexBuffer = VertexBuffer::Create(VertexBuffer::BufferUsage::DYNAMIC);

	m_VertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_position" },
		/*{ShaderDataType::Float3, "a_normal"},
		{ ShaderDataType::Float3, "a_tangent"  },
		{ ShaderDataType::Float3, "a_bitangent"},*/
		{ ShaderDataType::Float2, "a_texcoords"}
		});

	m_VertexArray = VertexArray::Create();
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	m_IndexBuffer = IndexBuffer::Create(indices.data(), static_cast<uint32_t>(indices.size()));

	m_RenderDataBuffer = ConstantBuffer::Create(&m_RenderData, sizeof(m_RenderData), 0);
	m_RenderDataBuffer->SetData(&m_RenderData, sizeof(m_RenderData));
}

std::vector<Mesh*> Mesh::Import(std::filesystem::path path)
{

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "[ASSIMP] " << std::string(importer.GetErrorString()) << "\n";
		return std::vector<Mesh*>();
	}

	std::vector<Mesh*> retMeshes;

	for (uint32_t i = 0; i < scene->mNumMeshes; i++)
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		Material material{};

		for (uint32_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			Vertex vertex;

			vertex.Position = { scene->mMeshes[i]->mVertices[j].x,
								 scene->mMeshes[i]->mVertices[j].y,
								 scene->mMeshes[i]->mVertices[j].z };

			if (scene->mMeshes[i]->mTextureCoords[0])
			{
				const float uv_X = scene->mMeshes[i]->mTextureCoords[0][j].x;
				const float uv_Y = scene->mMeshes[i]->mTextureCoords[0][j].y;
				vertex.TexCoords = glm::vec2(uv_X, uv_Y);
			}
			else
			{
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			}

			Vertices.push_back(vertex);
		}

		Indices.reserve((size_t)scene->mMeshes[i]->mNumFaces * 3);

		for (uint32_t j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
		{
			for (uint32_t k = 0; k < scene->mMeshes[i]->mFaces[j].mNumIndices; k++)
			{
				Indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[k]);
			}
		}

		const aiMesh* model = scene->mMeshes[i];
		const aiMaterial* mtl = scene->mMaterials[model->mMaterialIndex];

		Texture2D* baseColorTexture = nullptr;
		// Base Color textures
		{
			aiString texpath;	// filename
			aiReturn texFound = mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texpath);

			if (texFound == AI_SUCCESS)
			{
				material.Texture = Texture2D::Create(path.remove_filename().string() + texpath.C_Str());
			}
		}

		retMeshes.push_back(new Mesh(Vertices, Indices, material));
	}

	return retMeshes;
}

void Mesh::Render(PerspectiveCamera& camera, glm::vec3 Position, glm::vec3 Scale, glm::vec3 Rotation)
{
	camera.UpdateView();

	m_Material->Shader->Bind();

	glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

	glm::mat4 model = glm::translate(glm::mat4(1.0f), Position) * rotation
					 * glm::scale(glm::mat4(1.0f), Scale);

	m_RenderData.CamViewProjection = camera.GetViewProjection();
	m_RenderData.MeshTransform = model;
	m_RenderDataBuffer->SetData(&m_RenderData, sizeof(m_RenderData));

	m_VertexBuffer->SetData(m_Vertices.data(), static_cast<uint32_t>(m_Vertices.size() * sizeof(Vertex)));


	if (m_Material->Texture)
	{
		m_Material->Texture->Bind(0);
	}

	m_RenderDataBuffer->Bind();
	m_IndexBuffer->Bind();
	m_VertexArray->DrawIndexed(m_IndexBuffer->GetCount());
}
#include "Mesh.h"
#include <iostream>
#include <Core/Application.h>

Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices, PBRMaterial& material)
	: m_Vertices(vertices), m_Indices(indices), m_Material(material)
{
	InitializeBuffers();
}


Mesh::~Mesh()
{

}

void Mesh::InitializeBuffers()
{
	//m_VertexBuffer = VertexBuffer::Create(static_cast<uint32_t>(vertices.size() * sizeof(Vertex)));
	m_Material.Shader = Shader::Create("res/shaders/PBRShader.hlsl");
	m_Material.Shader->Bind();
	m_VertexBuffer = VertexBuffer::Create(VertexBuffer::BufferUsage::DYNAMIC);

	m_VertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_position" },
		{ShaderDataType::Float3, "a_normal"},
		{ ShaderDataType::Float3, "a_tangent"  },
		{ ShaderDataType::Float3, "a_bitangent"},
		{ ShaderDataType::Float2, "a_texcoords"}
		});

	m_VertexArray = VertexArray::Create();
	m_VertexBuffer->SetData(m_Vertices.data(), static_cast<uint32_t>(m_Vertices.size() * sizeof(MeshVertex)));
	m_VertexArray->AddVertexBuffer(m_VertexBuffer);

	m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), static_cast<uint32_t>(m_Indices.size()));

	m_RenderDataBuffer = ConstantBuffer::Create(&m_RenderData, sizeof(m_RenderData), 0);
	m_RenderDataBuffer->SetData(&m_RenderData, sizeof(m_RenderData));
}

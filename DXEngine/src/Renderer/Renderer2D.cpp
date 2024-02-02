#include "Renderer2D.h"

#include "Core/Application.h"
#include <ext/matrix_transform.hpp>

#include "Platform/D3D11/D3D11ConstantBuffer.h"

#include "DirectXMath.h"

using namespace DirectX;

struct CameraBuferData
{
	glm::mat4 ViewProjection;
};

struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoords;
};

struct QuadRenderData
{
	Shader* Shader;
	CameraBuferData CameraData;
	ConstantBuffer* CameraBuffer;
};

static QuadRenderData s_QuadRenderData;

void Renderer2D::Init(Renderer* renderer)
{
	s_QuadRenderData.Shader = Shader::Create("res/shaders/testshader.hlsl");

	s_QuadRenderData.CameraBuffer = ConstantBuffer::Create(&s_QuadRenderData.CameraData, sizeof(CameraBuferData), 0);
	s_QuadRenderData.CameraBuffer->SetData(&s_QuadRenderData.CameraData, sizeof(CameraBuferData));
	
	s_QuadRenderData.Shader->AddConstantBuffer(s_QuadRenderData.CameraBuffer);
}

void Renderer2D::DrawQuad(const glm::vec3& Position, const glm::vec3& size, float rotation, const glm::vec4& color, Texture2D* texture, PerspectiveCamera & camera)
{
	
	s_QuadRenderData.Shader->Bind();

	camera.CalculateProjection();
	camera.CalculatePosition();
	camera.UpdateView();

	//s_QuadRenderData.CameraData.ProjectionMatrix = camera.GetProjectionMatrix();
	s_QuadRenderData.CameraData.ViewProjection = camera.GetViewProjection();
	
	s_QuadRenderData.CameraBuffer->SetData(&s_QuadRenderData.CameraData, sizeof(CameraBuferData));

	QuadVertex vertices[4];

	VertexBuffer* vb = VertexBuffer::Create(VertexBuffer::BufferUsage::DYNAMIC);
	VertexArray* va = VertexArray::Create();

	vb->SetLayout({
		{ ShaderDataType::Float3, "a_position"  },
		{ ShaderDataType::Float4, "a_color"	    },
		{ ShaderDataType::Float2, "a_texcoords" }
	});

	glm::vec4 Positions[4];

	Positions[0] = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	Positions[1] = glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);
	Positions[2] = glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
	Positions[3] = glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), Position)
		* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 1.0f, 0.0f })
		* glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

	constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

	for (int i = 0; i < 4; i++)
	{
		vertices[i].Position = model * Positions[i];
		vertices[i].Color = color;
		vertices[i].TexCoords = textureCoords[i];
	}

	
	// CONSTANT BUFFERS ARE NOT WORKING
	
	vb->SetData(vertices, sizeof(QuadVertex) * 4);

	va->AddVertexBuffer(vb);

	uint32_t indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};

	IndexBuffer* ib = IndexBuffer::Create(indices, 6);

	s_QuadRenderData.CameraBuffer->Bind();

	ib->Bind();
	texture->Bind(0);
	va->DrawIndexed(6);
}

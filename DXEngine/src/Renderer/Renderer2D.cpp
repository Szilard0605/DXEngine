#include "Renderer2D.h"

#include "Core/Application.h"
#include <ext/matrix_transform.hpp>

struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
};

void Renderer2D::Init(Renderer* renderer)
{
	

}

void Renderer2D::DrawQuad(const glm::vec3& Position, const glm::vec3& size, float rotation, const glm::vec4& color, Shader* shader, PerspectiveCamera& camera)
{
	shader->Bind();

	camera.CalculateProjection();
	camera.CalculatePosition();
	camera.UpdateView();

	Application::GetInstance()->GetRenderer()->UseDepthTesting(false);

	QuadVertex vertices[4];

	VertexBuffer* vb = VertexBuffer::Create(VertexBuffer::BufferUsage::DYNAMIC);
	VertexArray* va = VertexArray::Create();

	vb->SetLayout({
		{ ShaderDataType::Float3, "a_position" },
		{ ShaderDataType::Float4, "a_color" }
	});

	glm::mat4 model = glm::translate(glm::mat4(1.0f), Position)
						  * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 1.0f, 0.0f })
						  * glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

	vertices[0].Position = glm::vec3(-0.5f, -0.5f, 1.0f);
	vertices[1].Position = glm::vec3( 0.5f, -0.5f, 1.0f);
	vertices[2].Position = glm::vec3( 0.5f,  0.5f, 1.0f);
	vertices[3].Position = glm::vec3(-0.5f,  0.5f, 1.0f);

	for (int i = 0; i < 4; i++)
	{
		vertices[i].Position = model * glm::vec4(vertices[i].Position, 1.0f);
		vertices[i].Color = color;
	}

	

	struct CamData										
	{
		glm::mat4 ViewProjectionMatrix;
	} CamData;

	CamData.ViewProjectionMatrix = camera.GetViewProjection();
	
	// CONSTANT BUFFERS ARE NOT WORKING
	shader->GetConstantBuffers()[0]->SetData(&CamData, sizeof(CamData));

	vb->SetData(vertices, sizeof(QuadVertex) * 4);

	va->AddVertexBuffer(vb);

	uint32_t indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};

	IndexBuffer* ib = IndexBuffer::Create(indices, 6);

	shader->GetConstantBuffers()[0]->Bind();

	ib->Bind();
	va->DrawIndexed(6);
}

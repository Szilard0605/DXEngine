
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
	m_CameraDataBuffer = ConstantBuffer::Create(&m_CameraBufferData, sizeof(m_CameraBufferData), 0, ConstantBuffer::Type::Vertex);
	
	m_EquirectToCubemapShader = Shader::Create("res/shaders/EquirectToCube.hlsl");
	m_CubeVB = VertexBuffer::Create(VertexBuffer::BufferUsage::DYNAMIC);
	m_CubeVB->SetLayout({
		{ ShaderDataType::Float3, "a_position" }
	});

	struct CubeVertex
	{
		float x, y, z;
	};
	static const CubeVertex cubeVertices[] =
	{
		// +X
		{  1, -1, -1 }, {  1,  1, -1 }, {  1,  1,  1 },
		{  1, -1, -1 }, {  1,  1,  1 }, {  1, -1,  1 },

		// -X
		{ -1, -1,  1 }, { -1,  1,  1 }, { -1,  1, -1 },
		{ -1, -1,  1 }, { -1,  1, -1 }, { -1, -1, -1 },

		// +Y
		{ -1,  1, -1 }, { -1,  1,  1 }, {  1,  1,  1 },
		{ -1,  1, -1 }, {  1,  1,  1 }, {  1,  1, -1 },

		// -Y
		{ -1, -1,  1 }, { -1, -1, -1 }, {  1, -1, -1 },
		{ -1, -1,  1 }, {  1, -1, -1 }, {  1, -1,  1 },

		// +Z
		{ -1, -1,  1 }, {  1, -1,  1 }, {  1,  1,  1 },
		{ -1, -1,  1 }, {  1,  1,  1 }, { -1,  1,  1 },

		// -Z
		{  1, -1, -1 }, { -1, -1, -1 }, { -1,  1, -1 },
		{  1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 },
	};

	m_CubeVB->SetData((void*)cubeVertices, sizeof(cubeVertices));
	m_CubeVA = VertexArray::Create();
	m_CubeVA->AddVertexBuffer(m_CubeVB);

	m_EquirectToCubeCB = ConstantBuffer::Create(&m_EquirectToCubeData, sizeof(m_EquirectToCubeData), 0, ConstantBuffer::Type::Pixel);

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
	Application::GetInstance()->GetRenderer()->BindBackBuffer();
	{
		SkyboxPass();
		GeometryPass(camera);
	}
}

void PBRRenderer::Resize(uint32_t width, uint32_t height)
{ 
	Application::GetInstance()->GetRenderer()->Resize(width, height);
}

inline int nearestPowerOfTwo(int n)
{
	if (n <= 0) return 1;
	int exponent = static_cast<int>(glm::round(glm::log2(static_cast<float>(n))));
	return 1 << exponent;
}

SharedPtr<TextureCube> PBRRenderer::EquirectangularToCubemap(SharedPtr<Texture2D> equirectangularMap)
{
	if (!equirectangularMap)
		return nullptr;

	std::vector<SharedPtr<RenderTarget>> RTVs(6);
	Texture2DProperties& equirectProps = equirectangularMap->GetProperties();

	// Cube face view matrices
	glm::vec3 eye(0.0f);
	glm::mat4 captureViews[6] =
	{
		glm::lookAt(eye, eye + glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)), // +X
		glm::lookAt(eye, eye + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)), // -X
		glm::lookAt(eye, eye + glm::vec3(0,  1, 0), glm::vec3(0, 0,  1)), // +Y
		glm::lookAt(eye, eye + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)), // -Y
		glm::lookAt(eye, eye + glm::vec3(0, 0,  1), glm::vec3(0,-1, 0)), // +Z
		glm::lookAt(eye, eye + glm::vec3(0, 0, -1), glm::vec3(0,-1, 0)), // -Z
	};

	// Cube face size: max vertical resolution, nearest power of two
	int faceSize = min(equirectProps.width / 2, equirectProps.height);
	m_CameraBufferData.ProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	// Render each face
	for (int i = 0; i < 6; i++)
	{
		RenderTargetDesc rtvDesc{};
		rtvDesc.width = faceSize;
		rtvDesc.height = faceSize;
		rtvDesc.Attachments.push_back(RenderTargetType::Color);
		RTVs[i] = RenderTarget::Create(rtvDesc);
		RTVs[i]->Clear(0.0f, 0.0f, 0.0f, 1.0f);
	}
	m_EquirectToCubemapShader->Bind();

	Application::GetInstance()->GetRenderer()->SetViewport(0, 0, faceSize, faceSize);

	// Render each face
	for (int i = 0; i < 6; i++)
	{
		// 90° FOV, 1:1 aspect
		m_CameraBufferData.ViewMatrix = captureViews[i];
		m_CameraDataBuffer->SetData(&m_CameraBufferData, sizeof(m_CameraBufferData));

		m_EquirectToCubeData.FaceIndex = i;
		m_EquirectToCubeCB->SetData(&m_EquirectToCubeData, sizeof(m_EquirectToCubeData));

		m_EquirectToCubemapShader->Bind();
		m_CameraDataBuffer->Bind();
		m_EquirectToCubeCB->Bind();
		equirectangularMap->Bind(0);
		RTVs[i]->Bind();
		m_CubeVA->Draw(36);
		RTVs[i]->Unbind();
	}

	Application::GetInstance()->GetRenderer()->SetViewport(0, 0, Application::GetInstance()->GetWindow()->GetProperties().width, Application::GetInstance()->GetWindow()->GetProperties().height);

	return TextureCube::Create(RTVs);
}

void PBRRenderer::SkyboxPass()
{
	if (!m_SkyboxTexture)
		return;

	m_SkyboxShader->Bind();

	m_CameraBufferData.ProjectionMatrix = m_Camera.GetProjectionMatrix();
	m_CameraBufferData.ViewMatrix = glm::mat4(glm::mat3(m_Camera.GetViewMatrix())); // Remove translation from the view matrix
	

	m_CameraDataBuffer->SetData(&m_CameraBufferData, sizeof(m_CameraBufferData));
	m_SkyboxTexture->Bind(0);
	m_SkyboxIB->Bind();
	m_CameraDataBuffer->Bind();
	m_SkyboxVB->Bind();

	Application::GetInstance()->GetRenderer()->DisableDepthTesting(true);
	m_SkyboxVA->DrawIndexed(m_SkyboxIB->GetCount());
	Application::GetInstance()->GetRenderer()->DisableDepthTesting(false);
}
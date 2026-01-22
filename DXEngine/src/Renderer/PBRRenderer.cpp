
#include "PBRRenderer.h"
#include "Core/Application.h"
#include "Renderer/API/Shader.h"
#include "Math/Math.h"

struct RenderData
{
	glm::mat4 CamViewProjection;
	glm::mat4 MeshTransform;
} s_RenderData;


PBRRenderer::PBRRenderer(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height)
{
	m_Renderer = Application::GetInstance()->GetRenderer();

	s_RenderData.CamViewProjection = glm::mat4(1.0f);
	s_RenderData.MeshTransform = glm::mat4(1.0f);

	// Init Skybox
	m_SkyboxVA = VertexArray::Create();
	m_SkyboxVB = VertexBuffer::Create(VertexBuffer::BufferUsage::DYNAMIC);
	m_SkyboxIB = IndexBuffer::Create(m_SkyboxIndices, 36);

	m_SkyboxShader = Shader::Create("res/shaders/Skybox.hlsl");
	m_SkyboxShader->Bind();
	m_SkyboxVB->SetLayout({
		{ ShaderDataType::Float3, "a_position"  }
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
	m_CameraBufferData.ProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	m_LightDataBuffer = ConstantBuffer::Create(&m_LightBufferData, sizeof(m_LightBufferData), 1, ConstantBuffer::Type::Pixel);
	m_MaterialDataBuffer = ConstantBuffer::Create(&m_MaterialBufferData, sizeof(m_MaterialBufferData), 2, ConstantBuffer::Type::Pixel);

}

void PBRRenderer::AddMesh(SharedPtr<Mesh> mesh)
{
	m_Meshes.push_back(mesh);
}

void PBRRenderer::AddPointLight(PointLight pointLight)
{
	m_PointLights.push_back(pointLight);
}

void PBRRenderer::AddDirectionalLight(DirectionalLight dirLight)
{
	if (m_DirectionalLights.size() > 0)
	{
		m_DirectionalLights[0] = dirLight;
		return;
	}
	m_DirectionalLights.push_back(dirLight);
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
		SharedPtr<VertexBuffer> meshVB = mesh->GetVertexBuffer();

		PBRMaterial& material = mesh->GetMaterial();

		glm::mat4 rotation = glm::toMat4(glm::quat(glm::vec4(meshTransform.Rotation, 0.0f)));

		glm::mat4 model = glm::translate(glm::mat4(1.0f), meshTransform.Position) * rotation
			* glm::scale(glm::mat4(1.0f), meshTransform.Scale);

		s_RenderData.CamViewProjection = camera.GetViewProjection();
		s_RenderData.MeshTransform = model;


		m_LightBufferData.AmbientLightIntensity = m_AmbientLightIntensity;

		if (m_DirectionalLights.size() > 0)
		{
			m_LightBufferData.DirectionalLightColor = m_DirectionalLights[0].Color;
			m_LightBufferData.DirectionalLightDirection = m_DirectionalLights[0].Direction;
			m_LightBufferData.CameraPosition = camera.GetPosition();
		}

		material.Shader->Bind();

		renderDataBuffer->SetData(&s_RenderData, sizeof(s_RenderData));

		m_LightDataBuffer->SetData(&m_LightBufferData, sizeof(m_LightBufferData));

		m_MaterialBufferData.BaseColor = material.m_BaseColor;
		m_MaterialBufferData.Metallic = material.m_Metallic;
		m_MaterialBufferData.Roughness = material.m_Roughness;
		m_MaterialBufferData.Specular = material.m_Specular;

		if (material.BaseColorTexture)
		{
			m_MaterialBufferData.hasAlbedoMap = 1;
			material.BaseColorTexture->Bind(0);
		}

		if (material.NormalTexture)
		{
			m_MaterialBufferData.hasNormalMap = 1;
			material.NormalTexture->Bind(1);
		}

		if (material.MetallicRoughnessTexture)
		{
			m_MaterialBufferData.hasMetallicRougnessTexture = 1;
			material.MetallicRoughnessTexture->Bind(2);
		}

		if(m_SkyboxTexture)
			m_SkyboxTexture->Bind(4);

		m_MaterialDataBuffer->SetData(&m_MaterialBufferData, sizeof(m_MaterialBufferData));

		renderDataBuffer->Bind();
		m_LightDataBuffer->Bind();
		m_MaterialDataBuffer->Bind();
		meshIB->Bind();
		meshVB->Bind();
		meshVA->DrawIndexed(meshIB->GetCount());
	}
}

void PBRRenderer::Render(PerspectiveCamera& camera)
{
	camera.UpdateView();

	m_Renderer->BindViewport();
	m_Renderer->BindBackBuffer();
	{
		SkyboxPass();
		GeometryPass(camera);
	}
	//m_Renderer->Present();
}

void PBRRenderer::Resize(uint32_t width, uint32_t height)
{ 
	m_Renderer->Resize(width, height);
}

SharedPtr<TextureCube> PBRRenderer::EquirectangularToCubemap(SharedPtr<Texture2D> equirectangularMap)
{
	if (!equirectangularMap)
		return nullptr;

	static std::vector<SharedPtr<RenderTarget>> RTVs(6);
	Texture2DProperties& equirectProps = equirectangularMap->GetProperties();

	// Cube face view matrices
	static glm::vec3 eye(0.0f);
	static glm::mat4 captureViews[6] =
	{
		glm::lookAt(eye, eye + glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)), // +X
		glm::lookAt(eye, eye + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)), // -X
		glm::lookAt(eye, eye + glm::vec3(0,  1, 0), glm::vec3(0, 0,  1)), // +Y
		glm::lookAt(eye, eye + glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)), // -Y
		glm::lookAt(eye, eye + glm::vec3(0, 0,  1), glm::vec3(0,-1, 0)), // +Z
		glm::lookAt(eye, eye + glm::vec3(0, 0, -1), glm::vec3(0,-1, 0)), // -Z
	};

	static int faceSize = min(equirectProps.width / 2, equirectProps.height);

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

	m_Renderer->SetViewport(0, 0, faceSize, faceSize);

	for (int i = 0; i < 6; i++)
	{
		m_CameraBufferData.ViewMatrix = captureViews[i];
		m_CameraDataBuffer->SetData(&m_CameraBufferData, sizeof(m_CameraBufferData));

		m_EquirectToCubeData.FaceIndex = i;
		m_EquirectToCubeCB->SetData(&m_EquirectToCubeData, sizeof(m_EquirectToCubeData));

		m_CameraDataBuffer->Bind();
		m_EquirectToCubeCB->Bind();
		equirectangularMap->Bind(0);
		RTVs[i]->Bind();
		m_CubeVA->Draw(36);
		RTVs[i]->Unbind();
	}

	m_Renderer->SetViewport(0, 0, m_Width, m_Height);

	return TextureCube::Create(RTVs);
}

void PBRRenderer::SkyboxPass()
{
	if (!m_SkyboxTexture)
		return;

	m_CameraBufferData.ProjectionMatrix = m_Camera.GetProjectionMatrix();
	m_CameraBufferData.ViewMatrix = glm::mat4(glm::mat3(m_Camera.GetViewMatrix())); // Remove translation from the view matrix

	m_SkyboxShader->Bind();
	m_CameraDataBuffer->SetData(&m_CameraBufferData, sizeof(m_CameraBufferData));

	m_SkyboxTexture->Bind(0);
	m_SkyboxIB->Bind();
	m_CameraDataBuffer->Bind();
	m_SkyboxVB->Bind();

	m_SkyboxVA->DrawIndexed(m_SkyboxIB->GetCount());
}

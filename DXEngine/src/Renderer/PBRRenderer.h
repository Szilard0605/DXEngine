#pragma once

#include <memory>
#include <stdint.h>
#include "glm.hpp"

#include "Utils/Utils.h"
#include "Mesh.h"
#include "Light.h"
#include "Camera/PerspectiveCamera.h"
#include "Renderer/API/TextureCube.h"
#include "Renderer/API/ConstantBuffer.h"
#include "Image.h"

class PBRRenderer
{
public:
	PBRRenderer(uint32_t width, uint32_t height);

	void Render(PerspectiveCamera& camera);
	void AddMesh(SharedPtr<Mesh> mesh);
	void AddPointLight(PointLight pointLight);
	void AddDirectionalLight(DirectionalLight dirLight);

	void Resize(uint32_t width, uint32_t height);
	void SetSkyboxTexture(SharedPtr<TextureCube> texture) { m_SkyboxTexture = texture; }
	SharedPtr<TextureCube> EquirectangularToCubemap(SharedPtr<Texture2D> equirectangularMap);

	float GetAmbientLightIntensity() const { return m_AmbientLightIntensity; }
	void SetAmbientLightIntensity(float intensity) { m_AmbientLightIntensity = intensity; }

private:
	void GeometryPass(PerspectiveCamera& camera);
	void SkyboxPass();

private:
	float m_AmbientLightIntensity = 0.1f;

	uint32_t m_Width, m_Height;
	SharedPtr<Renderer> m_Renderer;

	PerspectiveCamera m_Camera;

	std::vector<SharedPtr<Mesh>> m_Meshes;

	std::vector<PointLight> m_PointLights;
	std::vector<DirectionalLight> m_DirectionalLights;

	struct alignas(16)
	{
		float AmbientLightIntensity = 0;
		glm::vec3 DirectionalLightDirection;
		glm::vec3 DirectionalLightColor;
		float Padding2 = 0;
		glm::vec3 CameraPosition;
		float Padding3 = 0;

	} m_LightBufferData;

	SharedPtr<ConstantBuffer> m_LightDataBuffer = nullptr;

	struct alignas(16)
	{
		glm::vec3 BaseColor;
		float     Metallic;
		float     Roughness;
		float     Specular;
		float     Padding;

		uint32_t hasAlbedoMap = 0;
		uint32_t hasNormalMap = 0;
		uint32_t hasMetallicRougnessTexture = 0;

		float padding1;
		float padding2;	
	} m_MaterialBufferData;

	SharedPtr<ConstantBuffer> m_MaterialDataBuffer = nullptr;

	// Skybox

	float m_SkyboxVertices[24] =
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f
	};

	uint32_t m_SkyboxIndices[36] =
	{
		0, 1, 2,  2, 3, 0,   // -Z
		4, 7, 6,  6, 5, 4,   // +Z
		4, 5, 1,  1, 0, 4,   // -X
		3, 2, 6,  6, 7, 3,   // +X
		4, 0, 3,  3, 7, 4,   // +Y
		1, 5, 6,  6, 2, 1    // -Y
	};

	struct alignas(16)
	{
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
	} m_CameraBufferData;

	SharedPtr<VertexArray> m_SkyboxVA = nullptr;
	SharedPtr<VertexBuffer> m_SkyboxVB = nullptr;
	SharedPtr<IndexBuffer>  m_SkyboxIB = nullptr;
	SharedPtr<Shader> m_SkyboxShader = nullptr;
	SharedPtr<ConstantBuffer> m_CameraDataBuffer = nullptr;
	SharedPtr<TextureCube> m_SkyboxTexture = nullptr;

	SharedPtr<Shader> m_EquirectToCubemapShader = nullptr;
	SharedPtr<ConstantBuffer> m_EquirectToCubeCB = nullptr;
	SharedPtr<VertexBuffer> m_CubeVB;
	SharedPtr<VertexArray> m_CubeVA;

	struct
	{
		uint32_t FaceIndex = 0;
		float padding[3];
	} m_EquirectToCubeData;


};


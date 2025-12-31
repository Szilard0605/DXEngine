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

class PBRRenderer
{
public:
	PBRRenderer(uint32_t width, uint32_t height);

	void Render(PerspectiveCamera& camera);
	void AddMesh(SharedPtr<Mesh> mesh);
	void AddPointLight(PointLight pointLight, glm::vec3 position);

	void Resize(uint32_t width, uint32_t height);
	void SetSkyboxTexture(SharedPtr<TextureCube> texture) { m_SkyboxTexture = texture; }

private:
	void GeometryPass(PerspectiveCamera& camera);
	void SkyboxPass();
private:

	SharedPtr<RenderTarget> m_MainRenderTarget = nullptr;
	PerspectiveCamera m_Camera;

	std::vector<SharedPtr<Mesh>> m_Meshes;
	std::vector<std::pair<PointLight, glm::vec3>> m_PointLights;

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

	struct SkyboxRenderData
	{
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
	} m_SkyboxRenderData;

	SharedPtr<VertexArray> m_SkyboxVA = nullptr;
	SharedPtr<VertexBuffer> m_SkyboxVB = nullptr;
	SharedPtr<IndexBuffer>  m_SkyboxIB = nullptr;
	SharedPtr<Shader> m_SkyboxShader = nullptr;
	SharedPtr<ConstantBuffer> m_SkyboxRenderDataBuffer = nullptr;
	SharedPtr<TextureCube> m_SkyboxTexture = nullptr;

};


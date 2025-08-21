#pragma once

#include <memory>
#include <stdint.h>
#include "glm.hpp"

#include "Utils/Utils.h"
#include "Math/Transform.h"
#include "Camera/PerspectiveCamera.h"
#include "Renderer/API/Shader.h"
#include "Mesh.h"
#include "Light.h"

class PBRRenderer
{
public:
	PBRRenderer(uint32_t width, uint32_t height);

	void Render(PerspectiveCamera& camera);
	void AddMesh(SharedPtr<Mesh> mesh);
	void AddPointLight(PointLight pointLight, glm::vec3 position);

	void Resize(uint32_t width, uint32_t height);

private:
	void GeometryPass(PerspectiveCamera& camera);
	void TestPass(PerspectiveCamera& camera);
private:

	SharedPtr<RenderTarget> m_MainRenderTarget = nullptr;

	std::vector<SharedPtr<Mesh>> m_Meshes;
	std::vector<std::pair<PointLight, glm::vec3>> m_PointLights;
};


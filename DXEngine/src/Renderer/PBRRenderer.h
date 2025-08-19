#pragma once

#include <memory>
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
	PBRRenderer();

	void Render(PerspectiveCamera& camera);
	void AddMesh(SharedPtr<Mesh> mesh);
	void AddPointLight(PointLight pointLight, glm::vec3 position);

private:
	std::vector<SharedPtr<Mesh>> m_Meshes;
	std::vector<std::pair<PointLight, glm::vec3>> m_PointLights;
};


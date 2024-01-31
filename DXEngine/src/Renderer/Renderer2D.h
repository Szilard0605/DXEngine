#pragma once

#include "glm.hpp"

#include "Renderer/Platform/D3D11/D3D11.h"
#include "Camera/PerspectiveCamera.h"

#include "Renderer/API/API.h"

#include "Platform/D3D11/D3D11Texture2D.h"

class Renderer2D
{
public:

	static void Init(Renderer* renderer);

	static void DrawQuad(const glm::vec3& Position, const glm::vec3& size, float rotation, const glm::vec4& color, Texture2D* texture, PerspectiveCamera& camera);
};


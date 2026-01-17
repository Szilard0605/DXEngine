#pragma once

#include "glm.hpp"

struct PointLight 
{
	glm::vec3 Color{ 1, 1, 1 };
	glm::vec3 Position {0, 0, 0};
	float Intensity{ 1.0f };

	float padding;
};

struct DirectionalLight
{
	glm::vec3 Color{ 1, 1, 1 };
	glm::vec3 Direction { -1, -1, -1 };
	float Intensity{ 1.0f };

	glm::vec2 padding;
};
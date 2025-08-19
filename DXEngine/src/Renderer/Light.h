#pragma once

#include "glm.hpp"

struct Light
{
	glm::vec3 Color {1, 1, 1};
	float Intensity = 1.0f;
};

struct PointLight : public Light
{
	glm::vec3 Position {0, 0, 0};
	float Range = 100.0f; 
};

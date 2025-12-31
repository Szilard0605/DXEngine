#pragma once

#include "glm.hpp"
#include "API/Texture2D.h"

#include <memory>

struct PBRMaterial
{
public:
	SharedPtr<Shader> Shader = nullptr;

	SharedPtr<Texture2D> m_BaseColorTexture = nullptr;
	SharedPtr<Texture2D> m_NormalTexture    = nullptr;
	SharedPtr<Texture2D> m_MetallicTexture  = nullptr;
	SharedPtr<Texture2D> m_RoughnessTexture = nullptr;
	SharedPtr<Texture2D> m_EmissiveTexture  = nullptr;

	glm::vec3 m_BaseColor = glm::vec3 { 1.0f };
	float m_Specular      = 0.5f;
	float m_Roughness     = 0.5f;
	float m_Metallic      = 0.0f;
};
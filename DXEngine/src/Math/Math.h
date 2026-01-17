#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm.hpp"
#include <gtx/norm.hpp>
#include <gtx/quaternion.hpp>

namespace Math
{
	struct Transform
	{
		glm::vec3 Position { 0, 0, 0 };
		glm::vec3 Scale { 1, 1, 1 };
		glm::vec3 Rotation { 0, 0, 0 };
		glm::mat4 GetTransformMatrix() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
}
#pragma once

#include <glm/glm.hpp>

namespace symphony
{
	struct MaterialUniforms
	{
		alignas(16) glm::vec3 Ambient = glm::vec3(1.0f);
		alignas(16) glm::vec3 Diffuse = glm::vec3(1.0f);
		alignas(16) glm::vec3 Specular = glm::vec3(1.0f);
		alignas(16) float Shininess = 1.0;
	};
}
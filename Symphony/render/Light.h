#pragma once

#include <glm/glm.hpp>

namespace symphony
{
	struct LightInformation
	{
		alignas(16) glm::vec3 CameraPosition;
		alignas(16) glm::vec3 LightPosition;
		alignas(16) glm::vec3 LightDirection;
		alignas(16) float CutOff;
		alignas(16) float OuterCutOff;

		alignas(16) glm::vec3 Ambient;
		alignas(16) glm::vec3 Diffuse;
		alignas(16) glm::vec3 Specular;

		alignas(16) float Constant;
		alignas(16) float Linear;
		alignas(16) float Quadratic;
	};
}
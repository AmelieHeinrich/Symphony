#pragma once

#include <glm/glm.hpp>

namespace symphony
{
	enum class MaterialTextureType
	{
		Albedo,
		Prefilter,
		Normal,
		Metallic,
		Roughness,
		Parallax,
		AmbientOcclusion
	};

	__declspec(align(16))
	struct MaterialUniforms
	{
		glm::vec3 Ambient = glm::vec3(1.0f);
		glm::vec3 Diffuse = glm::vec3(1.0f);
		glm::vec3 Specular = glm::vec3(1.0f);
		glm::vec3 Transmittance = glm::vec3(1.0f);
		glm::vec3 Emission = glm::vec3(1.0f);
		float Roughness = 0.0;
		float Metallic = 0.0;
		float Sheen = 0.0;
		float IOR = 0.0;
		float Dissolve = 0.0;
		float ClearcoatThickness = 0.0;
		float ClearcoatRoughness = 0.0;
		float Shininess = 1.0;
	};
}
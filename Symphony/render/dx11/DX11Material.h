#pragma once

#include <string>
#include <d3d11.h>
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "DX11Texture2D.h"
#include "DX11Shader.h"
#include "DX11UniformBuffer.h"

namespace symphony
{
	struct DX11Material
	{
		glm::vec3 Ambient = glm::vec3(1.0f);
		glm::vec3 Diffuse = glm::vec3(1.0f);
		glm::vec3 Specular = glm::vec3(1.0f);
		glm::vec3 Transmittance = glm::vec3(1.0f);
		glm::vec3 Emission = glm::vec3(1.0f);
		float Roughness = 0.0f;
		float Metallic = 0.0f;
		float Sheen = 0.0f;
		float IOR = 0.0f;
		float Dissolve = 0.0f;
		float ClearcoatThickness = 0.0f;
		float ClearcoatRoughness = 0.0f;
		float Shininess = 1.0f;

		std::shared_ptr<DX11Shader> Shader;
		std::shared_ptr<DX11UniformBuffer> UniformBuffer;

		DX11Material(const char* vertexPath, const char* fragmentPath);
		~DX11Material();

		void Bind(uint32_t materialIndex);
		void Update();
		void Unbind();
	};
}
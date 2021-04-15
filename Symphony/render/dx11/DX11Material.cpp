#include "DX11Material.h"
#include "render/Material.h"

namespace symphony
{
	DX11Material::DX11Material(const char* vertexPath, const char* fragmentPath)
	{
		Shader = std::make_shared<DX11Shader>(vertexPath, fragmentPath);
		MaterialUniforms ubo{};
		UniformBuffer = std::make_shared<DX11UniformBuffer>(&ubo, sizeof(MaterialUniforms));
	}

	DX11Material::~DX11Material()
	{
		UniformBuffer.reset();
		Shader.reset();
	}

	void DX11Material::Bind(uint32_t materialIndex)
	{
		Shader->Bind();
		UniformBuffer->BindForShader(materialIndex);
	}

	void DX11Material::Update()
	{
		MaterialUniforms ubo{};
		ubo.Ambient = Ambient;
		ubo.Diffuse = Diffuse;
		ubo.Specular = Specular;
		ubo.Shininess = Shininess;

		UniformBuffer->Update(&ubo);
	}

	void DX11Material::Unbind()
	{
		
	}
}
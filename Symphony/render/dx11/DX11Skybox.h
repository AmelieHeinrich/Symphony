#pragma once

#include <d3d11.h>
#include <string>
#include <memory>
#include "DX11Shader.h"
#include "DX11Mesh.h"
#include "DX11UniformBuffer.h"

namespace symphony
{
	class DX11Skybox
	{
	public:
		DX11Skybox(const std::string& file);
		~DX11Skybox();

		void Draw(RendererUniforms ubo);
	private:
		std::shared_ptr<DX11Shader> m_SkyboxShader;
		std::shared_ptr<DX11Mesh> m_SkyboxMesh;
		std::shared_ptr<DX11UniformBuffer> m_SkyboxUniformBuffer;
	};
}
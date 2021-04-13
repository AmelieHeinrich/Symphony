#include "DX11Skybox.h"
#include "render/ImageData.h"
#include "DX11Renderer.h"
#include <glm/glm.hpp>
#include "render/MeshBuilder.h"

namespace symphony
{
	DX11Skybox::DX11Skybox(const std::string& file)
	{
		m_SkyboxShader = std::make_shared<DX11Shader>("shaderlib/hlsl/SkyboxVertex.hlsl", "shaderlib/hlsl/SkyboxFragment.hlsl");
		m_SkyboxShader->Bind();
		m_SkyboxMesh = std::make_shared<DX11Mesh>(MeshBuilder::LoadModelData("resources/skybox/sphere.obj", file.c_str()));
		m_SkyboxUniformBuffer = std::make_shared<DX11UniformBuffer>();
		m_SkyboxShader->Unbind();
	}

	DX11Skybox::~DX11Skybox()
	{
		m_SkyboxMesh.reset();
		m_SkyboxUniformBuffer.reset();
		m_SkyboxShader.reset();
	}

	void DX11Skybox::Draw(RendererUniforms ubo)
	{
		auto device = DX11Renderer::GetRendererData().Context;
		m_SkyboxShader->Bind();
		m_SkyboxUniformBuffer->BindForShader(0);
		
		m_SkyboxUniformBuffer->Update(ubo);
		m_SkyboxMesh->Draw();
	}
}
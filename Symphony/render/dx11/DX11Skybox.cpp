#include "DX11Skybox.h"
#include "render/ImageData.h"
#include "DX11Renderer.h"
#include <glm/glm.hpp>
#include "render/MeshBuilder.h"

namespace symphony
{
	DX11Skybox::DX11Skybox(const std::string& file)
	{
		auto device = DX11Renderer::GetRendererData().Device;
		ModelData data = MeshBuilder::LoadModelData("resources/skybox/sphere.obj", file.c_str());

		m_SkyboxShader = std::make_shared<DX11Shader>("shaderlib/hlsl/SkyboxVertex.hlsl", "shaderlib/hlsl/SkyboxFragment.hlsl");
		m_SkyboxShader->Bind();

		MeshVBO = std::make_shared<DX11VertexBuffer>(data.RendererResources.first);
		MeshEBO = std::make_shared<DX11IndexBuffer>(data.RendererResources.second);
		MeshT2D = std::make_shared<DX11Texture2D>(data.TextureFilepath, DXGI_FORMAT_R32G32B32A32_FLOAT);

		RendererUniforms ubo{};
		m_SkyboxUniformBuffer = std::make_shared<DX11UniformBuffer>(&ubo, sizeof(RendererUniforms));
		m_SkyboxShader->Unbind();
	}

	DX11Skybox::~DX11Skybox()
	{
		MeshT2D.reset();
		MeshEBO.reset();
		MeshVBO.reset();

		m_SkyboxUniformBuffer.reset();
		m_SkyboxShader.reset();
	}

	void DX11Skybox::Draw(RendererUniforms ubo)
	{
		auto device = DX11Renderer::GetRendererData().Context;
		m_SkyboxShader->Bind();
		m_SkyboxUniformBuffer->BindForShader(0);

		m_SkyboxUniformBuffer->Update(&ubo);

		MeshVBO->Bind();
		MeshEBO->Bind();
		MeshT2D->Bind(0);

		device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device->DrawIndexed(MeshEBO->GetIndicesSize(), 0, 0);

		MeshVBO->Unbind();
		MeshEBO->Unbind();
		MeshT2D->Unbind(0);
	}
}
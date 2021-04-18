#include "DX12Mesh.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12Mesh::DX12Mesh(ModelData data)
		: Mesh(data)
	{
		
	}

	DX12Mesh::~DX12Mesh()
	{
		for (auto ubo : MeshUBOS)
		{
			ubo.reset();
			ubo = nullptr;
		}

		for (auto texture : MeshT2D)
		{
			texture.reset();
			texture = nullptr;
		}

		MeshEBO.reset();
		MeshVBO.reset();
	}

	void DX12Mesh::CreateResources()
	{
		MeshVBO = std::make_shared<DX12VertexBuffer>(m_Data.RendererResources.first);
		MeshEBO = std::make_shared<DX12IndexBuffer>(m_Data.RendererResources.second);

		for (auto texture : m_Data.Textures)
			MeshT2D.push_back(std::make_shared<DX12Texture2D>(texture.first.c_str()));
		
		for (int i = 0; i < 2; i++)
			MeshUBOS[i] = std::make_shared<DX12UniformBuffer>();
	}

	void DX12Mesh::Draw(RendererUniforms ubo)
	{
		auto clist = DX12Renderer::GetCurrentCommand()->GetCommandList();
		auto bufferIndex = DX12Renderer::GetRendererData().BufferIndex;

		ubo.SceneModel = ModelMatrix;

		MeshUBOS[bufferIndex]->Bind();
		MeshVBO->Bind();
		MeshEBO->Bind();
		for (auto texture : MeshT2D)
			texture->Bind();

		MeshUBOS[bufferIndex]->Update(ubo);
		clist->DrawIndexedInstanced(MeshEBO->GetIndicesSize(), 1, 0, 0, 0);
	}
}
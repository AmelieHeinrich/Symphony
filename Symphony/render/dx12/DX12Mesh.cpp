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

		MeshT2D.reset();
		MeshEBO.reset();
		MeshVBO.reset();
	}

	void DX12Mesh::CreateResources()
	{
		MeshVBO = std::make_shared<DX12VertexBuffer>(m_Data.RendererResources.first);
		MeshEBO = std::make_shared<DX12IndexBuffer>(m_Data.RendererResources.second);
		MeshT2D = std::make_shared<DX12Texture2D>(m_Data.TextureFilepath);
		
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
		MeshT2D->Bind();

		MeshUBOS[bufferIndex]->Update(ubo);
		clist->DrawIndexedInstanced(MeshEBO->GetIndicesSize(), 1, 0, 0, 0);
	}
}
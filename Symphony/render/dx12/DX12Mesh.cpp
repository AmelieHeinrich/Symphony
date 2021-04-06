#include "DX12Mesh.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12Mesh::DX12Mesh(ModelData data)
		: Mesh(data)
	{
		MeshVBO = std::make_shared<DX12VertexBuffer>(data.RendererResources.first);
		MeshEBO = std::make_shared<DX12IndexBuffer>(data.RendererResources.second);
		MeshT2D = std::make_shared<DX12Texture2D>(data.TextureFilepath);
		MeshUBO = std::make_shared<DX12UniformBuffer>();
	}

	DX12Mesh::~DX12Mesh()
	{
		MeshUBO.reset();
		MeshT2D.reset();
		MeshEBO.reset();
		MeshVBO.reset();
	}

	void DX12Mesh::Draw(RendererUniforms ubo)
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		auto command = DX12Renderer::GetRendererData().RendererCommand;
		ubo.SceneModel = ModelMatrix;

		MeshUBO->Bind();
		MeshVBO->Bind();
		MeshEBO->Bind();
		MeshT2D->Bind();

		clist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		clist->DrawIndexedInstanced(MeshEBO->GetIndicesSize(), 1, 0, 0, 0);
		MeshUBO->Update(ubo);

		MeshEBO->Unbind();
		MeshVBO->Unbind();
		MeshUBO->Unbind();
	}
}
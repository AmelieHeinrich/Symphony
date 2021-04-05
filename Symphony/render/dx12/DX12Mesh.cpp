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
	}

	DX12Mesh::~DX12Mesh()
	{
		MeshT2D.reset();
		MeshEBO.reset();
		MeshVBO.reset();
	}

	void DX12Mesh::Draw()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();

		MeshVBO->Bind();
		MeshEBO->Bind();
		MeshT2D->Bind();

		clist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		clist->DrawIndexedInstanced(MeshEBO->GetIndicesSize(), 1, 0, 0, 0);

		MeshEBO->Unbind();
		MeshVBO->Unbind();
	}
}
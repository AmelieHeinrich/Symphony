#include "DX11Mesh.h"
#include "DX11Renderer.h"

namespace symphony
{
	DX11Mesh::DX11Mesh(ModelData data)
		: Mesh(data)
	{
		MeshVBO = std::make_shared<DX11VertexBuffer>(data.RendererResources.first);
		MeshEBO = std::make_shared<DX11IndexBuffer>(data.RendererResources.second);
		MeshT2D = std::make_shared<DX11Texture2D>(data.TextureFilepath);
	}

	DX11Mesh::~DX11Mesh()
	{
		MeshT2D.reset();
		MeshEBO.reset();
		MeshVBO.reset();
	}

	void DX11Mesh::Draw()
	{
		auto context = DX11Renderer::GetRendererData().Context;

		MeshVBO->Bind();
		MeshEBO->Bind();
		MeshT2D->Bind(0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(MeshEBO->GetIndicesSize(), 0, 0);

		MeshVBO->Unbind();
		MeshEBO->Unbind();
		MeshT2D->Unbind(0);
	}
}
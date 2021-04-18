#include "DX11Mesh.h"
#include "DX11Renderer.h"

namespace symphony
{
	DX11Mesh::DX11Mesh(ModelData data)
		: Mesh(data)
	{
		MeshVBO = std::make_shared<DX11VertexBuffer>(data.RendererResources.first);
		MeshEBO = std::make_shared<DX11IndexBuffer>(data.RendererResources.second);

		for (auto texture : data.Textures)
			MeshT2D.push_back(std::make_shared<DX11Texture2D>(texture.first.c_str()));
	}

	DX11Mesh::~DX11Mesh()
	{
		for (auto texture : MeshT2D)
			texture.reset();
		MeshEBO.reset();
		MeshVBO.reset();
	}

	void DX11Mesh::Draw()
	{
		auto context = DX11Renderer::GetRendererData().Context;

		MeshVBO->Bind();
		MeshEBO->Bind();
		for (int i = 0; i < MeshT2D.size(); i++)
			MeshT2D[i]->Bind(i);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(MeshEBO->GetIndicesSize(), 0, 0);

		MeshVBO->Unbind();
		MeshEBO->Unbind();
		for (int i = 0; i < MeshT2D.size(); i++)
			MeshT2D[i]->Unbind(i);
	}
}
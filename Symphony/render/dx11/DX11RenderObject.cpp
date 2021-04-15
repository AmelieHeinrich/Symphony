#include "DX11RenderObject.h"

namespace symphony
{
	DX11RenderObject::DX11RenderObject(std::shared_ptr<DX11Mesh> mesh, std::shared_ptr<DX11Material> material)
		: mesh(mesh), material(material)
	{
		
	}

	DX11RenderObject::~DX11RenderObject()
	{
		mesh.reset();
		material.reset();
	}

	void DX11RenderObject::Update(uint32_t materialUboIndex)
	{
		material->Bind(materialUboIndex);
		material->Update();
		mesh->Draw();
	}
}
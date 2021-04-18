#pragma once

#include "DX11VertexBuffer.h"
#include "DX11IndexBuffer.h"
#include "DX11Texture2D.h"
#include <render/Mesh.h>

namespace symphony
{
	class DX11Mesh : public Mesh
	{
	public:
		DX11Mesh(ModelData data);
		~DX11Mesh();

		void Draw();
	private:
		std::shared_ptr<DX11VertexBuffer> MeshVBO;
		std::shared_ptr<DX11IndexBuffer> MeshEBO;
		std::vector<std::shared_ptr<DX11Texture2D>> MeshT2D;
	};
}
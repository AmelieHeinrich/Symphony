#pragma once

#include "DX11Mesh.h"
#include "DX11Material.h"
#include <render/Material.h>

namespace symphony
{
	class DX11RenderObject
	{
	public:
		DX11RenderObject(std::shared_ptr<DX11Mesh> mesh, std::shared_ptr<DX11Material> material);
		~DX11RenderObject();

		void Update(uint32_t materialUboIndex);

		uint32_t GetNumberOfVertices() {
			return mesh->GetNumberOfVertices();
		}

		glm::mat4 GetModelMatrix() {
			return mesh->GetModelMatrix();
		}

		std::shared_ptr<DX11Mesh> mesh;
		std::shared_ptr<DX11Material> material;
	};
}
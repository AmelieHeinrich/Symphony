#pragma once

#include "DX12VertexBuffer.h"
#include "DX12IndexBuffer.h"
#include "DX12Texture2D.h"
#include "DX12UniformBuffer.h"
#include <render/Mesh.h>

namespace symphony
{
	class DX12Mesh : public Mesh
	{
	public:
		DX12Mesh(ModelData data);
		~DX12Mesh();

		void Draw(RendererUniforms ubo);
	private:
		std::shared_ptr<DX12VertexBuffer> MeshVBO;
		std::shared_ptr<DX12IndexBuffer> MeshEBO;
		std::shared_ptr<DX12Texture2D> MeshT2D;
		std::shared_ptr<DX12UniformBuffer> MeshUBO;
	};
}
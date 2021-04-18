#pragma once

#include <glad/gl.h>
#include <render/Mesh.h>
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLTexture2D.h"
#include "core/DLLExport.h"

namespace symphony
{
	SymphonyClass GLMesh : public Mesh
	{
	public:
		GLMesh(ModelData data);
		~GLMesh();

		void Draw();
	private:
		std::shared_ptr<GLVertexBuffer> MeshVBO;
		std::shared_ptr<GLIndexBuffer> MeshEBO;
		std::vector<std::shared_ptr<GLTexture2D>> MeshT2D;
		uint32_t MeshVAO;
	};
}
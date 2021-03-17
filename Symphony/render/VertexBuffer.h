#pragma once

#include "core/DLLExport.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace symphony
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		//glm::vec2 TexCoords;
		//glm::vec3 Normals;
	};

	SymphonyClass VertexBuffer
	{
	public:
		VertexBuffer(const std::vector<Vertex>& vertices)
			: VerticesSize(vertices.size()) {};
		virtual ~VertexBuffer() = default;

		virtual void* GetVertexBufferHandle() = 0;
		virtual uint32_t GetVerticesSize() { return VerticesSize; }

	private:
		uint32_t VerticesSize;
	};
}
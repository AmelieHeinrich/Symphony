#pragma once

#include <glad/gl.h>
#include "render/VertexBuffer.h"

namespace symphony
{
	class GLVertexBuffer : public VertexBuffer
	{
	public:
		GLVertexBuffer(const std::vector<Vertex>& vertices);
		~GLVertexBuffer();

		void Bind();
		void Unbind();

		virtual void* GetVertexBufferHandle() override {
			return (void*)m_BufferHandle;
		}
	private:
		uint32_t m_BufferHandle;
	};
}
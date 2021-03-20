#pragma once

#include <glad/gl.h>
#include "render/IndexBuffer.h"

namespace symphony
{
	class GLIndexBuffer : public IndexBuffer
	{
	public:
		GLIndexBuffer(const std::vector<uint32_t>& vertices);
		~GLIndexBuffer();

		void Bind();
		void Unbind();

		virtual void* GetIndexBufferHandle() override {
			return (void*)m_BufferHandle;
		}
	private:
		uint32_t m_BufferHandle;
	};
}
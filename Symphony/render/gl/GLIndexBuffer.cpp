#include "GLIndexBuffer.h"

namespace symphony
{
	GLIndexBuffer::GLIndexBuffer(const std::vector<uint16_t>& vertices)
		: IndexBuffer(vertices)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferHandle);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(uint16_t), vertices.data(), GL_STATIC_DRAW);
	}

	GLIndexBuffer::~GLIndexBuffer()
	{
		glDeleteBuffers(1, &m_BufferHandle);
	}

	void GLIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferHandle);
	}

	void GLIndexBuffer::Unbind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
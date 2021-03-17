#include "GLVertexBuffer.h"
#include <glm/gtc/type_ptr.hpp>

namespace symphony
{
	GLVertexBuffer::GLVertexBuffer(const std::vector<Vertex>& vertices)
		: VertexBuffer(vertices)
	{
		glCreateBuffers(1, &m_BufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferHandle);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{
		glDeleteBuffers(1, &m_BufferHandle);
	}

	void GLVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferHandle);
	}

	void GLVertexBuffer::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
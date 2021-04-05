#include "GLUniformBuffer.h"
#include "render/Renderer.h"
#include <string>
#include "GLRenderer.h"

namespace symphony
{
	GLUniformBuffer::GLUniformBuffer(uint32_t program)
	{
		unsigned int uniformBlockIndex = glGetUniformBlockIndex(program, "RendererUniforms");
		glUniformBlockBinding(program, uniformBlockIndex, 0);

		glCreateBuffers(1, &m_RenderID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RenderID);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(RendererUniforms), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_RenderID, 0, sizeof(RendererUniforms));
	}

	GLUniformBuffer::~GLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RenderID);
	}

	void GLUniformBuffer::Update(RendererUniforms uniform)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RenderID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(uniform), &uniform);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void GLUniformBuffer::SetUniformSampler(uint32_t program)
	{
		
	}

	void GLUniformBuffer::SetCurrentTexture(uint32_t program, uint32_t slot)
	{
		glUniform1i(glGetUniformLocation(program, "currentTexture"), slot);
	}
}
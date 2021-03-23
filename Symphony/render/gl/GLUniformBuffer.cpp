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
		std::vector<int> gltextures;
		auto textures = GLRenderer::GetTextures();
		
		for (int i = 0; i < 10; i++) {
			if (i < textures.size()) {
				gltextures.push_back(reinterpret_cast<uint32_t>(textures[i]->GetTextureBuffer()));
			}
			else {
				gltextures.push_back(-1);
			}
		}

		/*int loc = glGetUniformLocation(program, "RendererTextures");
		glUniform1iv(loc, 10, gltextures.data());*/
	}

	void GLUniformBuffer::SetCurrentTexture(uint32_t program, uint32_t slot)
	{
		glUniform1i(glGetUniformLocation(program, "TextureSlot"), slot);
	}
}
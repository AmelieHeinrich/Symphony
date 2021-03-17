#include "GLRenderer.h"
#include "window/Window.h"

namespace symphony
{
	std::vector<std::shared_ptr<GLVertexBuffer>> GLRenderer::m_VertexBuffers;
	std::vector<std::shared_ptr<GLIndexBuffer>> GLRenderer::m_IndexBuffers;
	std::shared_ptr<GLShader> GLRenderer::m_RendererShader;
	uint32_t GLRenderer::m_RendererVAO = 0;

	void GLRenderer::Init(Window* window)
	{
		m_RendererShader = std::make_shared<GLShader>("shaderlib/glsl/Vertex.glsl", "shaderlib/glsl/Fragment.glsl");
		glCreateVertexArrays(1, &m_RendererVAO);
		glBindVertexArray(m_RendererVAO);
	}

	void GLRenderer::Prepare()
	{
		for (auto i : m_VertexBuffers) {
			i->Bind();
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)offsetof(Vertex, Color));
			glEnableVertexAttribArray(1);
			i->Unbind();
		}
	}

	void GLRenderer::Shutdown()
	{
		m_VertexBuffers.clear();
		m_IndexBuffers.clear();

		glDeleteVertexArrays(1, &m_RendererVAO);
	}

	void GLRenderer::ClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void GLRenderer::Draw()
	{
		glBindVertexArray(m_RendererVAO);
		m_RendererShader->Bind();

		if (m_IndexBuffers.empty()) 
		{
			for (auto i : m_VertexBuffers) {
				i->Bind();
				glDrawArrays(GL_TRIANGLES, 0, i->GetVerticesSize());
				i->Unbind();
			}
		}
		else
		{
			for (auto i : m_VertexBuffers) {
				for (auto j : m_IndexBuffers) {
					i->Bind();
					j->Bind();

					glDrawElements(GL_TRIANGLES, i->GetVerticesSize() * sizeof(uint16_t), GL_UNSIGNED_SHORT, (void*)0);

					j->Unbind();
					i->Unbind();
				}
			}
		}

		m_RendererShader->Unbind();
	}

	void GLRenderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{
		m_VertexBuffers.push_back(std::make_shared<GLVertexBuffer>(vertices));
	}

	void GLRenderer::AddIndexBuffer(const std::vector<uint16_t>& indices)
	{
		m_IndexBuffers.push_back(std::make_shared<GLIndexBuffer>(indices));
	}
}
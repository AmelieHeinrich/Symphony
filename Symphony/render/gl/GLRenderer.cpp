#include "GLRenderer.h"
#include "window/Window.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace symphony
{
	std::vector<std::shared_ptr<GLVertexBuffer>> GLRenderer::m_VertexBuffers;
	std::vector<std::shared_ptr<GLIndexBuffer>> GLRenderer::m_IndexBuffers;
	std::shared_ptr<GLShader> GLRenderer::m_RendererShader;
	std::shared_ptr<GLUniformBuffer> GLRenderer::m_UniformBuffer;
	uint32_t GLRenderer::m_RendererVAO = 0;

	void GLRenderer::Init(Window* window)
	{
		m_RendererShader = std::make_shared<GLShader>("shaderlib/glsl/Vertex.glsl", "shaderlib/glsl/Fragment.glsl");
		m_UniformBuffer = std::make_shared<GLUniformBuffer>((uint32_t)m_RendererShader->GetLinkedProgram());
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
		m_UniformBuffer.reset();
		m_RendererShader.reset();
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

		RendererUniforms ubo{};
		ubo.SceneModel = glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000, glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.SceneView = glm::mat4(1.0f);
		ubo.SceneProjection = glm::mat4(1.0f);

		m_UniformBuffer->Update(ubo);

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

					glDrawElements(GL_TRIANGLES, i->GetVerticesSize() * sizeof(uint16_t), GL_UNSIGNED_INT, (void*)0);

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

	void GLRenderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexBuffers.push_back(std::make_shared<GLIndexBuffer>(indices));
	}
}
#include "GLRenderer.h"
#include "window/Window.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <core/Log.h>

namespace symphony
{
	std::vector<std::shared_ptr<GLVertexBuffer>> GLRenderer::m_VertexBuffers;
	std::vector<std::shared_ptr<GLIndexBuffer>> GLRenderer::m_IndexBuffers;
	std::vector<std::shared_ptr<GLTexture2D>> GLRenderer::m_Textures;
	std::shared_ptr<GLShader> GLRenderer::m_RendererShader;
	std::shared_ptr<GLUniformBuffer> GLRenderer::m_UniformBuffer;
	uint32_t GLRenderer::m_RendererVAO = 0;

	uint32_t GLRenderer::FBWidth = 0;
	uint32_t GLRenderer::FBHeight = 0;

	static void GLDebug(unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         SY_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       SY_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          SY_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: SY_CORE_TRACE(message); return;
		}
	}

	void GLRenderer::Init(Window* window)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GLDebug, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);

		m_RendererShader = std::make_shared<GLShader>("shaderlib/glsl/Vertex.glsl", "shaderlib/glsl/Fragment.glsl");
		m_UniformBuffer = std::make_shared<GLUniformBuffer>(reinterpret_cast<uint32_t>(m_RendererShader->GetLinkedProgram()));
		glCreateVertexArrays(1, &m_RendererVAO);
		glBindVertexArray(m_RendererVAO);

		int w;
		int h;
		SDL_GetWindowSize(window->GetWindowHandle(), &w, &h);
		FBWidth = w;
		FBHeight = h;
	}

	void GLRenderer::Prepare()
	{
		m_RendererShader->Bind();
		for (auto i : m_VertexBuffers) {
			i->Bind();
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, Color));
			glEnableVertexAttribArray(1);
			//texcoords
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, TexCoords));
			glEnableVertexAttribArray(2);
			i->Unbind();
		}

		m_UniformBuffer->SetUniformSampler(reinterpret_cast<uint32_t>(m_RendererShader->GetLinkedProgram()));
		m_RendererShader->Unbind();

		glEnable(GL_DEPTH_TEST);
	}

	void GLRenderer::Shutdown()
	{
		m_UniformBuffer.reset();
		m_RendererShader.reset();
		m_VertexBuffers.clear();
		m_IndexBuffers.clear();
		m_Textures.clear();

		glDeleteVertexArrays(1, &m_RendererVAO);
	}

	void GLRenderer::ClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void GLRenderer::Draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(m_RendererVAO);
		m_RendererShader->Bind();

		for (int i = 0; i < m_Textures.size(); i++)
		{
			m_Textures[i]->Bind(i);
			m_UniformBuffer->SetCurrentTexture(reinterpret_cast<uint32_t>(m_RendererShader->GetLinkedProgram()), i);
		}

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

					glDrawElements(GL_TRIANGLES, i->GetVerticesSize() * sizeof(uint32_t), GL_UNSIGNED_INT, (void*)0);

					j->Unbind();
					i->Unbind();
				}
			}
		}


		RendererUniforms ubo{};
		ubo.SceneProjection = glm::perspective(glm::radians(45.0f), FBWidth / (float)FBHeight, 0.01f, 1000.0f);
		ubo.SceneView = glm::mat4(1.0f);
		ubo.SceneModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -50.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000.0f, glm::vec3(0.0f, -1.0f, 0.0f));

		m_UniformBuffer->Update(ubo);

		for (auto i : m_Textures) {
			i->Unbind();
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

	void GLRenderer::AddTexture2D(const char* filepath)
	{
		m_Textures.push_back(std::make_shared<GLTexture2D>(filepath));
	}
}
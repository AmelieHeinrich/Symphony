#include "GLRenderer.h"
#include "window/Window.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <core/Log.h>
#include "GLGui.h"

namespace symphony
{
	std::shared_ptr<GLShader> GLRenderer::m_RendererShader;
	std::shared_ptr<GLUniformBuffer> GLRenderer::m_UniformBuffer;
	std::unordered_map<std::string, std::shared_ptr<GLMesh>> GLRenderer::m_Meshes;

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

		int w;
		int h;
		SDL_GetWindowSize(window->GetWindowHandle(), &w, &h);
		FBWidth = w;
		FBHeight = h;
	}

	void GLRenderer::Prepare()
	{
		m_RendererShader->Bind();
		m_RendererShader->Unbind();

		glEnable(GL_DEPTH_TEST);

		GLGui::Init();
	}

	void GLRenderer::Shutdown()
	{
		GLGui::Shutdown();

		for (auto i : m_Meshes)
			i.second.reset();
		m_Meshes.clear();

		m_UniformBuffer.reset();
		m_RendererShader.reset();
	}

	void GLRenderer::Resize(uint32_t width, uint32_t height)
	{
		FBWidth = width;
		FBHeight = height;
		glViewport(0, 0, width, height);
	}

	void GLRenderer::ClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void GLRenderer::Draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_RendererShader->Bind();

		int numTris = 0;
		int drawCalls = 0;
		for (auto mesh : m_Meshes) {
			auto model = mesh.second;

			RendererUniforms ubo{};
			ubo.SceneProjection = glm::perspective(glm::radians(45.0f), FBWidth / (float)FBHeight, 0.01f, 1000.0f);
			ubo.SceneView = glm::mat4(1.0f);
			ubo.SceneModel = model->ModelMatrix;

			glUniform1i(glGetUniformLocation(reinterpret_cast<uint32_t>(m_RendererShader->GetLinkedProgram()), "currentTexture"), 0);
			m_UniformBuffer->Update(ubo);

			model->Draw();
			numTris += model->GetNumberOfVertices() / 3;
			drawCalls++;
		}
		Renderer::Stats.NumTriangles = numTris;
		Renderer::Stats.DrawCalls = drawCalls;
		numTris = 0;
		drawCalls = 0;

		m_RendererShader->Unbind();

		GLGui::BeginGUI();
		ImGui::ShowDemoWindow();
		GLGui::EndGUI();
	}

	void GLRenderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{
		
	}

	void GLRenderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{
		
	}

	void GLRenderer::AddTexture2D(const char* filepath)
	{
		
	}

	void GLRenderer::AddMesh(Mesh mesh, const std::string& name)
	{
		m_Meshes[name] = std::make_shared<GLMesh>(mesh.GetModelData());
	}

	void GLRenderer::SetMeshTransform(const std::string& meshName, const glm::mat4& transform)
	{
		m_Meshes[meshName]->ModelMatrix = transform;
	}

	void GLRenderer::PrintRendererInfo()
	{
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* version = glGetString(GL_VERSION);

		SY_CORE_INFO("GL Vendor: " + std::string((const char*)vendor));
		SY_CORE_INFO("GL Renderer: " + std::string((const char*)renderer));
		SY_CORE_INFO("GL Version: " + std::string((const char*)version));
	}
}
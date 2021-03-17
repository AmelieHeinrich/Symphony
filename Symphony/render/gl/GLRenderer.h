#pragma once

#include "render/Renderer.h"
#include "GLShader.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include <SDL.h>

namespace symphony
{
	class GLRenderer
	{
	public:
		static void Init(Window* window);
		static void Prepare();
		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a);
		static void Draw();

		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint16_t>& indices);
	private:
		static std::vector<std::shared_ptr<GLVertexBuffer>> m_VertexBuffers;
		static std::vector<std::shared_ptr<GLIndexBuffer>> m_IndexBuffers;
		static std::shared_ptr<GLShader> m_RendererShader;
		static uint32_t m_RendererVAO;
	};
}
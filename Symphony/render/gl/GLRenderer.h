#pragma once
#include <memory>

#include "render/Renderer.h"
#include "GLShader.h"
#include "GLVertexBuffer.h"
#include "GLIndexBuffer.h"
#include "GLUniformBuffer.h"
#include "GLTexture2D.h"
#include <SDL.h>
#include "core/DLLExport.h"

namespace symphony
{
	SymphonyClass GLRenderer
	{
	public:
		static void Init(Window* window);
		static void Prepare();
		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a);
		static void Draw();

		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint32_t>& indices);
		static void AddTexture2D(const char* filepath);

		static const std::vector<std::shared_ptr<GLTexture2D>>& GetTextures() {
			return m_Textures;
		}
	private:
		static std::vector<std::shared_ptr<GLVertexBuffer>> m_VertexBuffers;
		static std::vector<std::shared_ptr<GLIndexBuffer>> m_IndexBuffers;
		static std::vector<std::shared_ptr<GLTexture2D>> m_Textures;
		static std::shared_ptr<GLShader> m_RendererShader;
		static std::shared_ptr<GLUniformBuffer> m_UniformBuffer;
		static uint32_t m_RendererVAO;
	};
}
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
#include "GLMesh.h"
#include <unordered_map>

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
		static void AddMesh(Mesh mesh, const std::string& name);
		static void SetMeshTransform(const std::string& meshName, const glm::mat4& transform);
	private:
		static std::unordered_map<std::string, std::shared_ptr<GLMesh>> m_Meshes;
		static std::shared_ptr<GLShader> m_RendererShader;
		static std::shared_ptr<GLUniformBuffer> m_UniformBuffer;
		static uint32_t FBWidth;
		static uint32_t FBHeight;
	};
}
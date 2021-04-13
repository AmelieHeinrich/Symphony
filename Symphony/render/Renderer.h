#pragma once

#include "core/DLLExport.h"
#include <vector>
#include "core/PlatformDetection.h"
#include "VertexBuffer.h"
#include <glm/glm.hpp>
#include <utility>
#include <functional>

namespace symphony
{
	class Mesh;

	typedef std::pair<std::vector<Vertex>, std::vector<uint32_t>> ModelRendererResources;
	struct ModelData
	{
		ModelRendererResources RendererResources;
		const char* TextureFilepath;
	};

	class Window;

	// Renderer uniforms (nO WAY)
	struct RendererUniforms
	{
		alignas(16) glm::mat4 SceneModel = glm::mat4(1.0f);
		alignas(16) glm::mat4 SceneProjection = glm::mat4(1.0f);
		alignas(16) glm::mat4 SceneView = glm::mat4(1.0f);
	};

	// Wrapper class around all the renderers
	SymphonyClass Renderer
	{
	public:
		// Initialise the renderer (for example window context creation)
		static void Init(Window * window);

		// Preparing for rendering (for example recording command buffers)
		static void Prepare();

		// Clearing all the renderer's data
		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a);

		// Present
		static void Draw();

		static void PrintRendererInfo();
		static void PrintRendererStats();
		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint32_t>& indices);
		static void AddTexture2D(const char* filepath);
		static void AddModelData(ModelData data);
		static void AddMesh(Mesh mesh, const std::string& name);
		static void SetMeshTransform(const std::string& name, const glm::mat4& matrix);
		static void Resize(uint32_t width, uint32_t height);
		static void SetCamera(const glm::mat4& view);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t NumTriangles = 0;

			uint32_t GetTotalVertexCount() const { return NumTriangles * 3; }
			uint32_t GetTotalIndexCount() const { return NumTriangles * 6; }
		};
		static Renderer::Statistics Stats;
	private:
		static RenderAPI s_RenderAPI;
	};
}
#pragma once

#include "core/DLLExport.h"
#include <vector>
#include "VertexBuffer.h"
#include "core/PlatformDetection.h"
#include <glm/glm.hpp>

namespace symphony
{
	class Window;

	struct RendererUniforms
	{
		alignas(16) glm::mat4 SceneModel;
		alignas(16) glm::mat4 SceneProjection;
		alignas(16) glm::mat4 SceneView;
	};

	SymphonyClass Renderer
	{
	public:
		static void Init(Window * window);
		static void Prepare();
		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a);
		static void Draw();

		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint16_t>& indices);

	private:
		static RenderAPI s_RenderAPI;
	};
}
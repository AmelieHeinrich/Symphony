#pragma once

#include "render/Renderer.h"
#include <memory>
#include "DX12Device.h"
#include "DX12Fence.h"
#include "DX12Command.h"
#include "DX12Memory.h"
#include "DX12SwapChain.h"
#include "window/Window.h"

namespace symphony
{
	struct DX12RendererData
	{
		std::shared_ptr<DX12Device> RendererDevice;
		std::shared_ptr<DX12Fence> RendererFence;
		std::shared_ptr<DX12Command> RendererCommand;
		std::shared_ptr<DX12Memory> RendererMemory;
		std::shared_ptr<DX12SwapChain> RendererSwapChain;

		uint32_t BufferIndex = 0;
	};

	class DX12Renderer
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

		static DX12RendererData GetRendererData() {
			return m_RendererData;
		}
	private:
		static DX12RendererData m_RendererData;
	};
}
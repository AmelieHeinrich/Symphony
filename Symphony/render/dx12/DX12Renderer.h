#pragma once

#include "render/Renderer.h"
#include <memory>
#include "DX12Device.h"
#include "DX12Fence.h"
#include "DX12Command.h"
#include "DX12Memory.h"
#include "DX12SwapChain.h"
#include "DX12Shader.h"
#include "DX12Pipeline.h"
#include "window/Window.h"

namespace symphony
{
	struct DX12Vertex : public Vertex
	{
		static std::vector<D3D12_INPUT_ELEMENT_DESC> GetLayoutDescription()
		{
			std::vector<D3D12_INPUT_ELEMENT_DESC> desc;
			desc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			desc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			return desc;
		}
	};

	struct DX12RendererData
	{
		std::shared_ptr<DX12Device> RendererDevice;
		std::shared_ptr<DX12Fence> RendererFence;
		std::shared_ptr<DX12Command> RendererCommand;
		std::shared_ptr<DX12Memory> RendererMemory;
		std::shared_ptr<DX12SwapChain> RendererSwapChain;
		std::shared_ptr<DX12Shader> RendererShader;
		std::shared_ptr<DX12Pipeline> RendererGraphicsPipeline;

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
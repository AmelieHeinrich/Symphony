#include "DX12Renderer.h"

namespace symphony
{
	DX12RendererData DX12Renderer::m_RendererData;

	void DX12Renderer::Init(Window* window)
	{
		m_RendererData.RendererDevice = std::make_shared<DX12Device>(true);
		m_RendererData.RendererFence = std::make_shared<DX12Fence>();
		m_RendererData.RendererCommand = std::make_shared<DX12Command>();
		m_RendererData.RendererMemory = std::make_shared<DX12Memory>();
		m_RendererData.RendererSwapChain = std::make_shared<DX12SwapChain>(window);
		m_RendererData.RendererShader = std::make_shared<DX12Shader>("shaderlib/hlsl/dx12/Vertex.hlsl", "shaderlib/hlsl/dx12/Fragment.hlsl");

		DX12PipelineCreateInfo pci;
		pci.Multisampled = false;
		pci.MultisampleCount = 0;
		pci.PipelineShader = m_RendererData.RendererShader;
		m_RendererData.RendererGraphicsPipeline = std::make_shared<DX12Pipeline>(pci);
	}

	void DX12Renderer::Prepare()
	{

	}

	void DX12Renderer::Shutdown()
	{
		m_RendererData.RendererGraphicsPipeline.reset();
		m_RendererData.RendererShader.reset();
		m_RendererData.RendererSwapChain->ReleaseBackBuffers();
		m_RendererData.RendererSwapChain->ReleaseSwapChain();
		m_RendererData.RendererMemory.reset();
		m_RendererData.RendererCommand.reset();
		m_RendererData.RendererFence.reset();
		m_RendererData.RendererDevice.reset();
	}

	void DX12Renderer::ClearColor(float r, float g, float b, float a)
	{
		m_RendererData.RendererCommand->ClearColor(r, g, b, a);
	}

	void DX12Renderer::Draw()
	{
		m_RendererData.RendererCommand->CloseCommandList();
		m_RendererData.RendererCommand->ExecuteCommandList();
		m_RendererData.RendererFence->UpdateFence();
		m_RendererData.RendererCommand->SignalFence(m_RendererData.RendererFence);
		m_RendererData.RendererFence->WaitEvents();
		m_RendererData.RendererCommand->ResetCommandAllocatorAndList();
		m_RendererData.BufferIndex = (m_RendererData.BufferIndex + 1) % 2;

		m_RendererData.RendererCommand->BeginFrame(m_RendererData.BufferIndex);

		// DRAW
		m_RendererData.RendererCommand->Clear(m_RendererData.BufferIndex);
		m_RendererData.RendererGraphicsPipeline->Bind();
		// END

		m_RendererData.RendererSwapChain->Present();
		m_RendererData.RendererCommand->EndFrame(m_RendererData.BufferIndex);
	}

	void DX12Renderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{

	}

	void DX12Renderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{

	}

	void DX12Renderer::AddTexture2D(const char* filepath)
	{

	}
}
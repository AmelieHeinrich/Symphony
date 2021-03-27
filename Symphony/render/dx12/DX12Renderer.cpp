#include "DX12Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

namespace symphony
{
	DX12RendererData DX12Renderer::m_RendererData;
	std::vector<std::shared_ptr<DX12VertexBuffer>> DX12Renderer::m_VertexBuffers;
	std::vector<std::shared_ptr<DX12IndexBuffer>> DX12Renderer::m_IndexBuffers;
	std::vector<std::shared_ptr<DX12Texture2D>> DX12Renderer::m_Textures;

	void DX12Renderer::Init(Window* window)
	{
		m_RendererData.RendererDevice = std::make_shared<DX12Device>(true);
		m_RendererData.RendererFence = std::make_shared<DX12Fence>();
		m_RendererData.RendererCommand = std::make_shared<DX12Command>();
		m_RendererData.RendererMemory = std::make_shared<DX12Memory>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 2);
		m_RendererData.RendererSwapChain = std::make_shared<DX12SwapChain>(window);
	}

	void DX12Renderer::Prepare()
	{
		m_RendererData.RendererShader = std::make_shared<DX12Shader>("shaderlib/hlsl/dx12/Vertex.hlsl", "shaderlib/hlsl/dx12/Fragment.hlsl");

		DX12PipelineCreateInfo pci;
		pci.Multisampled = false;
		pci.MultisampleCount = 0;
		pci.PipelineShader = m_RendererData.RendererShader;
		m_RendererData.RendererGraphicsPipeline = std::make_shared<DX12Pipeline>(pci);

		m_RendererData.RendererUniformBuffers.resize(2);
		for (int i = 0; i < 2; i++) {
			m_RendererData.RendererUniformBuffers[i] = std::make_shared<DX12UniformBuffer>();
		}
	}

	void DX12Renderer::Shutdown()
	{
		for (auto i : m_Textures) {
			i.reset();
		}
		m_Textures.clear();

		for (auto i : m_RendererData.RendererUniformBuffers) {
			i.reset();
		}
		m_RendererData.RendererUniformBuffers.clear();

		for (auto i : m_IndexBuffers) {
			i.reset();
		}
		m_IndexBuffers.clear();

		for (auto i : m_VertexBuffers) {
			i.reset();
		}
		m_VertexBuffers.clear();

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
		D3D12_VIEWPORT view{};
		view.Width = 1280;
		view.Height = 720;
		view.MaxDepth = 1.0f;
		view.MinDepth = 0.0f;

		D3D12_RECT scissor{0};
		scissor.right = view.Width;
		scissor.bottom = view.Height;

		m_RendererData.RendererCommand->GetCommandList()->RSSetViewports(1, &view);
		m_RendererData.RendererCommand->GetCommandList()->RSSetScissorRects(1, &scissor);
		m_RendererData.RendererCommand->Clear(m_RendererData.BufferIndex);
		m_RendererData.RendererGraphicsPipeline->Bind();
		m_RendererData.RendererShader->Bind();

		RendererUniforms ubo{};
		ubo.SceneModel = glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000, glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.SceneView = glm::mat4(1.0f);
		ubo.SceneProjection = glm::mat4(1.0f);

		for (auto i : m_Textures) {
			i->Bind();
		}

		for (int i = 0; i < 2; i++) {
			m_RendererData.RendererUniformBuffers[i]->Bind();
			m_RendererData.RendererUniformBuffers[i]->Update(ubo);
		}

		uint32_t finalVertexSize = 0;
		uint32_t finalIndexSize = 0;
		for (auto i : m_VertexBuffers) {
			finalVertexSize += i->GetVerticesSize();
		}
		for (auto i : m_IndexBuffers) {
			finalIndexSize += i->GetIndicesSize();
		}

		if (m_IndexBuffers.empty()) {
			for (auto i : m_VertexBuffers) {
				i->Bind();
				m_RendererData.RendererCommand->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				m_RendererData.RendererCommand->GetCommandList()->DrawInstanced(finalVertexSize, 1, 0, 0);
			}
		}
		else {
			for (auto i : m_VertexBuffers) {
				i->Bind();
			}
			for (auto i : m_IndexBuffers) {
				i->Bind();
				m_RendererData.RendererCommand->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				m_RendererData.RendererCommand->GetCommandList()->DrawIndexedInstanced(finalIndexSize, 1, 0, 0, 0);
			}
		}
		// END

		m_RendererData.RendererSwapChain->Present();
		m_RendererData.RendererCommand->EndFrame(m_RendererData.BufferIndex);
	}

	void DX12Renderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{
		m_VertexBuffers.push_back(std::make_shared<DX12VertexBuffer>(vertices));
	}

	void DX12Renderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexBuffers.push_back(std::make_shared<DX12IndexBuffer>(indices));
	}

	void DX12Renderer::AddTexture2D(const char* filepath)
	{
		m_Textures.push_back(std::make_shared<DX12Texture2D>(filepath));
	}
}
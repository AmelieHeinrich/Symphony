#include "DX12Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include "DX12Gui.h"

namespace symphony
{
	DX12RendererData DX12Renderer::m_RendererData;
	std::unordered_map<std::string, std::shared_ptr<DX12Mesh>> DX12Renderer::m_Meshes;

	void DX12Renderer::Init(Window* window)
	{
		m_RendererData.RendererDevice = std::make_shared<DX12Device>(true);
		m_RendererData.RendererFence = std::make_shared<DX12Fence>();
		m_RendererData.RendererCommand = std::make_shared<DX12Command>();
		
		DX12HeapManager::Init();

		m_RendererData.RendererSwapChain = std::make_shared<DX12SwapChain>(window);

		int w;
		int h;
		SDL_GetWindowSize(window->GetWindowHandle(), &w, &h);

		m_RendererData.FBWidth = w;
		m_RendererData.FBHeight = h;
	}

	void DX12Renderer::Prepare()
	{
		m_RendererData.RendererShader = std::make_shared<DX12Shader>("shaderlib/hlsl/dx12/Vertex.hlsl", "shaderlib/hlsl/dx12/Fragment.hlsl");

		DX12PipelineCreateInfo pci;
		pci.Multisampled = false;
		pci.MultisampleCount = 0;
		pci.PipelineShader = m_RendererData.RendererShader;
		m_RendererData.RendererGraphicsPipeline = std::make_shared<DX12Pipeline>(pci);

		D3D12_HEAP_PROPERTIES dsHeapProperties;
		ZeroMemory(&dsHeapProperties, sizeof(&dsHeapProperties));
		dsHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsHeapProperties.CreationNodeMask = NULL;
		dsHeapProperties.VisibleNodeMask = NULL;

		D3D12_RESOURCE_DESC dsResDesc;
		ZeroMemory(&dsResDesc, sizeof(D3D12_RESOURCE_DESC));
		dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsResDesc.Alignment = 0;
		dsResDesc.Width = m_RendererData.FBWidth;
		dsResDesc.Height = m_RendererData.FBHeight;
		dsResDesc.DepthOrArraySize = 1;
		dsResDesc.MipLevels = 1;
		dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsResDesc.SampleDesc.Count = 1;
		dsResDesc.SampleDesc.Quality = 0;
		dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValueDs = {};
		ZeroMemory(&clearValueDs, sizeof(D3D12_CLEAR_VALUE));
		clearValueDs.Format = DXGI_FORMAT_D32_FLOAT;
		clearValueDs.DepthStencil.Depth = 1.0f;
		clearValueDs.DepthStencil.Stencil = 0;

		auto res = m_RendererData.RendererDevice->GetDevice()->CreateCommittedResource(
			&dsHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&dsResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValueDs,
			IID_PPV_ARGS(&m_RendererData.RendererDepthResource)
		);
		CheckIfFailed(res, "D3D12: Failed to create depth stencil view!");

		D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesk = {};
		ZeroMemory(&dsViewDesk, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
		dsViewDesk.Format = DXGI_FORMAT_D32_FLOAT;
		dsViewDesk.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsViewDesk.Flags = D3D12_DSV_FLAG_NONE;
		dsViewDesk.Texture2D.MipSlice = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleDsv = DX12HeapManager::DepthResourceHeap->GetHeapHandle();
		m_RendererData.RendererDevice->GetDevice()->CreateDepthStencilView(m_RendererData.RendererDepthResource, &dsViewDesk, heapHandleDsv);

		DX12HeapManager::InitMeshHeap();

		for (auto mesh : m_Meshes)
			mesh.second->CreateResources();

		DX12Gui::Init();
	}

	void DX12Renderer::Shutdown()
	{
		DX12Gui::Shutdown();

		for (auto i : m_Meshes)
			i.second.reset();
		m_Meshes.clear();

		m_RendererData.RendererDepthResource->Release();
		DX12HeapManager::Release();
		m_RendererData.RendererGraphicsPipeline.reset();
		m_RendererData.RendererShader.reset();
		m_RendererData.RendererSwapChain->ReleaseBackBuffers();
		m_RendererData.RendererSwapChain->ReleaseSwapChain();
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
		m_RendererData.RendererCommand->ResetCommandAllocatorAndList();
		m_RendererData.RendererCommand->BeginFrame(m_RendererData.BufferIndex);

		// DRAW
		D3D12_VIEWPORT view{};
		view.Width = m_RendererData.FBWidth;
		view.Height = m_RendererData.FBHeight;
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

		auto descriptorHeap = DX12HeapManager::ShaderHeap;
		ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap->GetDescriptorHeap() };
		m_RendererData.RendererCommand->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
		m_RendererData.RendererCommand->GetCommandList()->SetGraphicsRootDescriptorTable(1, descriptorHeap->GetGPUHandle());

		for (auto mesh : m_Meshes) {
			auto model = mesh.second;

			RendererUniforms ubo{};
			ubo.SceneProjection = glm::perspective(glm::radians(45.0f), m_RendererData.FBWidth / (float)m_RendererData.FBHeight, 0.01f, 1000.0f);
			ubo.SceneView = glm::mat4(1.0f);

			m_RendererData.RendererCommand->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			model->Draw(ubo);
		}

		DX12Gui::BeginGUI();
		ImGui::ShowDemoWindow();
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DX12Renderer::GetRendererData().RendererCommand->GetCommandList());

		m_RendererData.RendererCommand->CloseCommandList();
		m_RendererData.RendererCommand->ExecuteCommandList();

		m_RendererData.RendererCommand->EndFrame(m_RendererData.BufferIndex);
		m_RendererData.RendererSwapChain->Present();

		m_RendererData.RendererCommand->SignalFence(m_RendererData.RendererFence);
		m_RendererData.RendererFence->WaitEvents();
		m_RendererData.RendererFence->UpdateFence();
		m_RendererData.BufferIndex = (m_RendererData.BufferIndex + 1) % 2;
	}

	void DX12Renderer::Resize(uint32_t width, uint32_t height)
	{
		m_RendererData.FBWidth = width;
		m_RendererData.FBHeight = height;

		m_RendererData.RendererDepthResource->Release();
		D3D12_HEAP_PROPERTIES dsHeapProperties;
		ZeroMemory(&dsHeapProperties, sizeof(&dsHeapProperties));
		dsHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsHeapProperties.CreationNodeMask = NULL;
		dsHeapProperties.VisibleNodeMask = NULL;

		D3D12_RESOURCE_DESC dsResDesc;
		ZeroMemory(&dsResDesc, sizeof(D3D12_RESOURCE_DESC));
		dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsResDesc.Alignment = 0;
		dsResDesc.Width = m_RendererData.FBWidth;
		dsResDesc.Height = m_RendererData.FBHeight;
		dsResDesc.DepthOrArraySize = 1;
		dsResDesc.MipLevels = 1;
		dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsResDesc.SampleDesc.Count = 1;
		dsResDesc.SampleDesc.Quality = 0;
		dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValueDs = {};
		ZeroMemory(&clearValueDs, sizeof(D3D12_CLEAR_VALUE));
		clearValueDs.Format = DXGI_FORMAT_D32_FLOAT;
		clearValueDs.DepthStencil.Depth = 1.0f;
		clearValueDs.DepthStencil.Stencil = 0;

		auto res = m_RendererData.RendererDevice->GetDevice()->CreateCommittedResource(
			&dsHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&dsResDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValueDs,
			IID_PPV_ARGS(&m_RendererData.RendererDepthResource)
		);
		CheckIfFailed(res, "D3D12: Failed to create depth stencil view!");

		D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesk = {};
		ZeroMemory(&dsViewDesk, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
		dsViewDesk.Format = DXGI_FORMAT_D32_FLOAT;
		dsViewDesk.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsViewDesk.Flags = D3D12_DSV_FLAG_NONE;
		dsViewDesk.Texture2D.MipSlice = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE heapHandleDsv = DX12HeapManager::DepthResourceHeap->GetHeapHandle();
		m_RendererData.RendererDevice->GetDevice()->CreateDepthStencilView(m_RendererData.RendererDepthResource, &dsViewDesk, heapHandleDsv);
		m_RendererData.RendererSwapChain->Resize(width, height);
	}

	void DX12Renderer::PrintRendererInfo()
	{
		DXGI_ADAPTER_DESC desc{};
		m_RendererData.RendererDevice->GetAdapter()->GetDesc(&desc);

		std::wstring deviceName = desc.Description;

		SY_CORE_INFO("D3D12 Device Description: " + std::string(deviceName.begin(), deviceName.end()));
		SY_CORE_INFO("D3D12 Vendor ID: " + std::to_string(desc.VendorId));
		SY_CORE_INFO("D3D12 Device ID: " + std::to_string(desc.DeviceId));
		SY_CORE_INFO("D3D12 Sub System ID: " + std::to_string(desc.SubSysId));
		SY_CORE_INFO("D3D12 Revision: " + std::to_string(desc.Revision));
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

	void DX12Renderer::AddMesh(Mesh mesh, const std::string& name)
	{
		m_Meshes[name] = std::make_shared<DX12Mesh>(mesh.GetModelData());
	}

	void DX12Renderer::SetMeshTransform(const std::string& meshName, const glm::mat4& transform)
	{
		m_Meshes[meshName]->ModelMatrix = transform;
	}
}
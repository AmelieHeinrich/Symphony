#include "DX12VertexBuffer.h"
#include "DX12Renderer.h"
#include "DX12Device.h"
#include <wrl.h>

namespace symphony
{
	DX12VertexBuffer::DX12VertexBuffer(const std::vector<Vertex>& vertices)
		: VertexBuffer(vertices)
	{
		m_Resource = CreateDefaultBuffer(vertices, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		D3D12_VERTEX_BUFFER_VIEW controlPointsBufferView;
		controlPointsBufferView.BufferLocation = m_Resource->GetGPUVirtualAddress();
		controlPointsBufferView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));
		controlPointsBufferView.SizeInBytes = static_cast<UINT>(controlPointsBufferView.StrideInBytes * vertices.size());

		m_VertexBufferView = controlPointsBufferView;
	}

	DX12VertexBuffer::~DX12VertexBuffer()
	{
		m_Resource->Release();
	}

	void DX12VertexBuffer::Bind()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		clist->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	}

	void DX12VertexBuffer::Unbind()
	{

	}

	ID3D12Resource* DX12VertexBuffer::CreateDefaultBuffer(const std::vector<Vertex>& data, D3D12_RESOURCE_STATES finalState)
	{
		using namespace std;
		using namespace Microsoft::WRL;
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();

		UINT elementSize{ static_cast<UINT>(sizeof(Vertex)) };
		UINT bufferSize{ static_cast<UINT>(data.size() * elementSize) };

		// #1
		D3D12_HEAP_PROPERTIES heapProps;
		ZeroMemory(&heapProps, sizeof(heapProps));
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		// #2
		D3D12_RESOURCE_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = bufferSize;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		// #3
		ID3D12Resource* defaultBuffer;
		HRESULT hr{ device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&defaultBuffer)) };

		if (FAILED(hr))
		{
			throw(runtime_error{ "Error creating a default buffer." });
		}

		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		// #4
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		hr = device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf()));

		if (FAILED(hr))
		{
			throw(runtime_error{ "Error creating an upload buffer." });
		}

		// #5
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.ReleaseAndGetAddressOf()))))
		{
			throw(runtime_error{ "Error creating a command allocator." });
		}

		// #6
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.ReleaseAndGetAddressOf()))))
		{
			throw(runtime_error{ "Error creating a command list." });
		}

		// #7
		D3D12_COMMAND_QUEUE_DESC queueDesc;
		ZeroMemory(&queueDesc, sizeof(queueDesc));
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
		if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue.ReleaseAndGetAddressOf()))))
		{
			throw(runtime_error{ "Error creating a command queue." });
		}

		// #8
		void* pData;
		if (FAILED(uploadBuffer->Map(0, NULL, &pData)))
		{
			throw(runtime_error{ "Failed map intermediate resource." });
		}

		memcpy(pData, data.data(), bufferSize);
		uploadBuffer->Unmap(0, NULL);

		// #9
		commandList->CopyBufferRegion(defaultBuffer, 0, uploadBuffer.Get(), 0, bufferSize);

		// #10
		D3D12_RESOURCE_BARRIER barrierDesc;
		ZeroMemory(&barrierDesc, sizeof(barrierDesc));
		barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrierDesc.Transition.pResource = defaultBuffer;
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrierDesc.Transition.StateAfter = finalState;
		barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		commandList->ResourceBarrier(1, &barrierDesc);

		// #11
		commandList->Close();
		std::vector<ID3D12CommandList*> ppCommandLists{ commandList.Get() };
		commandQueue->ExecuteCommandLists(static_cast<UINT>(ppCommandLists.size()), ppCommandLists.data());

		// #12
		UINT64 initialValue{ 0 };
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		if (FAILED(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()))))
		{
			throw(runtime_error{ "Error creating a fence." });
		}

		// #13
		HANDLE fenceEventHandle{ CreateEvent(nullptr, FALSE, FALSE, nullptr) };
		if (fenceEventHandle == NULL)
		{
			throw(runtime_error{ "Error creating a fence event." });
		}

		// #14
		if (FAILED(commandQueue->Signal(fence.Get(), 1)))
		{
			throw(runtime_error{ "Error siganalling buffer uploaded." });
		}

		// #15
		if (FAILED(fence->SetEventOnCompletion(1, fenceEventHandle)))
		{
			throw(runtime_error{ "Failed set event on completion." });
		}

		// #16
		DWORD wait{ WaitForSingleObject(fenceEventHandle, 10000) };
		if (wait != WAIT_OBJECT_0)
		{
			throw(runtime_error{ "Failed WaitForSingleObject()." });
		}

		return defaultBuffer;
	}
}
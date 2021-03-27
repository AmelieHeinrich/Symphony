#include "DX12Texture2D.h"
#include "DX12Renderer.h"
#include "DX12Device.h"
#include "render/ImageData.h"
#include "dx12ext/d3dx12.h"
#include <wrl.h>

namespace symphony
{
	DX12Texture2D::DX12Texture2D(const char* filepath)
	{
		using namespace std;
		using namespace Microsoft::WRL;
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();

		ImageData image_Data = ImageData::LoadImageData(filepath, 2);

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = image_Data.Width;
		textureDesc.Height = image_Data.Height;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		DX12Device::ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_TextureResource)));
	
		const UINT64 textureUploadBufferSize = GetRequiredIntermediateSize(m_TextureResource, 0, 1);
		DX12Device::ThrowIfFailed(device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_TextureUploadResource)));
		
		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = image_Data.DataBuffer;
		textureData.RowPitch = 4 * textureDesc.Width; // size of all our triangle vertex data
		textureData.SlicePitch = static_cast<LONG_PTR>(4) * textureDesc.Height; // also the size of our triangle vertex data

		auto commandList = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		auto commandQueue = DX12Renderer::GetRendererData().RendererCommand->GetCommandQueue();

		UpdateSubresources(commandList, m_TextureResource, m_TextureUploadResource, 0, 0, 1, &textureData);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_TextureResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		m_TextureDescriptorHeap = std::make_shared<DX12Memory>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);
	
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(m_TextureResource, &srvDesc, m_TextureDescriptorHeap->GetHeapHandle());

		commandList->Close();
		ID3D12CommandList* ppCommandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		UINT64 initialValue{ 0 };
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		if (FAILED(device->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()))))
		{
			throw(runtime_error{ "Error creating a fence." });
		}

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
		DWORD wait{ WaitForSingleObject(fenceEventHandle, INFINITE) };
		if (wait != WAIT_OBJECT_0)
		{
			throw(runtime_error{ "Failed WaitForSingleObject()." });
		}

		ImageData::FreeImageData(image_Data);
	}

	DX12Texture2D::~DX12Texture2D()
	{
		m_TextureUploadResource->Release();
		m_TextureResource->Release();
		m_TextureDescriptorHeap.reset();
	}

	void DX12Texture2D::Bind()
	{
		auto clist = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_TextureDescriptorHeap->GetDescriptorHeap() };
		clist->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		clist->SetGraphicsRootDescriptorTable(1, m_TextureDescriptorHeap->GetGPUHandle());
	}
}
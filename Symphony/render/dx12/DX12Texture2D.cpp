#include "DX12Texture2D.h"
#include "DX12Renderer.h"
#include "DX12Device.h"
#include "render/ImageData.h"
#include "dx12ext/d3dx12.h"
#include <wrl.h>
#include "DX12HeapManager.h"

namespace symphony
{
	uint32_t DX12Texture2D::m_TextureCreationIndex = 0;

	DX12Texture2D::DX12Texture2D(const char* filepath)
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();
		auto descriptorHeap = DX12HeapManager::SamplerHeap;

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
		textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		auto res = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_TextureResource));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create Texture2D resource!");

		UINT64 textureUploadBufferSize;
		device->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureUploadBufferSize);
		res = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(textureUploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_TextureUploadResource));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create Texture2D upload buffer!");

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &image_Data.DataBuffer[0];
		textureData.RowPitch = 4 * textureDesc.Width; // size of all our triangle vertex data
		textureData.SlicePitch = textureData.RowPitch * textureDesc.Height; // also the size of our triangle vertex data

		auto commandQueue = DX12Renderer::GetRendererData().CommandQueue;
		ID3D12CommandAllocator* allocator = 0;
		ID3D12GraphicsCommandList* clist = 0;
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, 0, IID_PPV_ARGS(&clist));
		allocator->Release();

		UpdateSubresources(clist, m_TextureResource, m_TextureUploadResource, 0, 0, 1, &textureData);

		ImageData::FreeImageData(image_Data);
	
		auto heapHandle = descriptorHeap->GetHeapHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
		handle.ptr = (UINT64)heapHandle.ptr + GetAllocationByteOffset();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(m_TextureResource, &srvDesc, handle);
		m_TextureCreationIndex++;

		clist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_TextureResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		clist->Close();
		ID3D12CommandList* ppCommandLists[] = { clist };
		commandQueue->ExecuteCommandLists(1, ppCommandLists);
	}

	DX12Texture2D::~DX12Texture2D()
	{
		m_TextureUploadResource->Release();
		m_TextureResource->Release();
	}

	void DX12Texture2D::Bind()
	{
		
	}

	void DX12Texture2D::Unbind()
	{
		
	}

	UINT64 DX12Texture2D::GetAllocationByteOffset()
	{
		UINT64 uploadSize = DX12Renderer::GetRendererData().RendererDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		return uploadSize * m_TextureCreationIndex;
	}
}
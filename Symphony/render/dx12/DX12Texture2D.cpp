#include "DX12Texture2D.h"
#include "DX12Renderer.h"
#include "DX12Device.h"
#include "render/ImageData.h"
#include "dx12ext/d3dx12.h"
#include <wrl.h>
#include "DX12HeapManager.h"

namespace symphony
{
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

		auto commandList = DX12Renderer::GetRendererData().RendererCommand->GetCommandList();
		auto commandQueue = DX12Renderer::GetRendererData().RendererCommand->GetCommandQueue();

		UpdateSubresources(commandList, m_TextureResource, m_TextureUploadResource, 0, 0, 1, &textureData);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_TextureResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(m_TextureResource, &srvDesc, descriptorHeap->GetHeapHandle());

		commandList->Close();
		ID3D12CommandList* ppCommandLists[] = { commandList };
		commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		auto fence = DX12Renderer::GetRendererData().RendererFences[DX12Renderer::GetRendererData().BufferIndex]->GetFence();
		auto fenceValue = DX12Renderer::GetRendererData().RendererFences[DX12Renderer::GetRendererData().BufferIndex]->GetUIFence();

		ImageData::FreeImageData(image_Data);

		if (FAILED(commandQueue->Signal(fence, fenceValue)))
		{
			SY_CORE_ERROR("D3D12: Failed signaling buffer upload!");
		}
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
}
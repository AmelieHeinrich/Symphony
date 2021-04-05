#include "DX12Fence.h"
#include "DX12Renderer.h"

namespace symphony
{
	DX12Fence::DX12Fence()
	{
		auto device = DX12Renderer::GetRendererData().RendererDevice->GetDevice();

		auto res = device->CreateFence(uiFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create fence!");
	}

	DX12Fence::~DX12Fence()
	{
		fence->Release();
	}

	void DX12Fence::UpdateFence()
	{
		uiFence++;
	}

	void DX12Fence::WaitEvents()
	{
		if (fence->GetCompletedValue() < uiFence) {
			HANDLE hEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
			fence->SetEventOnCompletion(uiFence, hEvent);
			WaitForSingleObject(hEvent, INFINITE);
			CloseHandle(hEvent);
		}
	}
}
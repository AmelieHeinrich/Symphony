#pragma once

#include <d3d12.h>

namespace symphony
{
	class DX12Fence
	{
	public:
		DX12Fence();
		~DX12Fence();

		void UpdateFence();
		void WaitEvents();

		ID3D12Fence* GetFence() {
			return fence;
		}

		UINT GetUIFence() {
			return uiFence;
		}
	private:
		ID3D12Fence* fence;
		UINT uiFence;
	};
}
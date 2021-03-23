#pragma once

#include <d3d12.h>
#include <dxgi.h>

namespace symphony
{
	class DX12Device
	{
	public:
		DX12Device(bool enableDebug);
		~DX12Device();

		static void ThrowIfFailed(HRESULT hr);

		UINT GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE type);

		IDXGIFactory* GetFactory() {
			return factory;
		}

		ID3D12Device* GetDevice() {
			return device;
		}

		ID3D12Debug* GetDebug() {
			if (DebugEnabled)
				return debug;
			else
				return nullptr;
		}

		ID3D12DebugDevice* GetDebugDevice() {
			if (DebugEnabled)
				return debugDevice;
			else
				return nullptr;
		}
	private:
		IDXGIFactory* factory = nullptr;
		ID3D12Device* device = nullptr;

		ID3D12Debug* debug = nullptr;
		ID3D12DebugDevice* debugDevice = nullptr;

		float DebugEnabled = true;
	};
}

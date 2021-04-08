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

		IDXGIAdapter* GetAdapter() {
			return adapter;
		}

		ID3D12Debug1* GetDebug() {
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
		IDXGIDevice* dxgiDevice = nullptr;
		IDXGIFactory* factory = nullptr;
		IDXGIAdapter* adapter = nullptr;
		ID3D12Device* device = nullptr;

		ID3D12Debug1* debug = nullptr;
		ID3D12DebugDevice* debugDevice = nullptr;

		float DebugEnabled = true;
	};
}

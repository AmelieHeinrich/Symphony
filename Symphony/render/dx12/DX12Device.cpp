#include "DX12Device.h"
#include <stdexcept>

namespace symphony
{
	DX12Device::DX12Device(bool enableDebug)
		: DebugEnabled(enableDebug)
	{
		if (DebugEnabled)
		{
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
			debug->EnableDebugLayer();
			debug->SetEnableGPUBasedValidation(true);
		}

		ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&factory)));
		ThrowIfFailed(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));

		if (DebugEnabled)
		{
			ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&debugDevice)));
		}
	}

	DX12Device::~DX12Device()
	{
		device->Release();
		factory->Release();

		if (DebugEnabled)
		{
			debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_IGNORE_INTERNAL | D3D12_RLDO_DETAIL);
			debugDevice->Release();
			debug->Release();
		}
	}

	void DX12Device::ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}

	UINT DX12Device::GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		return device->GetDescriptorHandleIncrementSize(type);
	}
}
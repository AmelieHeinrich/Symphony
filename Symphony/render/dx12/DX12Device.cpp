#include "DX12Device.h"
#include <stdexcept>
#include "DX12Renderer.h"

namespace symphony
{
	DX12Device::DX12Device(bool enableDebug)
		: DebugEnabled(enableDebug)
	{
		if (DebugEnabled)
		{
			auto res = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
			DX12Renderer::CheckIfFailed(res, "D3D12: Failed to acquire debug interface!");
			debug->EnableDebugLayer();
			debug->SetEnableGPUBasedValidation(true);
		}

		auto res = CreateDXGIFactory(IID_PPV_ARGS(&factory));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create DXGI factory!");

		factory->EnumAdapters(1, &adapter);

		res = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create device!");

		if (DebugEnabled)
		{
			res = device->QueryInterface(IID_PPV_ARGS(&debugDevice));
			DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create debug device!");
		}

		adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
	}

	DX12Device::~DX12Device()
	{
		device->Release();
		factory->Release();
		adapter->Release();

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
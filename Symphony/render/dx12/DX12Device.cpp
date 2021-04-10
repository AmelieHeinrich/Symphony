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

		factory->EnumAdapters(0, &adapter);

		res = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
		DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create device!");

		if (DebugEnabled)
		{
			res = device->QueryInterface(IID_PPV_ARGS(&debugDevice));
			DX12Renderer::CheckIfFailed(res, "D3D12: Failed to create debug device!");
		}

		adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);


		
		{
			ID3D12InfoQueue* infoQueue = 0;
			device->QueryInterface(IID_PPV_ARGS(&infoQueue));

			// Break on severities
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			// Suppress Messages

			D3D12_MESSAGE_SEVERITY supressSeverities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			D3D12_MESSAGE_ID supressIds[] =
			{
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
			};

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumSeverities = ARRAYSIZE(supressSeverities);
			filter.DenyList.pSeverityList = supressSeverities;
			filter.DenyList.NumIDs = ARRAYSIZE(supressIds);
			filter.DenyList.pIDList = supressIds;

			infoQueue->PushStorageFilter(&filter);

			infoQueue->Release();
		}
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
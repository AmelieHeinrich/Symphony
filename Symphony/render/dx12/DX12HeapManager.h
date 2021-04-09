#pragma once

#include "DX12Memory.h"
#include <memory>
#include "core/DLLExport.h"

namespace symphony
{
	SymphonyClass DX12HeapManager
	{
	public:
		static void Init();
		static void InitMeshHeap();
		static void Release();

		static std::shared_ptr<DX12Memory> RenderTargetViewHeap;
		static std::shared_ptr<DX12Memory> DepthResourceHeap;
		static std::shared_ptr<DX12Memory> ConstantBufferHeap;
		static std::shared_ptr<DX12Memory> SamplerHeap;
		static std::shared_ptr<DX12Memory> ImGuiHeap;
	};
}
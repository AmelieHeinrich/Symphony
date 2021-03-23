#pragma once

#include "core/DLLExport.h"
#include <vector>
#include <cstdint>

namespace symphony
{
	// Mother class
	SymphonyClass IndexBuffer
	{
	public:
		IndexBuffer(const std::vector<uint32_t>& indices)
			: IndicesSize(static_cast<uint32_t>(indices.size())) {}
		virtual ~IndexBuffer() = default;

		virtual void* GetIndexBufferHandle() = 0;
		virtual uint32_t GetIndicesSize() { return IndicesSize; }
	private:
		uint32_t IndicesSize;
	};
}
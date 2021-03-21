#pragma once

#include "core/DLLExport.h"
#include <vector>
#include <cstdint>

namespace symphony
{
	// Mother class
	class IndexBuffer
	{
	public:
		IndexBuffer(const std::vector<uint32_t>& indices)
			: IndicesSize(indices.size()) {}
		virtual ~IndexBuffer() = default;

		virtual void* GetIndexBufferHandle() = 0;
		virtual uint32_t GetIndicesSize() { return IndicesSize; }
	private:
		uint32_t IndicesSize;
	};
}
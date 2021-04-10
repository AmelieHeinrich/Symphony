#pragma once

#include "render/Texture.h"
#include <d3d12.h>
#include "DX12Memory.h"
#include <memory>

namespace symphony
{
	class DX12Texture2D : public Texture
	{
	public:
		DX12Texture2D(const char* filepath);
		~DX12Texture2D();

		virtual void* GetTextureBuffer() override {
			return m_TextureResource;
		}

		void Bind();
		void Unbind();
	private:
		UINT64 GetAllocationByteOffset();

		ID3D12Resource* m_TextureResource;
		ID3D12Resource* m_TextureUploadResource;
		static uint32_t m_TextureCreationIndex;
	};
}

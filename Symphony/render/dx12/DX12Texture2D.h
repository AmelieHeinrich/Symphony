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

		std::shared_ptr<DX12Memory> GetDescriptorHeap() {
			return m_TextureDescriptorHeap;
		}

		void Bind();
		void Unbind();
	private:
		ID3D12Resource* m_TextureResource;
		ID3D12Resource* m_TextureUploadResource;
		std::shared_ptr<DX12Memory> m_TextureDescriptorHeap;
	};
}

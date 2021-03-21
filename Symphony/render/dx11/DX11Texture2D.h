#pragma once

#include "render/Texture.h"
#include <d3d11.h>

namespace symphony
{
	class DX11Texture2D : public Texture
	{
	public:
		DX11Texture2D(const char* filepath);
		~DX11Texture2D();

		void Bind(int slot);
		void Unbind(int slot);

		virtual void* GetTextureBuffer() override {
			return m_Texture;
		}

		ID3D11ShaderResourceView* GetResourceView() {
			return m_ShaderResourceView;
		}

		ID3D11SamplerState* GetSamplerState() {
			return m_SamplerState;
		}
	private:
		ID3D11Texture2D* m_Texture;
		ID3D11SamplerState* m_SamplerState;
		ID3D11ShaderResourceView* m_ShaderResourceView;
	};
}

#pragma once

#include <d3d11.h>
#include <cstdint>
#include <glm/glm.hpp>

namespace symphony
{
	class DX11RenderTexture
	{
	public:
		DX11RenderTexture(uint32_t width, uint32_t height);
		~DX11RenderTexture();

		void Clear(const glm::vec4& color);
		void EnsureSize(uint32_t width, uint32_t height);
		void Resize(uint32_t width, uint32_t height);

		void Bind();
		void Unbind();

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

		ID3D11Texture2D* GetTexture2D() {
			return m_Texture;
		}

		ID3D11RenderTargetView* GetRenderTargetView() {
			return m_RenderTargetView;
		}

		ID3D11DepthStencilView* GetDepthStencilView() {
			return m_DepthStencilView;
		}
	private:
		void LoadBuffers(uint32_t width, uint32_t height);

		uint32_t m_Width, m_Height;
		ID3D11Texture2D* m_Texture;
		ID3D11Texture2D* m_DepthStencilBuffer;
		ID3D11DepthStencilView* m_DepthStencilView;
		ID3D11DepthStencilState* m_DepthStencilState;
		ID3D11RenderTargetView* m_RenderTargetView;
	};
}
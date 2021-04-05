#pragma once

#include "DX11SwapChain.h"
#include "DX11RenderContext.h"
#include "render/Renderer.h"
#include "DX11Shader.h"
#include "DX11VertexBuffer.h"
#include "DX11IndexBuffer.h"
#include "DX11UniformBuffer.h"
#include "DX11Texture2D.h"
#include <memory>
#include <core/Log.h>

namespace symphony
{
	struct DirectXRendererData
	{
		ID3D11Device* Device = nullptr;
		ID3D11DeviceContext* Context = nullptr;
		D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

		IDXGIDevice* DXGIDevice = nullptr;
		IDXGIAdapter* DXGIAdapter = nullptr;
		IDXGIFactory* DXGIFactory = nullptr;

		std::shared_ptr<DX11SwapChain> RendererSwapChain = nullptr;
		std::shared_ptr<DX11RenderContext> RendererContext = nullptr;

		// Clear color values
		float CCR = 0.0f;
		float CCG = 0.0f;
		float CCB = 0.0f;
		float CCA = 1.0f;

		uint32_t FBWidth;
		uint32_t FBHeight;
	};

	class DX11Renderer : public Renderer
	{
	public:
		static void Init(Window* window);
		static void Prepare();
		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a);
		static void Draw();

		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint32_t>& indices);
		static void AddTexture2D(const char* filepath);

		static DirectXRendererData GetRendererData() {
			return m_RendererData;
		}

		static void CheckIfFailed(HRESULT res, const char* msg)
		{
			if (FAILED(res))
			{
				SY_CORE_ERROR(msg);
			}
		}
	private:
		static DirectXRendererData m_RendererData;
		static std::vector<std::shared_ptr<DX11VertexBuffer>> m_VertexBuffers;
		static std::vector<std::shared_ptr<DX11IndexBuffer>> m_IndexBuffers;
		static std::vector<std::shared_ptr<DX11Texture2D>> m_Textures;
		static std::shared_ptr<DX11Shader> RendererShader;
		static std::shared_ptr<DX11UniformBuffer> RendererUniformBuffer;
	};
}
#pragma once

#include "DX11SwapChain.h"
#include "DX11RenderContext.h"
#include "render/Renderer.h"
#include "DX11Shader.h"
#include "DX11VertexBuffer.h"
#include "DX11IndexBuffer.h"
#include "DX11UniformBuffer.h"
#include <memory>

namespace symphony
{
	struct DirectXRendererData
	{
		ID3D11Device* Device;
		ID3D11DeviceContext* Context;
		D3D_FEATURE_LEVEL FeatureLevel;

		IDXGIDevice* DXGIDevice;
		IDXGIAdapter* DXGIAdapter;
		IDXGIFactory* DXGIFactory;

		std::shared_ptr<DX11SwapChain> RendererSwapChain;
		std::shared_ptr<DX11RenderContext> RendererContext;

		float CCR = 0.0f;
		float CCG = 0.0f;
		float CCB = 0.0f;
		float CCA = 1.0f;
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
		static void AddIndexBuffer(const std::vector<uint16_t>& indices);

		static DirectXRendererData GetRendererData() {
			return m_RendererData;
		}
	private:
		static DirectXRendererData m_RendererData;
		static std::vector<std::shared_ptr<DX11VertexBuffer>> m_VertexBuffers;
		static std::vector<std::shared_ptr<DX11IndexBuffer>> m_IndexBuffers;
		static std::shared_ptr<DX11Shader> RendererShader;
		static std::shared_ptr<DX11UniformBuffer> RendererUniformBuffer;
	};
}
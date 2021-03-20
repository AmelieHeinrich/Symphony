#include "DX11Renderer.h"
#include <wrl.h>
#include <SDL_syswm.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "window/Window.h"
#include <SDL.h>

namespace symphony
{
	DirectXRendererData DX11Renderer::m_RendererData;
	std::vector<std::shared_ptr<DX11VertexBuffer>> DX11Renderer::m_VertexBuffers;
	std::vector<std::shared_ptr<DX11IndexBuffer>> DX11Renderer::m_IndexBuffers;
	std::shared_ptr<DX11Shader> DX11Renderer::RendererShader;
	std::shared_ptr<DX11UniformBuffer> DX11Renderer::RendererUniformBuffer;

	void DX11Renderer::Init(Window* window)
	{
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window->GetWindowHandle(), &wmInfo);
		HWND windowRaw = wmInfo.info.win.window;

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		uint32_t driverSize = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL levels[] =
		{
			D3D_FEATURE_LEVEL_11_0
		};
		uint32_t levelSize = ARRAYSIZE(levels);
		HRESULT success = 0;

		for (int i = 0; i < driverSize;)
		{
			success = D3D11CreateDevice(NULL, driverTypes[i], NULL, NULL, levels, levelSize, D3D11_SDK_VERSION, &m_RendererData.Device, &m_RendererData.FeatureLevel, &m_RendererData.Context);

			if (SUCCEEDED(success))
				break;
			++i;
		}

		if (FAILED(success))
		{
			__debugbreak();
		}

		m_RendererData.RendererContext = std::make_shared<DX11RenderContext>(m_RendererData.Context);

		m_RendererData.Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_RendererData.DXGIDevice);
		m_RendererData.DXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&m_RendererData.DXGIAdapter);
		m_RendererData.DXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_RendererData.DXGIFactory);

		int w, h;
		SDL_GetWindowSize(window->GetWindowHandle(), &w, &h);
		m_RendererData.RendererSwapChain = std::make_shared<DX11SwapChain>(windowRaw, w, h);

		RendererShader = std::make_shared<DX11Shader>("shaderlib/hlsl/Vertex.hlsl", "shaderlib/hlsl/Fragment.hlsl");
		RendererUniformBuffer = std::make_shared<DX11UniformBuffer>();
	}

	void DX11Renderer::Prepare()
	{
		
	}

	void DX11Renderer::Shutdown()
	{
		for (auto i : m_IndexBuffers) {
			i.reset();
		}
		m_IndexBuffers.clear();

		for (auto i : m_VertexBuffers) {
			i.reset();
		}
		m_VertexBuffers.clear();

		RendererUniformBuffer.reset();
		RendererShader.reset();

		m_RendererData.RendererSwapChain.reset();
		m_RendererData.DXGIDevice->Release();
		m_RendererData.DXGIAdapter->Release();
		m_RendererData.DXGIFactory->Release();
		m_RendererData.RendererContext.reset();
		m_RendererData.Device->Release();
	}

	void DX11Renderer::ClearColor(float r, float g, float b, float a)
	{
		m_RendererData.CCR = r;
		m_RendererData.CCG = g;
		m_RendererData.CCB = b;
		m_RendererData.CCA = a;
	}

	void DX11Renderer::Draw()
	{
		m_RendererData.RendererContext->SetClearColor(m_RendererData.RendererSwapChain, m_RendererData.CCR, m_RendererData.CCG, m_RendererData.CCB, m_RendererData.CCA);
		m_RendererData.Context->ClearDepthStencilView(m_RendererData.RendererSwapChain->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1, 0);

		RendererShader->Bind();
		RendererUniformBuffer->BindForShader(0);

		uint32_t finalVertexSize = 0;
		uint32_t finalIndexCount = 0;

		for (auto i : m_VertexBuffers) {
			finalVertexSize += i->GetVerticesSize();
		}

		for (auto i : m_IndexBuffers) {
			finalIndexCount += i->GetIndicesSize();
		}

		if (m_IndexBuffers.empty()) {
			for (auto i : m_VertexBuffers) {
				i->Bind();
			}
			m_RendererData.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			m_RendererData.Context->Draw(finalVertexSize, 0);
		}
		else {
			for (auto i : m_VertexBuffers) {
				i->Bind();
			}
			for (auto i : m_IndexBuffers) {
				i->Bind();
			}

			m_RendererData.Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_RendererData.Context->DrawIndexed(finalIndexCount, 0, 0);
		}

		RendererUniforms ubo{};
		ubo.SceneModel = glm::rotate(glm::mat4(1.0f), (float)SDL_GetTicks() / 1000, glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.SceneView = glm::mat4(1.0f);
		ubo.SceneProjection = glm::mat4(1.0f);

		RendererUniformBuffer->Update(ubo);

		RendererShader->Unbind();

		m_RendererData.RendererSwapChain->Present();
	}

	void DX11Renderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{
		RendererShader->Bind();
		m_VertexBuffers.push_back(std::make_shared<DX11VertexBuffer>(vertices));
		RendererShader->Unbind();
	}

	void DX11Renderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexBuffers.push_back(std::make_shared<DX11IndexBuffer>(indices));
	}
}
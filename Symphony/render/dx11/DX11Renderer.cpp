#include "DX11Renderer.h"
#include <wrl.h>
#include <SDL_syswm.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "window/Window.h"
#include <SDL.h>
#include "DX11Gui.h"
#include <core/Application.h>

namespace symphony
{
	DirectXRendererData DX11Renderer::m_RendererData;
	std::unordered_map<std::string, std::shared_ptr<DX11Mesh>> DX11Renderer::m_Meshes;
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

		DX11Renderer::CheckIfFailed(success, "D3D11: Failed to initialise D3D11 Context!");

		m_RendererData.RendererContext = std::make_shared<DX11RenderContext>(m_RendererData.Context);

		m_RendererData.Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_RendererData.DXGIDevice);
		m_RendererData.DXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&m_RendererData.DXGIAdapter);
		m_RendererData.DXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_RendererData.DXGIFactory);

		int w, h;
		SDL_GetWindowSize(window->GetWindowHandle(), &w, &h);
		m_RendererData.RendererSwapChain = std::make_shared<DX11SwapChain>(windowRaw, w, h);

		RendererShader = std::make_shared<DX11Shader>("shaderlib/hlsl/Vertex.hlsl", "shaderlib/hlsl/Fragment.hlsl");
		RendererUniformBuffer = std::make_shared<DX11UniformBuffer>();

		m_RendererData.FBWidth = w;
		m_RendererData.FBHeight = h;

		DX11Gui::Init();

		m_RendererData.RendererSkybox = std::make_shared<DX11Skybox>("resources/skybox/skybox.hdr");

		RendererShader->Bind();
	}

	void DX11Renderer::Prepare()
	{
		
	}

	void DX11Renderer::Shutdown()
	{
		DX11Gui::Shutdown();

		RendererShader->Unbind();

		for (auto i : m_Meshes)
			i.second.reset();
		m_Meshes.clear();

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

		// MODEL PASS
		{
			RendererShader->Bind();

			RendererUniformBuffer->BindForShader(0);

			int numTris = 0;
			int drawCalls = 0;
			for (auto mesh : m_Meshes) {
				auto model = mesh.second;

				RendererUniforms ubo{};
				ubo.SceneProjection = glm::perspective(glm::radians(45.0f), m_RendererData.FBWidth / (float)m_RendererData.FBHeight, 0.01f, 1000.0f);
				if (!m_RendererData.CustomCamera)
				{
					ubo.SceneView = glm::mat4(1.0f);
				}
				else
				{
					ubo.SceneView = m_RendererData.View;
				}
				ubo.SceneModel = model->GetModelMatrix();
				RendererUniformBuffer->Update(ubo);

				numTris += model->GetNumberOfVertices() / 3;
				drawCalls++;

				model->Draw();
			}
			Renderer::Stats.NumTriangles = numTris;
			Renderer::Stats.DrawCalls = drawCalls;
			numTris = 0;
			drawCalls = 0;
		}

		// SKYBOX PASS
		{
			RendererUniforms ubo{};
			ubo.SceneProjection = glm::perspective(glm::radians(45.0f), m_RendererData.FBWidth / (float)m_RendererData.FBHeight, 0.01f, 100.0f);
			if (!m_RendererData.CustomCamera)
			{
				ubo.SceneView = glm::mat4(1.0f);
			}
			else
			{
				ubo.SceneView = glm::mat4(glm::mat3(m_RendererData.View));
			}
			ubo.SceneModel = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));
			m_RendererData.RendererSkybox->Draw(ubo);
		}

		// GUI PASS
		{
			DX11Gui::BeginGUI();
			ImGui::ShowDemoWindow();
			DX11Gui::EndGUI();
		}

		m_RendererData.RendererSwapChain->Present();
	}

	void DX11Renderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{
		
	}

	void DX11Renderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{
		
	}

	void DX11Renderer::AddTexture2D(const char* filepath)
	{
		
	}

	void DX11Renderer::AddMesh(Mesh mesh, const std::string& name)
	{
		m_Meshes[name] = std::make_shared<DX11Mesh>(mesh.GetModelData());
	}

	void DX11Renderer::SetMeshTransform(const std::string& meshName, const glm::mat4& transform)
	{
		m_Meshes[meshName]->ModelMatrix = transform;
	}

	void DX11Renderer::Resize(uint32_t width, uint32_t height)
	{
		m_RendererData.FBWidth = width;
		m_RendererData.FBHeight = height;
		m_RendererData.RendererSwapChain->RecreateRenderTargetView(width, height);
		Draw();
	}

	void DX11Renderer::SetCamera(const glm::mat4& view)
	{
		m_RendererData.View = view;

		m_RendererData.CustomCamera = true;
	}

	void DX11Renderer::PrintRendererInfo()
	{
		DXGI_ADAPTER_DESC desc{};
		m_RendererData.DXGIAdapter->GetDesc(&desc);

		std::wstring deviceName = desc.Description;

		SY_CORE_INFO("D3D11 Device Description: " + std::string(deviceName.begin(), deviceName.end()));
		SY_CORE_INFO("D3D11 Vendor ID: " + std::to_string(desc.VendorId));
		SY_CORE_INFO("D3D11 Device ID: " + std::to_string(desc.DeviceId));
		SY_CORE_INFO("D3D11 Sub System ID: " + std::to_string(desc.SubSysId));
		SY_CORE_INFO("D3D11 Revision: " + std::to_string(desc.Revision));
	}
}
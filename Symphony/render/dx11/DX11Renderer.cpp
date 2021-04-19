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
	std::unordered_map<std::string, std::shared_ptr<DX11RenderObject>> DX11Renderer::m_Meshes;
	std::shared_ptr<DX11Shader> DX11Renderer::RendererShader;

	std::shared_ptr<DX11UniformBuffer> DX11Renderer::RendererUniformBuffer;
	std::shared_ptr<DX11UniformBuffer> DX11Renderer::RendererLightUniformBuffer;

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

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)w;
		vp.Height = (FLOAT)h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		DX11Renderer::GetRendererData().Context->RSSetViewports(1, &vp);

		m_RendererData.FBWidth = w;
		m_RendererData.FBHeight = h;

		m_RendererData.RendererSwapChain = std::make_shared<DX11SwapChain>(windowRaw, w, h);
		m_RendererData.CurrentFramebuffer = std::make_shared<DX11RenderTexture>(w, h);

		RasterizerLibrary::InitDefaultRasterizers();

		RendererShader = std::make_shared<DX11Shader>("shaderlib/hlsl/Vertex.hlsl", "shaderlib/hlsl/Fragment.hlsl");

		RendererUniforms ubo{};
		m_RendererData.LightInfo = {};

		RendererUniformBuffer = std::make_shared<DX11UniformBuffer>(&ubo, sizeof(RendererUniforms));
		RendererLightUniformBuffer = std::make_shared<DX11UniformBuffer>(&m_RendererData.LightInfo, sizeof(LightInformation));

		DX11Gui::Init();

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

		RendererLightUniformBuffer.reset();
		RendererUniformBuffer.reset();
		RendererShader.reset();

		RasterizerLibrary::Shutdown();
		m_RendererData.CurrentFramebuffer.reset();
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
		m_RendererData.CurrentFramebuffer->Bind();
		m_RendererData.CurrentFramebuffer->Clear(glm::vec4{ m_RendererData.CCR, m_RendererData.CCG, m_RendererData.CCB, m_RendererData.CCA });

		// SKYBOX PASS
		{
			RasterizerLibrary::SetRasterizerState("Skybox State");
			RendererUniforms ubo{};
			if (!m_RendererData.CustomCamera)
			{
				ubo.SceneView = glm::mat4(1.0f);
				ubo.SceneProjection = glm::perspective(glm::radians(45.0f), m_RendererData.FBWidth / (float)m_RendererData.FBHeight, 0.01f, 100.0f);
			}
			else
			{
				ubo.SceneView = glm::mat4(glm::mat3(m_RendererData.View));
				ubo.SceneProjection = m_RendererData.Projection;
			}
			ubo.SceneModel = glm::scale(glm::mat4(1.0f), glm::vec3(10000.0f, 10000.0f, 10000.0f));
			m_RendererData.RendererSkybox->Draw(ubo);
		}

		// MODEL PASS
		{
			RasterizerLibrary::SetRasterizerState("Mesh State");

			RendererShader->Bind();

			RendererUniformBuffer->BindForShader(0);
			RendererLightUniformBuffer->BindForShader(1);

			int numTris = 0;
			int drawCalls = 0;
			for (auto mesh : m_Meshes) {
				auto model = mesh.second;

				RendererUniforms ubo{};
				if (!m_RendererData.CustomCamera)
				{
					ubo.SceneView = glm::mat4(1.0f);
					ubo.SceneProjection = glm::perspective(glm::radians(45.0f), m_RendererData.FBWidth / (float)m_RendererData.FBHeight, 0.01f, 100.0f);
				}
				else
				{
					ubo.SceneView = m_RendererData.View;
					ubo.SceneProjection = m_RendererData.Projection;
				}
				ubo.SceneModel = model->GetModelMatrix();
				RendererLightUniformBuffer->Update(&m_RendererData.LightInfo);
				RendererUniformBuffer->Update(&ubo);

				numTris += model->GetNumberOfVertices() / 3;
				drawCalls++;

				model->Update(2);
			}
			Renderer::Stats.NumTriangles = numTris;
			Renderer::Stats.DrawCalls = drawCalls;
			numTris = 0;
			drawCalls = 0;
		}

		m_RendererData.CurrentFramebuffer->Unbind();

		m_RendererData.RendererSwapChain->Bind();
		m_RendererData.RendererContext->SetClearColor(m_RendererData.RendererSwapChain, 0, 0, 0, 1);
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
		
	}

	void DX11Renderer::AddRenderObject(Mesh mesh, MaterialUniforms ubo, const std::string& name)
	{
		m_Meshes[name] = std::make_shared<DX11RenderObject>(std::make_shared<DX11Mesh>(mesh.GetModelData()), std::make_shared<DX11Material>("shaderlib/hlsl/Vertex.hlsl", "shaderlib/hlsl/Fragment.hlsl"));
		m_Meshes[name]->material->Ambient = ubo.Ambient;
		m_Meshes[name]->material->Diffuse = ubo.Diffuse;
		m_Meshes[name]->material->Specular = ubo.Specular;
		m_Meshes[name]->material->Transmittance = ubo.Transmittance;
		m_Meshes[name]->material->Emission = ubo.Emission;
		m_Meshes[name]->material->Roughness = ubo.Roughness;
		m_Meshes[name]->material->Metallic = ubo.Metallic;
		m_Meshes[name]->material->Sheen = ubo.Sheen;
		m_Meshes[name]->material->IOR = ubo.IOR;
		m_Meshes[name]->material->Dissolve = ubo.Dissolve;
		m_Meshes[name]->material->ClearcoatThickness = ubo.ClearcoatThickness;
		m_Meshes[name]->material->ClearcoatRoughness = ubo.ClearcoatRoughness;
		m_Meshes[name]->material->Shininess = ubo.Shininess;
	}

	void DX11Renderer::SetMeshTransform(const std::string& meshName, const glm::mat4& transform)
	{
		m_Meshes[meshName]->mesh->ModelMatrix = transform;
	}

	void DX11Renderer::Resize(uint32_t width, uint32_t height)
	{
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(Application::Get().GetWindow().GetWindowHandle() , &wmInfo);
		HWND windowRaw = wmInfo.info.win.window;

		m_RendererData.FBWidth = width;
		m_RendererData.FBHeight = height;

		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)width;
		vp.Height = (FLOAT)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		DX11Renderer::GetRendererData().Context->RSSetViewports(1, &vp);

		m_RendererData.RendererSwapChain->RecreateRenderTargetView(width, height);
		m_RendererData.CurrentFramebuffer->Resize(width, height);
	}

	void DX11Renderer::SetCamera(const glm::mat4& view, const glm::mat4& projection)
	{
		m_RendererData.View = view;
		m_RendererData.Projection = projection;

		m_RendererData.CustomCamera = true;
	}

	void DX11Renderer::SetSkybox(const std::string& path)
	{
		if (m_RendererData.RendererSkybox == nullptr)
			m_RendererData.RendererSkybox = std::make_shared<DX11Skybox>(path);
		else
		{
			m_RendererData.RendererSkybox.reset();
			m_RendererData.RendererSkybox = std::make_shared<DX11Skybox>(path);
		}
	}

	void DX11Renderer::SetLightInformation(const LightInformation& light)
	{
		m_RendererData.LightInfo.Ambient = light.Ambient;
		m_RendererData.LightInfo.Diffuse = light.Diffuse;
		m_RendererData.LightInfo.Specular = light.Specular;
		m_RendererData.LightInfo.CameraPosition = light.CameraPosition;
		m_RendererData.LightInfo.LightPosition = light.LightPosition;
		m_RendererData.LightInfo.LightDirection = light.LightDirection;
		m_RendererData.LightInfo.CutOff = light.CutOff;
		m_RendererData.LightInfo.OuterCutOff = light.OuterCutOff;
		m_RendererData.LightInfo.Constant = light.Constant;
		m_RendererData.LightInfo.Linear = light.Linear;
		m_RendererData.LightInfo.Quadratic = light.Quadratic;
	}

	void DX11Renderer::SendCameraPosition(const glm::vec3& camPos)
	{
		m_RendererData.LightInfo.CameraPosition = camPos;
	}

	void DX11Renderer::EndDraw()
	{
		m_RendererData.RendererSwapChain->Unbind();
		m_RendererData.RendererSwapChain->Present();
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
#pragma once

#include "DX11SwapChain.h"
#include "DX11RenderContext.h"
#include "render/Renderer.h"
#include "DX11Shader.h"
#include "DX11VertexBuffer.h"
#include "DX11IndexBuffer.h"
#include "DX11UniformBuffer.h"
#include "DX11Texture2D.h"
#include "DX11Mesh.h"
#include "DX11Skybox.h"
#include "DX11RasterizerLibrary.h"
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

		std::shared_ptr<DX11Skybox> RendererSkybox = nullptr;
		std::shared_ptr<DX11SwapChain> RendererSwapChain = nullptr;
		std::shared_ptr<DX11RenderContext> RendererContext = nullptr;

		// Clear color values
		float CCR = 0.0f;
		float CCG = 0.0f;
		float CCB = 0.0f;
		float CCA = 1.0f;

		uint32_t FBWidth;
		uint32_t FBHeight;

		glm::vec4 LightDirection = glm::vec4(0.0f);
		glm::vec4 CameraPosition = glm::vec4(0.0f);

		glm::mat4 View = glm::mat4(1.0f);
		bool CustomCamera = false;
	};

	class DX11Renderer : public Renderer
	{
	public:
		static void Init(Window* window);
		static void Prepare();
		static void Shutdown();

		static void ClearColor(float r, float g, float b, float a);
		static void Draw();

		static void PrintRendererInfo();
		static void AddVertexBuffer(const std::vector<Vertex>& vertices);
		static void AddIndexBuffer(const std::vector<uint32_t>& indices);
		static void AddTexture2D(const char* filepath);
		static void AddMesh(Mesh mesh, const std::string& name);
		static void SetMeshTransform(const std::string& meshName, const glm::mat4& transform);
		static void Resize(uint32_t width, uint32_t height);
		static void SetCamera(const glm::mat4& view);
		static void SetSkybox(const std::string& path);
		static void SetLightPosition(const glm::vec4& lightPos);
		static void SendCameraPosition(const glm::vec3& camPos);

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
		static std::unordered_map<std::string, std::shared_ptr<DX11Mesh>> m_Meshes;
		static std::shared_ptr<DX11Shader> RendererShader;

		// UBOS
		static std::shared_ptr<DX11UniformBuffer> RendererUniformBuffer;
		static std::shared_ptr<DX11UniformBuffer> RendererLightUniformBuffer;
	};
}
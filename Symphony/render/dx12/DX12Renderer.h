#pragma once

#include "render/Renderer.h"
#include <memory>
#include "DX12Device.h"
#include "DX12Fence.h"
#include "DX12Command.h"
#include "DX12Memory.h"
#include "DX12SwapChain.h"
#include "DX12Shader.h"
#include "DX12Pipeline.h"
#include "DX12VertexBuffer.h"
#include "DX12IndexBuffer.h"
#include "DX12UniformBuffer.h"
#include "DX12Texture2D.h"
#include "DX12Mesh.h"
#include "window/Window.h"
#include <core/Log.h>
#include <unordered_map>
#include <vector>
#include "DX12HeapManager.h"

namespace symphony
{
	struct DX12Vertex : public Vertex
	{
		static std::vector<D3D12_INPUT_ELEMENT_DESC> GetLayoutDescription()
		{
			std::vector<D3D12_INPUT_ELEMENT_DESC> desc;
			desc.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			desc.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			desc.push_back({ "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 5, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			return desc;
		}
	};

	struct DX12RendererData
	{
		std::shared_ptr<DX12Device> RendererDevice;
		std::vector<std::shared_ptr<DX12Fence>> RendererFences;
		std::shared_ptr<DX12Command> RendererCommand;
		std::shared_ptr<DX12SwapChain> RendererSwapChain;
		std::shared_ptr<DX12Shader> RendererShader;
		std::shared_ptr<DX12Pipeline> RendererGraphicsPipeline;

		ID3D12Resource* RendererDepthResource;

		uint32_t BufferIndex = 0;

		uint32_t FBWidth;
		uint32_t FBHeight;
		uint32_t ConstantBufferAddress;
	};

	class DX12Renderer
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
		static uint32_t GetNumMeshes() {
			return m_Meshes.size();
		}

		static DX12RendererData& GetRendererData() {
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
		static DX12RendererData m_RendererData;
		static std::unordered_map<std::string, std::shared_ptr<DX12Mesh>> m_Meshes;
	};
}
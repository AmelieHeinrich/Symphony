#include "Renderer.h"
#include "vk/VkRenderer.h"
#include "gl/GLRenderer.h"
#include "dx11/DX11Renderer.h"
#include "dx12/DX12Renderer.h"
#include "window/Window.h"

namespace symphony
{
	RenderAPI Renderer::s_RenderAPI;

	void Renderer::Init(Window* window)
	{
		s_RenderAPI = window->GetWindowRenderAPI();

		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::Init(window);
			break;
		case RenderAPI::OpenGL:
			GLRenderer::Init(window);
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::Init(window);
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::Init(window);
			break;
		}
	}

	void Renderer::Shutdown()
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::Shutdown();
			break;
		case RenderAPI::OpenGL:
			GLRenderer::Shutdown();
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::Shutdown();
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::Shutdown();
			break;
		}
	}

	void Renderer::Prepare()
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::Prepare();
			break;
		case RenderAPI::OpenGL:
			GLRenderer::Prepare();
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::Prepare();
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::Prepare();
			break;
		}
	}

	void Renderer::ClearColor(float r, float g, float b, float a)
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::ClearColor(r, g, b, a);
			break;
		case RenderAPI::OpenGL:
			GLRenderer::ClearColor(r, g, b, a);
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::ClearColor(r, g, b, a);
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::ClearColor(r, g, b, a);
			break;
		}
	}

	void Renderer::Draw()
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::Draw();
			break;
		case RenderAPI::OpenGL:
			GLRenderer::Draw();
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::Draw();
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::Draw();
			break;
		}
	}

	void Renderer::AddVertexBuffer(const std::vector<Vertex>& vertices)
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::AddVertexBuffer(vertices);
			break;
		case RenderAPI::OpenGL:
			GLRenderer::AddVertexBuffer(vertices);
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::AddVertexBuffer(vertices);
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::AddVertexBuffer(vertices);
			break;
		}
	}

	void Renderer::AddIndexBuffer(const std::vector<uint32_t>& indices)
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::AddIndexBuffer(indices);
			break;
		case RenderAPI::OpenGL:
			GLRenderer::AddIndexBuffer(indices);
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::AddIndexBuffer(indices);
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::AddIndexBuffer(indices);
			break;
		}
	}

	void Renderer::AddTexture2D(const char* filepath)
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::AddTexture2D(filepath);
			break;
		case RenderAPI::OpenGL:
			GLRenderer::AddTexture2D(filepath);
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::AddTexture2D(filepath);
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::AddTexture2D(filepath);
			break;
		}
	}

	void Renderer::AddModelData(ModelData data)
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::AddVertexBuffer(data.RendererResources.first);
			VulkanRenderer::AddIndexBuffer(data.RendererResources.second);
			VulkanRenderer::AddTexture2D(data.TextureFilepath);
			break;
		case RenderAPI::OpenGL:
			GLRenderer::AddVertexBuffer(data.RendererResources.first);
			GLRenderer::AddIndexBuffer(data.RendererResources.second);
			GLRenderer::AddTexture2D(data.TextureFilepath);
			break;
		case RenderAPI::DirectX11:
			DX11Renderer::AddVertexBuffer(data.RendererResources.first);
			DX11Renderer::AddIndexBuffer(data.RendererResources.second);
			DX11Renderer::AddTexture2D(data.TextureFilepath);
			break;
		case RenderAPI::DirectX12:
			DX12Renderer::AddVertexBuffer(data.RendererResources.first);
			DX12Renderer::AddIndexBuffer(data.RendererResources.second);
			DX12Renderer::AddTexture2D(data.TextureFilepath);
			break;
		}
	}
}
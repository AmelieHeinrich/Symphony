#include "Renderer.h"
#include "vk/VkRenderer.h"
#include "gl/GLRenderer.h"
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
		}
	}

	void Renderer::AddIndexBuffer(const std::vector<uint16_t>& indices)
	{
		switch (s_RenderAPI)
		{
		case RenderAPI::Vulkan:
			VulkanRenderer::AddIndexBuffer(indices);
			break;
		case RenderAPI::OpenGL:
			GLRenderer::AddIndexBuffer(indices);
			break;
		}
	}
}
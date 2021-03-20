#include <window/Window.h>
#include <iostream>
#include <core/PlatformDetection.h>
#include <core/Assert.h>
#include <render/Renderer.h>
#include <memory>
#undef main

using namespace symphony;

static void VulkanExample()
{
	Window VKWindow(1280, 720, "Symphony Vulkan 1.0", RenderAPI::Vulkan);

	Renderer::Init(&VKWindow);
	Renderer::ClearColor(0.57647058823f, 0.43921568627f, 0.83921568627f, 1.0f);

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	Renderer::AddTexture2D("resources/texture.jpg");
	Renderer::AddVertexBuffer(vertices);
	Renderer::AddIndexBuffer(indices);

	Renderer::Prepare();

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		Renderer::Draw();
	}

	Renderer::Shutdown();
}

static void OpenGLExample()
{
	Window VKWindow(1280, 720, "Symphony OpenGL 4.5", RenderAPI::OpenGL);

	Renderer::Init(&VKWindow);
	Renderer::ClearColor(0.57647058823f, 0.43921568627f, 0.83921568627f, 1.0f);

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 3, 1, 2, 3
	};

	Renderer::AddVertexBuffer(vertices);
	Renderer::AddIndexBuffer(indices);

	Renderer::Prepare();

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		Renderer::Draw();
	}

	Renderer::Shutdown();
}

static void DirectXExample()
{
	Window VKWindow(1280, 720, "Symphony DirectX 11", RenderAPI::DirectX);

	Renderer::Init(&VKWindow);
	Renderer::ClearColor(0.57647058823f, 0.43921568627f, 0.83921568627f, 1.0f);

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 3, 1, 2, 3
	};

	Renderer::AddVertexBuffer(vertices);
	Renderer::AddIndexBuffer(indices);

	Renderer::Prepare();

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		Renderer::Draw();
	}

	Renderer::Shutdown();
}

int main()
{
	VulkanExample();

	return 0;
}
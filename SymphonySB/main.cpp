#include <window/Window.h>
#include <iostream>
#include <core/PlatformDetection.h>
#include <core/Assert.h>
#include <render/Renderer.h>
#include <memory>

using namespace symphony;

#undef main
int main()
{
	Window VKWindow(1280, 720, "Symphony Vulkan 1.2", symphony::RenderAPI::Vulkan);
	
	Renderer::Init(&VKWindow);
	Renderer::ClearColor(0.57647058823f, 0.43921568627f, 0.83921568627f, 1.0f);

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	Renderer::AddVertexBuffer(vertices);
	Renderer::AddIndexBuffer(indices);

	Renderer::Prepare();

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		Renderer::Draw();
	}

	Renderer::Shutdown();

	return 0;
}
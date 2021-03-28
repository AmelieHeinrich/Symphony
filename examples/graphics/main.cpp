#include <window/Window.h>
#include <iostream>
#include <core/PlatformDetection.h>
#include <core/Assert.h>
#include <render/Renderer.h>
#include <memory>
#include <render/MeshBuilder.h>
#undef main

using namespace symphony;

int main()
{
    RenderAPI api = RenderAPI::Vulkan; // Pick whatever API you want (OpenGL 4.5, Vulkan, DirectX11, DirectX12)

    // Create the render window with the size you want
	Window window(1280, 720, "Symphony Graphics", api);

    // Initialise the renderer and clear the color buffer
	Renderer::Init(&window);
	Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Load the OBJ model (pair is std::pair<std::vector<Vertex>, std::vector<uint32_t>>)
	auto pair = MeshBuilder::CreateModelFromOBJ("resources/low.obj");

    // Add the resources to the renderer
	Renderer::AddTexture2D("resources/gun_texture.png");
	Renderer::AddVertexBuffer(pair.first);
	Renderer::AddIndexBuffer(pair.second);

    // Prepare the renderer for drawing
	Renderer::Prepare();

	while (window.IsWindowOpen()) {
        // Update the window and draw
		window.Update();
		Renderer::Draw();
	}

    // Shutdown renderer (clears resources, window automatically destroys at the end of the loop as well)
	Renderer::Shutdown();

    return 0;
}
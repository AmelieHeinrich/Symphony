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
	Window VKWindow(1280, 720, "Symphony Vulkan 1.0", RenderAPI::Vulkan);

	Renderer::Init(&VKWindow);
	Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ModelData gun = MeshBuilder::LoadModelDataAsync("resources/low.obj", "resources/gun_texture.png");

	Renderer::AddModelData(gun);
	Renderer::Prepare();

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		Renderer::Draw();
	}

	Renderer::Shutdown();

	return 0;
}
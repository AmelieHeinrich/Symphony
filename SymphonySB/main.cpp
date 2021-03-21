#include <window/Window.h>
#include <iostream>
#include <core/PlatformDetection.h>
#include <core/Assert.h>
#include <render/Renderer.h>
#include <memory>
#include <render/MeshBuilder.h>
#undef main

using namespace symphony;

static void VulkanExample()
{
	Window VKWindow(1280, 720, "Symphony Vulkan 1.0", RenderAPI::Vulkan);

	Renderer::Init(&VKWindow);
	Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	auto pair = MeshBuilder::CreateModelFromOBJ("resources/low.obj");

	Renderer::AddTexture2D("resources/gun_texture.png");
	Renderer::AddVertexBuffer(pair.first);
	Renderer::AddIndexBuffer(pair.second);

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
	Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	auto pair = MeshBuilder::CreateModelFromOBJ("resources/low.obj");

	Renderer::AddTexture2D("resources/gun_texture.png");
	Renderer::AddVertexBuffer(pair.first);
	Renderer::AddIndexBuffer(pair.second);

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
	Renderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	auto pair = MeshBuilder::CreateModelFromOBJ("resources/low.obj");

	Renderer::AddTexture2D("resources/gun_texture.png");
	Renderer::AddVertexBuffer(pair.first);
	Renderer::AddIndexBuffer(pair.second);

	Renderer::Prepare();

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		Renderer::Draw();
	}

	Renderer::Shutdown();
}

int main()
{
	DirectXExample();

	return 0;
}
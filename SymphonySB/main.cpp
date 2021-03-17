#include <window/Window.h>
#include <iostream>
#include <core/PlatformDetection.h>
#include <core/Assert.h>
#include <render/vk/VkRenderer.h>

using namespace symphony;

#undef main
int main()
{
	Window VKWindow(1280, 720, "Symphony Vulkan 1.2", symphony::RenderAPI::Vulkan);
	VulkanRenderer::ClearColor(0.2f, 0.8f, 0.3f, 1.0f);

	VulkanRenderer::Init(&VKWindow);

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		VulkanRenderer::Draw();
	}

	VulkanRenderer::Shutdown();

	return 0;
}
#include <core/RandomEngine.h>
#include <window/Window.h>
#include <iostream>
#include <core/PlatformDetection.h>
#include <core/Assert.h>

using namespace symphony;

int main()
{
	Window VKWindow(1280, 720, "Symphony Vulkan 1.2", symphony::RenderAPI::Vulkan);

	while (VKWindow.IsWindowOpen()) {
		VKWindow.Update();
		VKWindow.Clear();
	}

	return 0;
}
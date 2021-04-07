#pragma once

#include <imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>
#include <SDL.h>

namespace symphony
{
	class VulkanGUI
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginGUI();
		static void EndGUI();
	private:
		static VkDescriptorPool m_ImGuiDescriptorPool;
	};
}
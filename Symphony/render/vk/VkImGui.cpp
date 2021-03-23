#include "VkImGui.h"
#include "VkRenderer.h"
#include "window/Window.h"

namespace symphony
{
	SDL_Window* VulkanImGui::targetWindow;

	void VulkanImGui::Init(Window* window) 
	{
		targetWindow = window->GetWindowHandle();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		auto queueFamily = PhysicalDevice::FindQueueFamilyIndices(VulkanRenderer::GetData().m_PhysicalDevice->gpu(), VulkanRenderer::GetData().m_Surface->surface()).graphicsFamily.value();

		ImGui_ImplSDL2_InitForVulkan(window->GetWindowHandle());
		ImGui_ImplVulkan_InitInfo init_info{};
		const RendererData data = VulkanRenderer::GetData();
		init_info.Instance = data.m_Instance->instance();
		init_info.PhysicalDevice = data.m_PhysicalDevice->gpu();
		init_info.Device = data.m_Device->device();
		init_info.QueueFamily = queueFamily;
		init_info.Queue = data.m_Device->graphics_queue()->queue();
		init_info.PipelineCache = data.graphicsPipeline->GetPipelineCache();
		init_info.DescriptorPool = data.descriptorPool->GetDescriptorPool();
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;
		init_info.ImageCount = data.m_SwapChain->swap_chain_images().size();
		init_info.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&init_info, data.m_RenderPass->render_pass());

		{
			VkCommandPool command_pool = data.m_CommandPool->command_pool();
			VkCommandBuffer command_buffer = data.commandBuffers[data.currentFrame]->GetCommandBuffer();

			vkResetCommandPool(data.m_Device->device(), command_pool, 0);

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(command_buffer, &begin_info);

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			vkEndCommandBuffer(command_buffer);
			vkQueueSubmit(data.m_Device->graphics_queue()->queue(), 1, &end_info, VK_NULL_HANDLE);
			vkDeviceWaitIdle(data.m_Device->device());
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}

	void VulkanImGui::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
	}

	void VulkanImGui::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(targetWindow);
		ImGui::NewFrame();
	}

	void VulkanImGui::EndFrame()
	{
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(draw_data, VulkanRenderer::GetData().commandBuffers[VulkanRenderer::GetData().currentFrame]->GetCommandBuffer());
	}
}
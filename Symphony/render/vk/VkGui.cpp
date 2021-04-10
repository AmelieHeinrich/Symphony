#include "VkGui.h"
#include <xutility>
#include "VkRenderer.h"
#include "core/Application.h"

namespace symphony
{
	static void check_vk_err(VkResult err)
	{
		if (err == 0)
			return;
		SY_CORE_ERROR("[VkImGui] Error: VkResult = %d\n", err);
		if (err < 0)
			abort();
	}

	VkDescriptorPool VulkanGUI::m_ImGuiDescriptorPool = VK_NULL_HANDLE;

	void VulkanGUI::Init()
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		if (vkCreateDescriptorPool(VulkanRenderer::GetData().m_Device->device(), &pool_info, nullptr, &m_ImGuiDescriptorPool) != VK_SUCCESS)
			SY_CORE_ERROR("Vulkan GUI : Failed to create gui descriptor pool!");

		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SDL_Window* _window = Application::Get().GetWindow().GetWindowHandle();

		ImGui_ImplSDL2_InitForVulkan(_window);

		VkInstance _instance = VulkanRenderer::GetData().m_Instance->instance();
		VkPhysicalDevice _chosenGPU = VulkanRenderer::GetData().m_PhysicalDevice->gpu();
		VkDevice _device = VulkanRenderer::GetData().m_Device->device();
		VkQueue _graphicsQueue = VulkanRenderer::GetData().m_Device->graphics_queue()->queue();
		VkRenderPass _renderPass = VulkanRenderer::GetData().m_ImGuiPass->render_pass();
		VkPipelineCache ppCache = VulkanRenderer::GetData().graphicsPipeline->GetPipelineCache();

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = _instance;
		init_info.PhysicalDevice = _chosenGPU;
		init_info.Device = _device;
		init_info.Queue = _graphicsQueue;
		init_info.DescriptorPool = m_ImGuiDescriptorPool;
		init_info.MinImageCount = 2;
		init_info.ImageCount = 2;
		init_info.CheckVkResultFn = check_vk_err;
		init_info.PipelineCache = ppCache;

		ImGui_ImplVulkan_Init(&init_info, _renderPass);

		VkCommandBuffer cmd = CommandBuffer::BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(cmd);
		CommandBuffer::EndSingleTimeCommands(cmd);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void VulkanGUI::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		VkDevice _device = VulkanRenderer::GetData().m_Device->device();
		vkDestroyDescriptorPool(_device, m_ImGuiDescriptorPool, nullptr);
	}

	void VulkanGUI::BeginGUI()
	{
		SDL_Window* _window = Application::Get().GetWindow().GetWindowHandle();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(_window);
		ImGui::NewFrame();
	}

	void VulkanGUI::EndGUI()
	{
		ImGuiIO& io = ImGui::GetIO();
		VkCommandBuffer cmd = VulkanRenderer::GetData().commandBuffer->GetCommandBuffer();
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}
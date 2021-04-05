#include "VkInstance.h"
#include <SDL_vulkan.h>
#include <vector>
#include "core/exception/VulkanException.h"
#include <core/Log.h>

namespace symphony
{
    Instance::Instance(SDL_Window* window)
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "SymphonySB";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Symphony";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        unsigned int count;
        if (!SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr)) {
            SY_CORE_ERROR("Vulkan: Failed to fetch vk extensions!");
        }

        std::vector<const char*> extensions = {
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME 
        };
        size_t additional_extension_count = extensions.size();
        extensions.resize(additional_extension_count + count);

        if (!SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data() + additional_extension_count)) {
            SY_CORE_ERROR("Vulkan: Failed to fetch vk extensions!");
        }

        auto layers = std::vector<const char*>{ "VK_LAYER_KHRONOS_validation" };

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();


        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            SY_CORE_ERROR("Vulkan: Failed to create instance!");
        }
    }

    Instance::~Instance()
    {
        vkDestroyInstance(m_Instance, nullptr);
    }
}
workspace "Symphony"
    architecture "x86_64"
    startproject "SymphonySB"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}"

group "ext"
	include "ext/SDL2"
	include "ext/glad"
    include "vendor/imgui"
group ""

project "Symphony"
    location "Symphony"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Symphony/**.cpp",
        "Symphony/**.h",
        "vendor/imgui/examples/imgui_impl_sdl.h",
        "vendor/imgui/examples/imgui_impl_sdl.cpp",
        "vendor/imgui/examples/imgui_impl_vulkan.h",
        "vendor/imgui/examples/imgui_impl_vulkan.cpp",
        "vendor/stb/stb_image.h",
        "vendor/tinyobjloader/tiny_obj_loader.h"
    }

    defines
    {
        "SYMPHONY_BUILD",
        "SYMPHONY_DLL",
        "_DLL"
    }

    includedirs
    {
        "Symphony",
		"ext/SDL2/include",
		"ext/glad/include",
        "$(VULKAN_SDK)/include",
        "vendor/glm",
        "vendor/imgui",
        "vendor/stb",
        "vendor/tinyobjloader",
        "vendor/entt/single_include",
        "vendor/spdlog/include"
    }

	links
	{
        "opengl32",
		"glad",
		"sdl2",
        "imgui"
	}

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/SymphonySB")
        }

        links
        {
            "$(VULKAN_SDK)/lib/vulkan-1.lib",
            "$(VULKAN_SDK)/lib/shaderc.lib",
            "$(VULKAN_SDK)/lib/shaderc_combined.lib",
            "$(VULKAN_SDK)/lib/glslang.lib",
            "gdi32",
            "kernel32",
            "user32",
			"d3d11",
			"d3dcompiler",
            "d3d12",
			"dxgi"
        }

    filter "configurations:Debug"
		buildoptions "/MDd"
        defines { "SYMPHONY_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        buildoptions "/MD"
        defines { "SYMPHONY_RELEASE" }
        optimize "On"

project "SymphonySB"
    location "SymphonySB"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "SymphonySB/**.cpp",
        "SymphonySB/**.h"
    }

    includedirs
    {
        "Symphony",
        "ext/SDL2/include",
        "$(VULKAN_SDK)/include",
        "vendor/glm",
        "vendor/entt/single_include/",
        "vendor/spdlog/include"
    }

    links
    {
        "Symphony",
        "$(VULKAN_SDK)/lib/vulkan-1.lib",
        "sdl2"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "configurations:Debug"
		buildoptions "/MTd"
        defines { "SYMPHONY_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        buildoptions "/MT"
        defines { "SYMPHONY_RELEASE" }
        optimize "On"

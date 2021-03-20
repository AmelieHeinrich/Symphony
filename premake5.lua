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

    warnings 'Off'

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
        "vendor/stb/stb_image.h"
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
        "vendor/stb"
    }

	links
	{
		"glad",
		"sdl2",
        "$(VULKAN_SDK)/lib/vulkan-1.lib",
        "imgui"
	}

    filter "system:windows"
        defines { "SYMPHONY_WINDOWS" }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/SymphonySB")
        }

        links
        {
            "opengl32",
			"glad",
            "gdi32",
            "kernel32",
            "user32",
			"d3d11",
			"d3dcompiler",
			"dxgi"
        }

    filter "system:linux"
        defines { "SYMPHONY_LINUX" }

    filter "system:macos"
        defines { "SYMPHONY_MAC" }

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
        "vendor/glm"
    }

    links
    {
        "Symphony",
        "$(VULKAN_SDK)/lib/vulkan-1.lib",
        "sdl2"
    }

    filter "system:windows"
        defines { "SYMPHONY_WINDOWS" }

    filter "system:linux"
        defines { "SYMPHONY_LINUX" }

    filter "system:macos"
        defines { "SYMPHONY_MAC" }

    filter "configurations:Debug"
		buildoptions "/MTd"
        defines { "SYMPHONY_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        buildoptions "/MT"
        defines { "SYMPHONY_RELEASE" }
        optimize "On"

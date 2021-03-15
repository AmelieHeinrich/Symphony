project "GLAD"
    kind "StaticLib"
    language "C"
    staticruntime "On"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")
    
    files
    {
        "src/**.c",
        "include/**.h"
    }

    includedirs
    {
        "include"
    }

    filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
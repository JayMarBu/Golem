workspace "Golem"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glfw"] = "Golem/external/glfw/include"

--include "Golem/external/glfw"
include "external/premake/glfw"

project "Golem"
	location "Golem"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "golpch.h"
	pchsource "Golem/src/golpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	links
	{	
		"glfw",
		"$(VULKAN_SDK)/lib/vulkan-1.lib"
	}

	includedirs
	{
		"$(VULKAN_SDK)/include",
		"%{prj.name}/external/spdlog/include",
		"Golem/src",
		"%{IncludeDir.glfw}"
	}



	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GOL_PLATFORM_WINDOWS",
			"GOL_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "GOL_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "GOL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "GOL_DIST"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"$(VULKAN_SDK)/include",
		"Golem/external/spdlog/include",
		"Golem/src"
	}

	links
	{
		"$(VULKAN_SDK)/lib/vulkan-1.lib",
		"Golem"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GOL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "GOL_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "GOL_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "GOL_DIST"
		optimize "On" 
workspace "Golem"
	architecture "x64"
	startproject "Sandbox"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glfw"] = "Golem/external/glfw/include"
IncludeDir["imgui"] = "Golem/external/imgui"
IncludeDir["glm"] = "Golem/external/glm"

group "Dependancies"
	include "external/premake/glfw"
	include "external/premake/imgui"

group ""

project "Golem"
	location "Golem"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

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
		"imgui",
		"$(VULKAN_SDK)/lib/vulkan-1.lib"
	}

	includedirs
	{
		"$(VULKAN_SDK)/include",
		"%{prj.name}/external/spdlog/include",
		"Golem/src",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}"
	}



	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"GOL_PLATFORM_WINDOWS",
			"GOL_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "GOL_DEBUG"
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines "GOL_RELEASE"
		optimize "On"
		runtime "Release"

	filter "configurations:Dist"
		defines "GOL_DIST"
		optimize "On"
		runtime "Release"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		"Golem/src",
		"Golem/external",
		"%{IncludeDir.glm}"
	}

	links
	{
		"$(VULKAN_SDK)/lib/vulkan-1.lib",
		"Golem",
		"imgui"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"GOL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "GOL_DEBUG"
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines "GOL_RELEASE"
		optimize "On"
		runtime "Release"

	filter "configurations:Dist"
		defines "GOL_DIST"
		optimize "On" 
		runtime "Release"
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
IncludeDir["tinyobj"] = "Golem/external/tinyobjloader"
IncludeDir["stb"] = "Golem/external/stb"
IncludeDir["entt"] = "Golem/external/entt"

group "Dependancies"
	include "external/premake/glfw"
	include "external/premake/imgui"

group ""

project "Golem"
	location "Golem"
	kind "StaticLib"
	language "C++"
	staticruntime "on"
	cppdialect "C++17"

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
		"$(VULKAN_SDK)/Lib/vulkan-1.lib"
	}

	includedirs
	{
		"$(VULKAN_SDK)/Include",
		"%{prj.name}/external/spdlog/include",
		"Golem/src",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.tinyobj}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.entt}"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GOL_PLATFORM_WINDOWS",
			"GOL_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX"
		}

	filter "configurations:Debug"
		defines "GOL_DEBUG"
		links 
		{
			"$(VULKAN_SDK)/Lib/shaderc_sharedd.lib"
		}
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "GOL_RELEASE"
		links 
		{
			"$(VULKAN_SDK)/Lib/shaderc_shared.lib"
		}
		optimize "on"
		runtime "Release"

	filter "configurations:Dist"
		defines "GOL_DIST"
		optimize "on"
		runtime "Release"

project "Golem-Editor"
	location "Golem-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/shaders/**.vert",
		"%{prj.name}/shaders/**.frag"
	}

	includedirs
	{
		"$(VULKAN_SDK)/Include",
		"Golem/external/spdlog/include",
		"Golem/src",
		"Golem/external",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.imgui}"
	}

	links
	{
		"$(VULKAN_SDK)/Lib/vulkan-1.lib",
		"Golem"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GOL_PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
			"for /R %%f in (*.vert *.frag ) do ( \n"..
				"echo Compiling: %%f\n"..
				"%VULKAN_SDK%/Bin/glslc.exe %%f -o %%f.spv\n"..
				"echo.\n"..
				")"
		}

	filter "configurations:Debug"
		defines "GOL_DEBUG"
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "GOL_RELEASE"
		optimize "on"
		runtime "Release"

	filter "configurations:Dist"
		defines "GOL_DIST"
		optimize "on" 
		runtime "Release"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/shaders/**.vert",
		"%{prj.name}/shaders/**.frag"
	}

	includedirs
	{
		"$(VULKAN_SDK)/Include",
		"Golem/external/spdlog/include",
		"Golem/src",
		"Golem/external",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.imgui}"
	}

	links
	{
		"$(VULKAN_SDK)/Lib/vulkan-1.lib",
		"Golem"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GOL_PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
			"for /R %%f in (*.vert *.frag ) do ( \n"..
				"echo Compiling: %%f\n"..
				"%VULKAN_SDK%/Bin/glslc.exe %%f -o %%f.spv\n"..
				"echo.\n"..
				")"
		}

	filter "configurations:Debug"
		defines "GOL_DEBUG"
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "GOL_RELEASE"
		optimize "on"
		runtime "Release"

	filter "configurations:Dist"
		defines "GOL_DIST"
		optimize "on" 
		runtime "Release"
imguidir = "../../../Golem/external/imgui/"

project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    staticruntime "on"
	location(imguidir)

	targetdir ("../../../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../../../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(imguidir.."imconfig.h"),
		(imguidir.."imgui.h"),
		(imguidir.."imgui.cpp"),
		(imguidir.."imgui_draw.cpp"),
		(imguidir.."imgui_internal.h"),
		(imguidir.."imgui_tables.cpp"),
		(imguidir.."imgui_widgets.cpp"),
		(imguidir.."imstb_rectpack.h"),
		(imguidir.."imstb_textedit.h"),
		(imguidir.."imstb_truetype.h"),
		(imguidir.."imgui_demo.cpp")
	}

	defines 
	{ 
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

glfwdir = "../../../Golem/external/glfw/"

project "GLFW"
	kind "StaticLib"
	language "C"
	location (glfwdir)
	staticruntime "on"

	targetdir ("../../../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../../../bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(glfwdir.."include/GLFW/glfw3.h"),
		(glfwdir.."include/GLFW/glfw3native.h"),
		(glfwdir.."src/glfw_config.h"),
		(glfwdir.."src/context.c"),
		(glfwdir.."src/init.c"),
		(glfwdir.."src/input.c"),
		(glfwdir.."src/monitor.c"),
		(glfwdir.."src/vulkan.c"),
		(glfwdir.."src/window.c"),
		(glfwdir.."src/internal.h"),
		(glfwdir.."src/platform.h"),
		(glfwdir.."src/mappings.h"),
		(glfwdir.."src/platform.c"),
		(glfwdir.."src/egl_context.c"),
    	(glfwdir.."src/osmesa_context.c"),
		(glfwdir.."src/null_platform.h"),
    	(glfwdir.."src/null_joystick.h"),
    	(glfwdir.."src/null_init.c"),

    	(glfwdir.."src/null_monitor.c"),
    	(glfwdir.."src/null_window.c"),
    	(glfwdir.."src/null_joystick.c"),
	}

	filter "system:windows"
		systemversion "latest"

		files
		{
			(glfwdir.."src/win32_init.c"),
			(glfwdir.."src/win32_module.c"),
			(glfwdir.."src/win32_joystick.c"),
			(glfwdir.."src/win32_monitor.c"),
			(glfwdir.."src/win32_time.c"),
			(glfwdir.."src/win32_time.h"),
			(glfwdir.."src/win32_thread.c"),
			(glfwdir.."src/win32_thread.h"),
			(glfwdir.."src/win32_window.c"),
			(glfwdir.."src/wgl_context.c"),
			(glfwdir.."src/egl_context.c"),
			(glfwdir.."src/osmesa_context.c")
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

		links
		{
			"Dwmapi.lib"
		}

	filter "configurations:Debug"
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		optimize "on"
		runtime "Release"

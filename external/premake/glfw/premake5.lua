glfwdir = "../../../Golem/external/glfw/"

project "GLFW"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	location (glfwdir)

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
		(glfwdir.."src/window.c")
	}
	filter "system:linux"
		pic "On"

		systemversion "latest"
		
		files
		{
			(glfwdir.."src/x11_init.c"),
			(glfwdir.."src/x11_monitor.c"),
			(glfwdir.."src/x11_window.c"),
			(glfwdir.."src/xkb_unicode.c"),
			(glfwdir.."src/posix_time.c"),
			(glfwdir.."src/posix_thread.c"),
			(glfwdir.."src/glx_context.c"),
			(glfwdir.."src/egl_context.c"),
			(glfwdir.."src/osmesa_context.c"),
			(glfwdir.."src/linux_joystick.c")
		}

		defines
		{
			"_GLFW_X11"
		}

	filter "system:windows"
		systemversion "latest"

		files
		{
			(glfwdir.."src/win32_init.c"),
			(glfwdir.."src/win32_joystick.c"),
			(glfwdir.."src/win32_monitor.c"),
			(glfwdir.."src/win32_time.c"),
			(glfwdir.."src/win32_thread.c"),
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
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
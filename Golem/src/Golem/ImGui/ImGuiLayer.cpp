#include "golpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/Vulkan/imgui_impl_vulkan.h"
#include "Platform/Vulkan/imgui_impl_glfw.h"
#include "GLFW/glfw3.h"
#include "Platform/Windows/WindowWin32.h"
#include "../Application.h"

namespace golem
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{
		OnDetach();
	}

	void ImGuiLayer::OnAttach()
	{
		auto& device = Application::Get().GetDevice();
		auto& window = Application::Get().GetWindow();

		// set up a descriptor pool stored on this instance, see header for more comments on this.
		VkDescriptorPoolSize pool_sizes[] = {
			{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &m_descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up descriptor pool for imgui");
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		// Setup Platform/Renderer backends
		// Initialize imgui for vulkan
		ImGui_ImplGlfw_InitForVulkan( WindowWin32::GetGLFWWindow(&window), false);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = device.instance();
		init_info.PhysicalDevice = device.physicalDevice();
		init_info.Device = device.device();
		init_info.QueueFamily = device.getGraphicsQueueFamily();
		init_info.Queue = device.graphicsQueue();

		// pipeline cache is a potential future optimization, ignoring for now
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = m_descriptorPool;
		// todo, I should probably get around to integrating a memory allocator library such as Vulkan
		// memory allocator (VMA) sooner than later. We don't want to have to update adding an allocator
		// in a ton of locations.
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.MinImageCount = 2;
		init_info.ImageCount = Application::Get().GetRenderer().GetImageCount();
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, Application::Get().GetRenderer().GetSwapChainRenderPass());

		// upload fonts, this is done by recording and submitting a one time use command buffer
		// which can be done easily bye using some existing helper functions on the golem device object
		auto commandBuffer = device.BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.EndSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void ImGuiLayer::OnDetach()
	{
		auto& device = Application::Get().GetDevice();

		vkDestroyDescriptorPool(device.device(), m_descriptorPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate(VkCommandBuffer commandBuffer)
	{
		NewFrame();

		bool open = true;

		ImGui::ShowDemoWindow(&open);

		Render(commandBuffer);
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrollEvent));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.Dispatch<KeyTypedEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnKeyTypedEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(ImGuiLayer::OnWindowResizedEvent));
	}

	void ImGuiLayer::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

	}

	void ImGuiLayer::Render(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImDrawData* drawdata = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (e.GetMouseButton() >= 0 && e.GetMouseButton() < ImGuiMouseButton_COUNT)
			io.AddMouseButtonEvent(e.GetMouseButton(), GLFW_PRESS);

		return io.WantCaptureMouse;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (e.GetMouseButton() >= 0 && e.GetMouseButton() < ImGuiMouseButton_COUNT)
			io.AddMouseButtonEvent(e.GetMouseButton(), GLFW_RELEASE);

		return io.WantCaptureMouse;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		/*if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			int window_x, window_y;
			glfwGetWindowPos(window, &window_x, &window_y);
			x += window_x;
			y += window_y;
		}*/
		io.AddMousePosEvent((float)e.GetX(), e.GetY());

		return io.WantCaptureMouse;
	}

	bool ImGuiLayer::OnMouseScrollEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.GetYOffset();
		io.MouseWheelH += e.GetXOffset();

		return io.WantCaptureMouse;
	}

	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = true;

		io.KeyCtrl	= io.KeysDown[GLFW_KEY_LEFT_CONTROL]|| io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT]	|| io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt	= io.KeysDown[GLFW_KEY_LEFT_ALT]	|| io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER]	|| io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		//if(e.GetKeyCode() == GLFW_KEY_BACKSPACE)
		//	io.AddInputCharacter(GLFW_KEY_BACKSPACE);

		return io.WantCaptureKeyboard;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = false;

		return io.WantCaptureKeyboard;
	}

	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		if(e.GetKeyCode() > 0 && e.GetKeyCode() < 0x10000)
			io.AddInputCharacter(e.GetKeyCode());

		return io.WantCaptureKeyboard;
	}

	bool ImGuiLayer::OnWindowResizedEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f,1.0f);
		return true;
	}

}
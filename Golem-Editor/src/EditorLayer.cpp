#include "EditorLayer.h"
#include "glfw/include/GLFW/glfw3.h"
#include "Golem/Time.h"
#include "Golem/Profiling/Profiler.h"
#include "Golem/ECS/GameObject.h"
#include "Golem/ECS/Components/RenderComponents.h"

namespace golem
{

	EditorLayer::EditorLayer()
		: Layer("Editor Layer")
	{
		CreateRenderPass();

		m_renderTexture = Ref<RenderTexture>(new RenderTexture(
			Application::Get().GetRenderer().GetSwapChain().extent().width,
			Application::Get().GetRenderer().GetSwapChain().extent().height,
			m_renderPass));

		
		CreateScene();
		CreateUBO();
		CreateDescriptors();
		CreateRenderSystems();

		m_gameObjectHierarchyEditor.SetScene(Application::Get().GetScene());
	}

	EditorLayer::~EditorLayer()
	{
		vkDestroyRenderPass(Application::Get().GetDevice().device(), m_renderPass, nullptr);
	}

	void EditorLayer::OnRender(VkCommandBuffer commandBuffer)
	{
		GOL_PROFILE_FUNCTION();

		// ------ update inputs ------
		if(m_viewportFocused)
		{
			m_camera.GetComponent<KeyboardMovementController>().MoveInPlaneXZ(Time::DeltaTime(), m_camera, m_viewportCentre);
		}

		// ------ update camera ------
		{
			GOL_PROFILE_SCOPE("camera update");
			auto& transform = m_camera.GetComponent<Transform>();
			m_camera.GetComponent<Camera>().SetViewXYZ(transform.translation, transform.EulerAngles());

			//float aspect = golem::Application::Get().GetRenderer().GetAspectRatio();
			float aspect = m_viewportSize.x / m_viewportSize.y;
			m_camera.GetComponent<Camera>().SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.0f);
		}
		// ------ fill frame info struct ------
		golem::FrameInfo fInfo
		{
			golem::Application::Get().GetRenderer().GetFrameIndex(),
			Time::DeltaTime(),
			commandBuffer,
			m_camera.GetComponent<Camera>(),
			m_camera.GetComponent<Transform>().translation,
			m_globalDescriptorSets[golem::Application::Get().GetRenderer().GetFrameIndex()]
		};

		// ------ update uniform buffers ------
		{
			GOL_PROFILE_SCOPE("update global uniforms");
			golem::GlobalUBO ubo{};
			auto& cam = m_camera.GetComponent<Camera>();
			ubo.projection = cam.GetProjection();
			ubo.view = cam.GetView();

			m_pointLightRenderSystem->Update(fInfo, ubo);

			m_UBObuffers[fInfo.frameIndex]->WriteToBuffer(&ubo);
			m_UBObuffers[fInfo.frameIndex]->Flush();
		}

		// ------ render scene ------
		{
			GOL_PROFILE_SCOPE("render scene");
			BeginRenderPass(commandBuffer);

			m_simpleRenderSystem->RenderGameObjects(fInfo);
			m_pointLightRenderSystem->Render(fInfo);

			EndRenderPass(commandBuffer);
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		GOL_PROFILE_FUNCTION();

		BeginDocking();

		DrawSceneViewport();

		ImGui::Begin("test");

		if (ImGui::Button("Recompile Simple Shader") && !m_simpleRenderSystem->IsRegenerating())
		{
			Application::Get().GetThreadPool().Enqueue([=] {m_simpleRenderSystem->RuntimeCreatePipeline(); });
		}

		if (ImGui::Button("Recompile Point Light Shader") && !m_pointLightRenderSystem->IsRegenerating())
		{
			Application::Get().GetThreadPool().Enqueue([=] {m_pointLightRenderSystem->RuntimeCreatePipeline(); });
		}

		ImGui::Spacing();

		ImGui::Text("time : %.5f", Time::DeltaTime().GetSeconds());
		ImGui::Text("fps : %.5f", Time::FPS());

		ImGui::Spacing();

		static int count = -1;

		ImGui::InputInt("Frame recording count", &count);

		if(Profiler::IsRecording() && count >= 0)
		{
			count--;
			if(count <= 0)
			{
				Profiler::End();
				count = -1;
			}
			
		}

		const char* buttonText = (Profiler::IsRecording())? "End Recording" : "Begin Recording";

		if(ImGui::Button(buttonText))
		{
			if(!Profiler::IsRecording())
				Profiler::Record();
			else
				Profiler::End();
		}

		ImGui::End();

		m_gameObjectHierarchyEditor.Draw();

		EndDocking();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		golem::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<ShaderReCompileEvent>(BIND_EVENT_FUNC(EditorLayer::OnShaderRecompile));
		dispatcher.Dispatch<ViewportResizeEvent>(BIND_EVENT_FUNC(EditorLayer::OnViewportResize));
	}

	void EditorLayer::CreateRenderPass()
	{
		auto colourFormat = Application::Get().GetRenderer().GetSwapChain().imageFormat();
		auto depthFormat = Application::Get().GetRenderer().GetSwapChain().FindDepthFormat();

		VkAttachmentDescription attachmentDescription{};
		attachmentDescription.format = depthFormat;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = colourFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthReference;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, attachmentDescription };
		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassCreateInfo.pAttachments = attachments.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassCreateInfo.pDependencies = dependencies.data();

		SAFE_RUN_VULKAN_FUNC(vkCreateRenderPass(Application::Get().GetDevice().device(), &renderPassCreateInfo, nullptr, &m_renderPass), "");
	}

	void EditorLayer::CreateScene()
	{
		auto& device = golem::Application::Get().GetDevice();

		Application::Get().SetScene(new Scene());

		m_camera = GameObject::Create("Camera");
		m_camera.AddComponent<Camera>();
		m_camera.AddComponent<KeyboardMovementController>().moveSpeed = 5;
		m_camera.GetComponent<Transform>().translation = { 0.0f,0.0f,-5 };

		auto gObj = GameObject::Create("Cube");
		gObj.AddComponent<MeshRendererComponent>(Model::CreateModelFromPrimative(device, Primitives::Cube, false));
		gObj.GetComponent<Transform>().translation = { 1.5f,0.f,0 };

		gObj = GameObject::Create("Floor");
		gObj.AddComponent<MeshRendererComponent>(Model::CreateModelFromPrimative(device, Primitives::Quad));
		{
			auto& t = gObj.GetComponent<Transform>();
			t.translation = { 0,0.75f,0 };
			t.scale = { 7,1,7 };
		}
		
		gObj = GameObject::Create("Vase");
		gObj.AddComponent<MeshRendererComponent>(Model::CreateModelFromFile(device, "models/smooth_vase.obj"));
		{
			auto& t = gObj.GetComponent<Transform>();
			t.translation = { -1.5f,0.5f,0 };
			t.scale = { 5,5,5 };
		}

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			gObj = GameObject::Create("Point Light" + i);
			gObj.AddComponent<PointLightComponent>(1.0f, lightColors[i]);
			auto rotateLight = glm::rotate(glm::mat4(1.0f), (i * glm::two_pi<float>()) / lightColors.size(), { 0,-1,0 });
			gObj.GetComponent<Transform>().translation = 1.5f * glm::vec3(rotateLight * glm::vec4(-1, -0.5, -1, 1));
			gObj.GetComponent<Transform>().scale.x = 0.05f;
		}

	}

	void EditorLayer::CreateUBO()
	{
		auto& device = golem::Application::Get().GetDevice();

		m_UBObuffers.resize(golem::SwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < m_UBObuffers.size(); i++)
		{
			m_UBObuffers[i] = std::make_unique<golem::Buffer>(
				device,
				sizeof(golem::GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				std::lcm(
					device.properties.limits.minUniformBufferOffsetAlignment,
					device.properties.limits.nonCoherentAtomSize)
				);

			m_UBObuffers[i]->Map();
		}
	}

	void EditorLayer::CreateDescriptors()
	{
		auto& device = golem::Application::Get().GetDevice();

		m_sampler = std::make_unique<golem::Sampler>();

		m_globalPool = golem::DescriptorPool::Builder(device)
			.SetMaxSets(golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		m_globalSetLayout = golem::DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		m_globalDescriptorSets.resize(golem::SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = m_UBObuffers[i]->DescriptorInfo();
			auto imageInfo = golem::Application::Get().GetTextureManager().ImageInfo(*m_sampler, "default");

			golem::DescriptorWriter(*m_globalSetLayout, *m_globalPool)
				.WriteBuffer(0, &bufferInfo)
				.WriteImage(1, &imageInfo)
				.Build(m_globalDescriptorSets[i]);
		}
	}

	void EditorLayer::CreateRenderSystems()
	{
		auto& device = golem::Application::Get().GetDevice();

		m_simpleRenderSystem = std::make_unique<golem::SimpleRenderSystem>(
			device,
			m_globalSetLayout->GetDescriptorSetLayout());

		m_pointLightRenderSystem = std::make_unique<golem::PointLightRenderSystem>(
			device,
			m_globalSetLayout->GetDescriptorSetLayout());
	}

	void EditorLayer::BeginRenderPass(VkCommandBuffer commandBuffer)
	{
		VkClearColorValue defaultClearColor = { { 0.1f, 0.1f, 0.1f, 1.0f } };
		VkClearValue clearValues[2];
		VkViewport viewport{};
		VkRect2D scissor{};

		clearValues[0].color = defaultClearColor;
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_renderPass;
		renderPassBeginInfo.framebuffer = m_renderTexture->GetFrameBuffer();
		renderPassBeginInfo.renderArea.extent.width = m_renderTexture->GetWidth();
		renderPassBeginInfo.renderArea.extent.height = m_renderTexture->GetHeight();
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		viewport.width = (float)m_renderTexture->GetWidth();
		viewport.height = (float)m_renderTexture->GetHeight();
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		scissor.extent.width = m_renderTexture->GetWidth();
		scissor.extent.height = m_renderTexture->GetHeight();
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	bool EditorLayer::OnShaderRecompile(golem::ShaderReCompileEvent& e)
	{
		e.GetRenderSystem()->CompleteRegeneration();

		return true;
	}

	bool EditorLayer::OnViewportResize(ViewportResizeEvent& e)
	{
		vkDeviceWaitIdle(Application::Get().GetDevice().device());
		m_renderTexture->Resize(e.GetWidth(), e.GetHeight(), m_renderPass);

		return true;
	}

	void EditorLayer::BeginDocking()
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", NULL, window_flags);

		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::DrawSceneViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
		ImGui::Begin("Viewport");

		m_viewportFocused = ImGui::IsWindowFocused();

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		m_viewportCentre = {windowPos.x + (windowSize.x*0.5f),windowPos.y + (windowSize.y * 0.5f) };

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		if (m_viewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			Application::FireEvent(new ViewportResizeEvent(viewportPanelSize.x, viewportPanelSize.y));
			m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		}

		ImGui::Image(m_renderTexture->GetDescriptorSet(Application::Get().GetRenderer().GetFrameIndex()), viewportPanelSize);

		ImGui::End();
		ImGui::PopStyleVar();
	}

}
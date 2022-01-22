#include "Golem.h"
#include "ImGui/imgui.h"

struct CameraWrapper
{
	CameraWrapper() : gObject(golem::TempGameObject::Create()) {}

	golem::Camera camera{};
	golem::TempGameObject gObject;
	golem::KeyboardMovementController controller{};
};

struct GlobalUBO
{
	glm::mat4 projectionView{ 1.0f };
	glm::vec3 lightDir = glm::normalize(glm::vec3(1, -3, -1));
};

class ExampleLayer : public golem::Layer
{
private:
	std::unique_ptr<golem::DescriptorPool> m_globalPool;
	std::unique_ptr<golem::DescriptorSetLayout> m_globalSetLayout;
	std::vector<VkDescriptorSet> m_globalDescriptorSets;

	std::vector<std::unique_ptr<golem::Buffer>> m_UBObuffers;

	std::unique_ptr<golem::SimpleRenderSystem> m_simpleRenderSystem;

	std::vector<golem::TempGameObject> m_gameObjects;
	CameraWrapper m_camera{};

	std::unique_ptr<golem::Sampler> m_sampler;

	golem::FrameTimer m_timer;

public:
	ExampleLayer()
		: Layer("Example")
	{
		m_sampler = std::make_unique<golem::Sampler>();

		m_camera.gObject.transform.translation = { 0.0f,0.0f,-5 };
		m_camera.controller.moveSpeed = 5;

		auto& device = golem::Application::Get().GetDevice();

		std::shared_ptr<golem::Model> model = golem::Model::CreateModelFromPrimative(device, golem::Primitives::Cube, false);
		auto cube = golem::TempGameObject::Create();
		cube.model = model;
		cube.transform.translation = { 0.f,0.f,-.01f };
		m_gameObjects.push_back(std::move(cube));

		//m_textureManager->LoadTexture("bunny", "media/textures/bunny.png");

		// init ubo
		m_UBObuffers.resize(golem::SwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < m_UBObuffers.size(); i++)
		{
			m_UBObuffers[i] = std::make_unique<golem::Buffer>(
				device,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				std::lcm(
					device.properties.limits.minUniformBufferOffsetAlignment,
					device.properties.limits.nonCoherentAtomSize)
				);

			m_UBObuffers[i]->Map();
		}

		// init descriptor pools
		m_globalPool = golem::DescriptorPool::Builder(device)
			.SetMaxSets(golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		m_globalSetLayout = golem::DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
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

		// init render systems
		m_simpleRenderSystem = std::make_unique<golem::SimpleRenderSystem>(
			device,
			golem::Application::Get().GetRenderer().GetSwapChainRenderPass(),
			m_globalSetLayout->GetDescriptorSetLayout(),
			"shaders/simple_shader/simple_shader.vert.spv",
			"shaders/simple_shader/simple_shader.frag.spv");
	}

	void OnUpdate(VkCommandBuffer commandBuffer) override
	{
		m_timer.frame();

		m_camera.controller.MoveInPlaneXZ(m_timer.getTime(), m_camera.gObject);
		m_camera.camera.SetViewXYZ(m_camera.gObject.transform.translation, m_camera.gObject.transform.rotation);

		float aspect = golem::Application::Get().GetRenderer().GetAspectRatio();
		m_camera.camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.0f);

		// Temp stuff ---------------------------------------------------------
			// ******fill frame info struct * *****
		golem::FrameInfo fInfo{
			golem::Application::Get().GetRenderer().GetFrameIndex(),
			m_timer.getTime(),
			commandBuffer,
			m_camera.camera,
			m_globalDescriptorSets[golem::Application::Get().GetRenderer().GetFrameIndex()]
		};

		// ****** update uniform buffers ******
		GlobalUBO ubo{};
		ubo.projectionView = m_camera.camera.GetProjection() * m_camera.camera.GetView();

		m_UBObuffers[fInfo.frameIndex]->WriteToBuffer(&ubo);
		m_UBObuffers[fInfo.frameIndex]->Flush();
		// Temp stuff ---------------------------------------------------------

		m_simpleRenderSystem->RenderGameObjects(fInfo, m_gameObjects);
	}

	void OnImGuiRender() override
	{
		//ImGui::Begin("test");
		//ImGui::End();
	}

	void OnEvent(golem::Event& e) override
	{
		//if(e.GetEventType() != golem::EventType::MouseMoved)
		//	GOL_TRACE("{0}", e.ToString());
	}
};

class Sandbox : public golem::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		//PushOverlay(new golem::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

golem::Application* golem::CreateApplication()
{
	return new Sandbox();
}
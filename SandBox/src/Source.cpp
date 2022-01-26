#include "Golem.h"
#include "ImGui/imgui.h"
#include "Golem/Temp/KeyboardMovementController.h"
#include "Golem/FrameTimer.h"

#include "Golem/Render/RenderSystem/SimpleRenderSystem.h"
#include "Golem/Render/RenderSystem/PointLightRenderSystem.h"
#include "Golem/Render/RenderSystem/InfiniGridRenderSystem.h"

#include "Golem/Events/RenderSystemEvents.h"

using golem::Application;

struct CameraWrapper
{
	CameraWrapper() : gObject(golem::TempGameObject::Create()) {}

	golem::Camera camera{};
	golem::TempGameObject gObject;
	golem::KeyboardMovementController controller{};
};

class ExampleLayer : public golem::Layer
{
private:
	std::unique_ptr<golem::DescriptorPool> m_globalPool;
	std::unique_ptr<golem::DescriptorSetLayout> m_globalSetLayout;
	std::vector<VkDescriptorSet> m_globalDescriptorSets;

	std::vector<std::unique_ptr<golem::Buffer>> m_UBObuffers;

	std::unique_ptr<golem::SimpleRenderSystem> m_simpleRenderSystem;
	std::unique_ptr<golem::PointLightRenderSystem> m_pointLightRenderSystem;
	std::unique_ptr<golem::InfiniGridRenderSystem> m_infinigridRenderSystem;

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

		std::shared_ptr<golem::Model> cubeModel = golem::Model::CreateModelFromPrimative(device, golem::Primitives::Cube, false);
		auto cube = golem::TempGameObject::Create();
		cube.model = cubeModel;
		cube.transform.translation = { 1.5f,0.f,-.01f };
		m_gameObjects.push_back(std::move(cube));

		std::shared_ptr<golem::Model> quadModel = golem::Model::CreateModelFromPrimative(device, golem::Primitives::Quad);
		auto quad = golem::TempGameObject::Create();
		quad.model = quadModel;
		quad.transform.translation = { 0,0.75f,0 };
		quad.transform.scale = { 7,1,7 };
		m_gameObjects.push_back(std::move(quad));

		std::shared_ptr<golem::Model> vaseModel = golem::Model::CreateModelFromFile(device, "models/smooth_vase.obj");
		auto vase = golem::TempGameObject::Create();
		vase.model = vaseModel;
		vase.transform.translation = { -1.5f,0.5f,-.01f };
		vase.transform.scale = {5,5,5};
		m_gameObjects.push_back(std::move(vase));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for(int i = 0; i < lightColors.size(); i++)
		{
			auto pLight = golem::TempGameObject::CreatePointLight(1.0f, 0.05f, lightColors[i]);

			auto rotateLight = glm::rotate(glm::mat4(1.0f),(i*glm::two_pi<float>())/lightColors.size(), {0,-1,0});

			pLight.transform.translation = 1.5f* glm::vec3(rotateLight * glm::vec4(-1,-0.5, -1, 1));

			m_gameObjects.push_back(std::move(pLight));
		}

		// init ubo
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

		// init descriptor pools
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

		// init render systems
		m_simpleRenderSystem = std::make_unique<golem::SimpleRenderSystem>(
			device,
			m_globalSetLayout->GetDescriptorSetLayout());

		m_pointLightRenderSystem = std::make_unique<golem::PointLightRenderSystem>(
			device,
			m_globalSetLayout->GetDescriptorSetLayout());

		m_infinigridRenderSystem = std::make_unique<golem::InfiniGridRenderSystem>(device);
	}

	void OnPostRender() override
	{
		
	}

	void OnRender(VkCommandBuffer commandBuffer) override
	{
		m_timer.frame();

		// ------ update camera ------
		m_camera.controller.MoveInPlaneXZ(m_timer.getTime(), m_camera.gObject);
		m_camera.camera.SetViewXYZ(m_camera.gObject.transform.translation, m_camera.gObject.transform.rotation);

		float aspect = golem::Application::Get().GetRenderer().GetAspectRatio();
		m_camera.camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.0f);

		// ------ fill frame info struct ------
		golem::FrameInfo fInfo{
			golem::Application::Get().GetRenderer().GetFrameIndex(),
			m_timer.getTime(),
			commandBuffer,
			m_camera.camera,
			m_camera.gObject.transform.translation,
			m_globalDescriptorSets[golem::Application::Get().GetRenderer().GetFrameIndex()]
		};

		// ------ update uniform buffers ------
		golem::GlobalUBO ubo{};
		ubo.projection = m_camera.camera.GetProjection();
		ubo.view = m_camera.camera.GetView();

		m_pointLightRenderSystem->Update(fInfo, ubo, m_gameObjects);

		m_UBObuffers[fInfo.frameIndex]->WriteToBuffer(&ubo);
		m_UBObuffers[fInfo.frameIndex]->Flush();


		// ------ render scene ------
		m_simpleRenderSystem->RenderGameObjects(fInfo, m_gameObjects);
		m_pointLightRenderSystem->Render(fInfo, m_gameObjects);
		m_infinigridRenderSystem->Render(fInfo);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("test");

		if (ImGui::Button("Recompile Simple Shader") && !m_simpleRenderSystem->IsRegenerating())
		{
			Application::Get().GetThreadPool().Enqueue([=]{m_simpleRenderSystem->RuntimeCreatePipeline();});
		}

		if (ImGui::Button("Recompile Point Light Shader") && !m_pointLightRenderSystem->IsRegenerating())
		{
			Application::Get().GetThreadPool().Enqueue([=]{m_pointLightRenderSystem->RuntimeCreatePipeline();});
		}

		if (ImGui::Button("Recompile InfiniGrid Shader") && !m_infinigridRenderSystem->IsRegenerating())
		{
			Application::Get().GetThreadPool().Enqueue([=] {m_infinigridRenderSystem->RuntimeCreatePipeline(); });
		}

		ImGui::Checkbox("Render Grid", &m_infinigridRenderSystem->render);

		ImGui::End();
	}

	void OnEvent(golem::Event& e) override
	{
		golem::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<golem::ShaderReCompileEvent>(BIND_EVENT_FUNC(ExampleLayer::OnShaderRecompile));
	}

	bool OnShaderRecompile(golem::ShaderReCompileEvent& e)
	{
		e.GetRenderSystem()->CompleteRegeneration();

		return true;
	}
};

class Sandbox : public golem::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

golem::Application* golem::CreateApplication()
{
	return new Sandbox();
}
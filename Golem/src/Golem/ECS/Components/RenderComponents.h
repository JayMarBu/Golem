#pragma once
#include "Golem/Render/Objects/Model.h"
#include "Component.h"

namespace golem
{
	class MeshRendererComponent : public Component
	{
		// Members ********************************************************************************
	public:
		Ref<Model> model;

		// Methods ********************************************************************************
	public:
		MeshRendererComponent() = default;
		MeshRendererComponent(Ref<Model> m)
			: model{m} {}
		MeshRendererComponent(Model* m)
			: model{m}{}
		MeshRendererComponent(const MeshRendererComponent& other)
			: model{other.model} {}
	};

	struct PointLightComponent : public Component
	{
		float intensity = 1.0f;
		glm::vec3 colour = {1.0f,1.0f,1.0f};

		PointLightComponent() = default;
		PointLightComponent(float i, glm::vec3 c)
			: intensity{i}, colour{c}{}
	};
}
#pragma once
#include "../Render/Objects/Model.h"
#include "../Components/Transform.h"

namespace golem
{
	struct CPointLight
	{
		float lightIntensity = 1.0f;
	};

	class TempGameObject
	{
		// Members ********************************************************************************
	public:
		using id_t = unsigned int;

		Ref<Model> model = nullptr;
		glm::vec3 colour{};

		Ref<CPointLight> pointLight = nullptr;
		Transform transform{};

	private:

		id_t id;
		// Methods ********************************************************************************
	public:

		REMOVE_COPY_CONSTRUCTOR(TempGameObject);

		TempGameObject(TempGameObject&&) = default;
		TempGameObject& operator=(TempGameObject&&) = default;

		static TempGameObject Create()
		{
			static id_t currentId = 0;
			return TempGameObject(currentId++);
		}

		static TempGameObject CreatePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 colour = glm::vec3(1.0f));

		id_t getID() { return id; }

	private:
		TempGameObject(id_t objID) : id{ objID } {}
	};
}
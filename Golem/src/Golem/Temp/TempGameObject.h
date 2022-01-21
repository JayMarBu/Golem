#pragma once
#include "../Render/Objects/Model.h"

namespace golem
{
	struct CTransform
	{
	public:
		CTransform* parent;

		glm::vec3 translation{ 0.f,0.f,0.f };
		glm::vec3 rotation{ 0.f,0.f,0.f };
		glm::vec3 scale{ 1.f,1.f,1.f };

		glm::mat4 mat4();

		glm::mat3 invRotationMat();

		glm::mat3 normalMatrix();

		glm::mat4 positionRotMat();

		glm::mat4 scaleMat();

		operator glm::mat4() { return mat4(); }
	};

	class TempGameObject
	{
		// Members ********************************************************************************
	public:
		using id_t = unsigned int;

		std::shared_ptr<Model> model{};
		glm::vec3 colour{};
		CTransform transform{};

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

		id_t getID() { return id; }

	private:
		TempGameObject(id_t objID) : id{ objID } {}
	};
}
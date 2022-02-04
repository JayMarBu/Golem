#include "golpch.h"
#include "TempGameObject.h"

namespace golem
{

	golem::TempGameObject TempGameObject::CreatePointLight(float intensity /*= 10.0f*/, float radius /*= 0.1f*/, glm::vec3 colour /*= glm::vec3(1.0f)*/)
	{
		auto gObj = TempGameObject::Create();

		gObj.colour = colour;

		gObj.transform.scale.x = radius;

		gObj.pointLight = std::make_unique<CPointLight>();
		gObj.pointLight->lightIntensity = intensity;
		return gObj;
	}

}
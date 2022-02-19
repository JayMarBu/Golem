#include "golpch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Components/TagComponent.h"
#include "Components/Transform.h"
#include "Editor/GameObjectHierarchyEditor.h"

namespace golem
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
		m_registry.clear();
	}

	void Scene::OnUpdate()
	{
		// update transforms
	}

	class GameObject Scene::CreateGameObject(const std::string& name /*= std::string()*/)
	{
		GameObject e = { m_registry.create() };
		e.AddComponent<TagComponent>((name.empty()) ? "Unnamed GameObject" : name);
		e.AddComponent<Transform>();
		return e;
	}

	void Scene::DeleteGameObject(GameObject obj)
	{
		m_registry.destroy(obj);
	}

}
#pragma once
#include "entt.hpp"
#include "../Render/FrameInfo.h"

namespace golem
{
	class GameObject;
	class System;

	class Scene
	{
		// Members ********************************************************************************
	public:
		
		using SystemFunc = void (*)(::golem::GameObject);

	private:
		entt::registry m_registry;

		static Scene* g_workSpace;

		// Methods ********************************************************************************
	private:
		
	public:
		Scene();
		~Scene();

		void OnUpdate();
		
		GameObject CreateGameObject(const std::string& name = std::string());
		void DeleteGameObject(GameObject);

		friend class GameObject;
		friend class System;
		friend class GameObjectHierarchyEditor;
	};

	class System
	{
		// Members ********************************************************************************
	public:
		using SystemFunc = std::function<void(GameObject, FrameInfo&)>;
	private:
		SystemFunc m_func;

		// Members ********************************************************************************
	public:
		System(SystemFunc func) : m_func{func} {}

		template<typename T>
		void Run(Ref<Scene> scene, FrameInfo& fInfo)
		{
			auto view = scene->m_registry.view<T>();

			for (auto obj : view)
			{
				m_func(GameObject(obj), fInfo);
			}
		}

	private:

	};
}
#pragma once
#include "Golem/ECS/GameObject.h"
#include "Golem/Core.h"

namespace golem
{
	class GameObjectHierarchyEditor
	{
		// Members ********************************************************************************
	private:
		Ref<Scene> m_scene;

		GameObject m_selectedGameObject;
	public:
		static float s_labelWidth;
		// Methods ********************************************************************************
	public:
		void SetScene(Ref<Scene>);

		void Draw();

		void DrawHierarchyViewport();

		void DrawGameObject(GameObject obj, bool isRoot = true);

		void DrawComponentsViewport();

		void DrawGameObjectComponents(GameObject obj);
	private:
	};
}
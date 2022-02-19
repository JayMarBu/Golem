#include "golpch.h"
#include "GameObjectHierarchyEditor.h"

#include "imgui.h"
#include "Golem/ECS/Components/TagComponent.h"
#include "Golem/ECS/Components/RenderComponents.h"
#include "Golem/ECS/Components/Transform.h"

namespace golem
{

	float GameObjectHierarchyEditor::s_labelWidth;

	void GameObjectHierarchyEditor::SetScene(Ref<Scene> scene)
	{
		m_scene = scene;
	}

	void GameObjectHierarchyEditor::Draw()
	{
		DrawHierarchyViewport();
		DrawComponentsViewport();
	}

	void GameObjectHierarchyEditor::DrawHierarchyViewport()
	{
		ImGui::Begin("Scene Hierarchy");

		if(!m_scene)
		{
			ImGui::Text("No Scene Selected");
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
		{
			m_selectedGameObject = {};
		}

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			ImGui::Text("Create Game Objects");
			ImGui::Separator();
			ImGui::Indent(15);
			if (ImGui::MenuItem("Empty Game Object"))
			{
				GameObject::Create("Empty Game Object");
			}

			ImGui::Indent(-15);
			ImGui::EndPopup();
		}

		m_scene->m_registry.each([&](auto goID)
		{
			GameObject gobj{ goID };
			DrawGameObject(gobj);
		});

		ImGui::End();
	}

	void GameObjectHierarchyEditor::DrawGameObject(GameObject obj)
	{
		auto& tag = obj.GetComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((m_selectedGameObject == obj) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)obj, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_selectedGameObject = obj;
		}

		if (opened)
		{
			
			ImGui::TreePop();
		}
	}

	void GameObjectHierarchyEditor::DrawComponentsViewport()
	{
		ImGui::Begin("Game Object Properties");

		s_labelWidth = ImGui::GetColumnWidth() * 0.25;

		if (m_selectedGameObject)
			DrawGameObjectComponents(m_selectedGameObject);

		ImGui::End();
	}

#define DRAW_COMPONENT(T) \
		if(obj.HasComponent<T>())\
		{\
			ComponentEditorResolver<T>::Draw(obj);\
			ImGui::Spacing();\
		}

	void GameObjectHierarchyEditor::DrawGameObjectComponents(GameObject obj)
	{
		DRAW_COMPONENT(TagComponent)
		DRAW_COMPONENT(Transform)
		DRAW_COMPONENT(Camera)
		DRAW_COMPONENT(MeshRendererComponent)
		DRAW_COMPONENT(PointLightComponent)
	}

}
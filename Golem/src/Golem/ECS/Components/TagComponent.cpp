#include "golpch.h"
#include "TagComponent.h"

#include "Golem/ECS/GameObject.h"
#include "imgui.h"
#include "Golem/ImGui/imgui_stdlib.h"

namespace golem
{
	IMPLEMENT_COMPONENT_EDITOR(TagComponent, TagComponentEditor);

	REFLECT_STRUCT_BEGIN(TagComponent)
	REFLECT_STRUCT_END()

	void TagComponentEditor::Draw(GameObject gObj)
	{
		auto& component = gObj.GetComponent<TagComponent>();

		ImGui::TableSetColumnIndex(0);
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
		ImGui::Text("name");

		ImGui::TableSetColumnIndex(1);
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
		ImGui::InputText("##tag", &component.tag);
	}
}
#pragma once
#include "GameObjectHierarchyEditor.h"
#include "imgui.h"
#include "Golem/Reflection/Reflection.h"

namespace golem
{
	// Editor Class *******************************************************************************
	class ComponentEditor
	{
	private:
		ComponentEditor() : m_name {"unsupported component"} {};
		
		std::string m_name;

		template<typename T>
		void DrawEditor(class GameObject gObj, bool defaultEditor = false)
		{
			ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
			if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeFlags, m_name.c_str()))
			{
				ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV;
				if (ImGui::BeginTable(("##table_" + m_name).c_str(), (defaultEditor)?1:2, flags))
				{
					if(!defaultEditor) ImGui::TableSetupColumn("##one", ImGuiTableColumnFlags_WidthFixed, GameObjectHierarchyEditor::s_labelWidth);
					ImGui::TableNextRow();

					Draw(gObj);

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}

	public:
		ComponentEditor(const char* name) : m_name{ name } {};

		virtual void Draw(class GameObject gObj);

		friend class GameObjectHierarchyEditor;
		friend class DefaultComponentEditorResolver;
	};

	// Editor Resolver ****************************************************************************
	struct DefaultComponentEditorResolver
	{
		template <typename T> static char func(decltype(&T::CEditor));
		template <typename T> static int func(...);

		template <typename T>
		struct IsReflected
		{
			enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
		};

		// This version is called if T has a static member named "Reflection":
		template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
		static void Draw(class GameObject gObj)
		{
			T::CEditor->DrawEditor<T>(gObj);
		}

		// This version is called otherwise:
		template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
		static void Draw(class GameObject gObj)
		{
			//return GetDefaultComponentEditor();
			ComponentEditor e{TypeResolver<T>::Get()->GetName().c_str()};
			e.DrawEditor<T>(gObj, true);
		}
	};


	template <typename T>
	struct ComponentEditorResolver
	{
		static ComponentEditor* Get()
		{
			return DefaultComponentEditorResolver::Get<T>();
		}

		static void Draw(class GameObject gObj)
		{
			DefaultComponentEditorResolver::Draw<T>(gObj);
		}
	};

	// Macros *************************************************************************************
	#define COMPONENT_EDITOR(editorType) \
		friend struct ::golem::DefaultComponentEditorResolver; \
		friend class ::golem::editorType; \
		static ::golem::ComponentEditor* CEditor;

	#define IMPLEMENT_COMPONENT_EDITOR(type, editorType)\
		::golem::ComponentEditor* type::CEditor = new editorType(#type);
}
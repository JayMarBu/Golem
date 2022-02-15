#include <Golem.h>
#include <Golem/EntryPoint.h>

#include "EditorLayer.h"

namespace golem
{
	class GolemEditorApp : public Application
	{
	public:
		GolemEditorApp()
			: Application("Golem Editor")
		{
			PushLayer(new EditorLayer());
		}

		~GolemEditorApp()
		{

		}

	};

	Application* CreateApplication()
	{
		return new GolemEditorApp();
	}
}


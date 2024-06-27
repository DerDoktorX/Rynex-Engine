#include <Rynex.h>
#include <Rynex/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Rynex {

	class RynexEditor : public Application
	{
	public:
		RynexEditor()
			: Application("Rynex Editor")
		{
			PushLayer(new EditorLayer());
		}

		~RynexEditor()
		{

		}
	};
	
	Application* CreateApplication()
	{
		return new RynexEditor();
	}
}


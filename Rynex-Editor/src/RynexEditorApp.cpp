#include <rypch.h>
#include "Launcher/LauncherLayer.h"
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/EntryPoint.h>
#include <Rynex/Project/Project.h>

namespace Rynex {

	class RynexEditor : public Application
	{
	public:
		RynexEditor(const ApplicationSpecification& spec)
			: Application(spec)
		{
			if(spec.CommandLineArgs.Count > 1)
				PushLayer(new EditorLayer());
			else
				PushLayer(new LauncherLayer());
		}

		~RynexEditor()
		{
		}
		
	};
	
	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Rynex-Editor";
		spec.CommandLineArgs = args;
		Project::New();
		return new RynexEditor(spec);
	}
}


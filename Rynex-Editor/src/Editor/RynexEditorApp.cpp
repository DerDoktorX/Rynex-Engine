#include <rypch.h>
#include "EditorLayer.h"

#include <Editor/Launcher/LauncherLayer.h>
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
			if(1 < spec.CommandLineArgs.Count)
				PushLayer(CreateRef<EditorLayer>());
			else
				PushLayer(CreateRef<LauncherLayer>());
		}

		~RynexEditor()
		{
			RY_CORE_INFO("RynexEditor App Delete!");
		}
		
	};
	
	Scope<Application> CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Rynex-Editor";
		spec.CommandLineArgs = args;
		Project::New();
		return CreateScope<RynexEditor>(spec);
	}
}


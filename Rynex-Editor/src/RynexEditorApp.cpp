#include <Rynex/Core/Application.h>
#include <Rynex/Core/EntryPoint.h>

#include "EditorLayer.h"


namespace Rynex {

	class RynexEditor : public Application
	{
	public:
		RynexEditor(const ApplicationSpecification& spec)
			: Application(spec)
		{
			//PushLayer(new )
			PushLayer(new EditorLayer());
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

		Project::New()->SaveActive("SandboxTest.rproj");

		return new RynexEditor(spec);
	}
}


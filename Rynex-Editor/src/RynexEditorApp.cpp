#include <rypch.h>
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/EntryPoint.h>

namespace Rynex {

	class RynexEditor : public Application
	{
	public:
		RynexEditor(const ApplicationSpecification& spec)
			: Application(spec)
		{
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
#if RY_PATH_IN_LINE
		Project::New()->SaveActive("SandboxTest.rproj");
#endif
		return new RynexEditor(spec);
	}
}


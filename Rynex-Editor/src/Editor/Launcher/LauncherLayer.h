#pragma once
#include <Rynex/Core/Layer.h>
#include <Rynex/Events/MouseEvent.h>

namespace Rynex {

	class LauncherLayer : public Layer
	{
	public:
		LauncherLayer();
		virtual ~LauncherLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(TimeStep ts) override;

		virtual void OnEvent(Event& e) override;

		bool OnMousePressed(MouseButtenPressedEvent& e);

		virtual void OnImGuiRender() override;
	private:
		void SelecFolderPath();
		void ProjectNew();
		void CancelProjectNew();
		void OpenProject();
		void CreateProject();
	private:
		glm::uvec2 m_WindowSize = { 0 , 0 };
		std::string m_ProjectPath = "";
		std::string m_ProjectName = "";
		bool m_CreateNewProject = false;

		int m_OpenOrCreateFlags = 0;
	};

}

#pragma once

#include <Rynex/Events/Event.h>
#include <Rynex/Events/KeyEvent.h>
#include <Rynex/Events/MouseEvent.h>
#include <Rynex/Core/TimeStep.h>

namespace Rynex {
	class EditorLayer;

	class MenuBarPannel
	{
	public:
		MenuBarPannel();
		~MenuBarPannel();

		void OnAttache(EditorLayer* editorLayer);
		void OnDetache();

		void OnEvent(Event& e);
		void OpenWindow();
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtenPressedEvent& e);

		void OnUpdate(TimeStep ts);
		void OnImGuiRender();

	private:
		void File();
		void Project();
		void Script();
		void Edit();
		void View();
		void Help();
	private:
		EditorLayer* m_EditorLayer = nullptr;
	};
}


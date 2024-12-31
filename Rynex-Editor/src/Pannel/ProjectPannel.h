#pragma once
#include <Rynex/Events/Event.h>
#include <Rynex/Events/KeyEvent.h>
#include <Rynex/Events/MouseEvent.h>
#include <Rynex/Core/TimeStep.h>
#include <Rynex/Project/Project.h>

namespace Rynex {

	class EditorLayer;

	class ProjectPannel
	{
	public:
		ProjectPannel(const std::string& name = "Project");
		~ProjectPannel();

		void OnAttache(EditorLayer* editorLayer);
		void OnDetache();

		void OnEvent(Event& e);
		void OpenWindow();
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtenPressedEvent& e);

		void OnUpdate(TimeStep ts);
		void OnImGuiRender();	
		
		void SaveProject();
	public:
		enum class ProjectPathSelectDialog {
			Nono = 0,
			Directory,
			ProjectFile,
			ScriptFile,
			AssetRegestry,
			SceneAsset
		};
	private:
		void SetNewValue();
		void GetCurentValues();
	
	private:
		EditorLayer* m_EditorLayer = nullptr;
		std::string m_Name = "";
		bool m_WindowOpen = true;


		std::string m_StartScene = "";		// 0

		std::string m_ProjectName = "";		// 1
		std::string m_ProjectDir = "";		// 2
		std::string m_ProjectFile = "";		// 3

		std::string m_ScriptAppPath = "";	// 4
		std::string m_ScriptCorePath = "";	// 5

		std::string m_AssetPath = "";		// 6
		std::string m_AssetRegeistry = "";	// 7

		int m_ChangeValue = 0;
		bool m_Change = false;
		Ref<Project> m_CurentProject = nullptr;
	};
}


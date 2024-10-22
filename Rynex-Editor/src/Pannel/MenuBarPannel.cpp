#include <rypch.h>
#include "MenuBarPannel.h"
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#if RY_SCRIPTING_HAZEL
	#include <Rynex/Scripting/HazelScripting/ScriptEngine.h> 
#else
	#include <Rynex/Scripting/ScriptingEngine.h>
#endif
#include <imgui/imgui.h>

namespace Rynex {

	namespace Utils {
		static void ExiteButten()
		{
			if (ImGui::MenuItem("Exit", NULL, false)) Application::Get().Close();
		}
	}

	MenuBarPannel::MenuBarPannel()
	{
	}

	MenuBarPannel::~MenuBarPannel()
	{
	}

	void MenuBarPannel::OnAttache(EditorLayer* editorLayer)
	{
		m_EditorLayer = editorLayer;
	}

	void MenuBarPannel::OnDetache()
	{
		m_EditorLayer = nullptr;
	}

	void MenuBarPannel::OnEvent(Event& e)
	{
	}

	void MenuBarPannel::OpenWindow()
	{
	}

	bool MenuBarPannel::OnKeyPressed(KeyPressedEvent& e)
	{
		return false;
	}

	bool MenuBarPannel::OnMousePressed(MouseButtenPressedEvent& e)
	{
		return false;
	}

	void MenuBarPannel::OnUpdate(TimeStep ts)
	{
	}

	void MenuBarPannel::OnImGuiRender()
	{
		if (ImGui::BeginMenuBar())
		{
			File();
			Project();
			Script();
			Edit();
			View();
			Help();
			ImGui::EndMenuBar();
		}
	}

	void MenuBarPannel::File()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Create Project", NULL, false))
				m_EditorLayer->NewProject();

			if (ImGui::MenuItem("Open Project", "Crtl+O"))
				m_EditorLayer->OpenProject();

			if (ImGui::MenuItem("Save Project...", "Crtl+S"))
				m_EditorLayer->SaveProject();

			ImGui::EndMenu();
		}
	}

	void MenuBarPannel::Project()
	{
		if (ImGui::BeginMenu("Project"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.

			if (ImGui::MenuItem("New Scene", NULL, false))
				m_EditorLayer->NewScene();

			if (ImGui::MenuItem("Open Scene...", "Crtl+O"))
				m_EditorLayer->OpenScene();

			if (ImGui::MenuItem("SaveAs Scene...", "Crtl+S"))
				m_EditorLayer->SaveSceneAs();


			ImGui::EndMenu();
		}
	}

	void MenuBarPannel::Script()
	{
		if (ImGui::BeginMenu("Script"))
		{
			if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
#if RY_SCRIPTING_HAZEL
				ScriptEngine::ReloadAssembly();
#else
				ScriptingEngine::ReloadAssambly();
#endif
			ImGui::EndMenu();
		}
	}

	void MenuBarPannel::Edit()
	{
		if (ImGui::BeginMenu("Edite"))
		{
			Utils::ExiteButten();
			ImGui::EndMenu();
		}
	}

	void MenuBarPannel::View()
	{
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Renderer"))
				m_EditorLayer->OpenRenderPannel();
			if (ImGui::MenuItem("Asset-Pannel"))
				m_EditorLayer->OpenAssetPannel();
			if (ImGui::MenuItem("Regestriy-Pannel"))
				m_EditorLayer->OpenRegestriyPannel();
			if (ImGui::MenuItem("Scene-Hierachy"))
				m_EditorLayer->OpenSceneHierachy();
			if (ImGui::MenuItem("Properties"))
				m_EditorLayer->OpenProperties();
			Utils::ExiteButten();
			ImGui::EndMenu();
		}
	}

	void MenuBarPannel::Help()
	{
		if (ImGui::BeginMenu("Help"))
		{
			Utils::ExiteButten();
			ImGui::EndMenu();
		}
	}
}
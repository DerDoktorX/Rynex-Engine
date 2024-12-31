#include <rypch.h>
#include "LauncherLayer.h"
#include "../EditorLayer.h"
#include <Rynex/Core/Application.h>
#include <Rynex/Utils/PlatformUtils.h>
#include <Rynex/Project/Project.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Rynex {

	namespace Utils {
		void InputText(const std::string& name, std::string& value, char* buffer, uint32_t buferSize)
		{
			ImGui::Text(name.c_str());
			strcpy(buffer, value.c_str());
			if (ImGui::InputText(("##" + name).c_str(), buffer, buferSize))
				value = buffer;
		}
	}

	LauncherLayer::LauncherLayer()
	{
	}

	void LauncherLayer::OnAttach()
	{
		m_WindowSize = {3/1920, 3/1080};

	}

	void LauncherLayer::OnDetach()
	{
	}

	void LauncherLayer::OnUpdate(TimeStep ts)
	{
	}

	void LauncherLayer::OnEvent(Event& e)
	{
	}

	bool LauncherLayer::OnMousePressed(MouseButtenPressedEvent& e)
	{
		return false;
	}

	void LauncherLayer::OnImGuiRender()
	{
        static bool dokingEnabled = true;
        static bool assetsEnabled = true;


        if (dokingEnabled)
        {

            static bool dokingSpaceOpen = true;
            static bool opt_fullscreen = true;
            static bool opt_padding = false;

            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }


            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Rynex-Launcher-GUI", &dokingSpaceOpen, window_flags);
            ImGui::PopStyleVar();
			
			
            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
			
			ImGui::Begin("Rynex-Launcher");
			static char buffer[260];

			Utils::InputText("Name-Project: ", m_ProjectName, buffer, sizeof(buffer));

			Utils::InputText("Location", m_ProjectPath, buffer, sizeof(buffer));

			ImGui::SameLine();

			if (ImGui::Button("...", { 30,22.5 }))
				SelecFolderPath();
			
			if (m_CreateNewProject)
			{
				bool active = m_ProjectPath == "" || m_ProjectName == "";
				if (active)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				if (ImGui::Button("Create Project", { 100,22.5 }))
					CreateProject();
				if (active)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
				ImGui::SameLine();

				if (ImGui::Button("Cancel", { 100,22.5 }))
					CancelProjectNew();
			}
			else
			{
				if (ImGui::Button("New Project", { 100,22.5 }))
					ProjectNew();
				bool active = m_ProjectPath == "" || m_ProjectName == "";
				if (active)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				if (ImGui::Button("Open Project", { 100,22.5 }))
					OpenProject();
				if (active)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
				ImGui::SameLine();

				if (ImGui::Button("Cancel", { 100,22.5 }))
					Application::Get().Close();
			}
			ImGui::End();
			ImGui::End();
		}
		
	}

	void LauncherLayer::SelecFolderPath()
	{
		if(m_CreateNewProject)
			m_ProjectPath = FileDialoges::SelectFolder();
		else
		{
			std::filesystem::path filepath = FileDialoges::OpenFile("Rynex Project (*.rproj)\0*.rproj\0");
			m_ProjectPath = filepath.parent_path().string();
			m_ProjectName = filepath.filename().string();
		}
	}

	void LauncherLayer::ProjectNew()
	{
		m_CreateNewProject = true;
		m_ProjectPath = "";
		m_ProjectName = "";
	}

	void LauncherLayer::CancelProjectNew()
	{
		m_CreateNewProject = false;
		m_ProjectPath = "";
		m_ProjectName = "";
	}

	void LauncherLayer::OpenProject()
	{
		if (m_ProjectPath == "" || m_ProjectName == "")
		{
			
			return;
		}
		Application::Get().SubmiteToMainThreedQueue([&]() {

			ApplicationSpecification& spec = Application::Get().GetSpecification();
			std::string path = m_ProjectPath + "\\" + m_ProjectName;
			spec.CommandLineArgs.Count += path.size();
			spec.CommandLineArgs.Args[1] = (char*)path.c_str();
			Application::Get().PushLayer(new EditorLayer());
			Application::Get().PopLayer(this);
		});
	}

	void LauncherLayer::CreateProject()
	{
		if (m_ProjectPath == "" || m_ProjectName == "")
		{
			
			return;
		}
		Application::Get().SubmiteToMainThreedQueue([&](){
			

			Ref<Project> project = Project::GetActive()->CreatNewPorject(m_ProjectPath, m_ProjectName);
			ApplicationSpecification& spec = Application::Get().GetSpecification();
			std::string path = (project->GetConfig().ProjectPath / m_ProjectName).string();
			spec.CommandLineArgs.Args++;
			spec.CommandLineArgs.Args[1] = (char*)path.c_str();
			Application::Get().PushLayer(new EditorLayer());
			Application::Get().PopLayer(this);
		});
	}

	
}
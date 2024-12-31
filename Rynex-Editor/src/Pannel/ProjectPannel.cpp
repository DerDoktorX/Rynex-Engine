#include <rypch.h>
#include "ProjectPannel.h"

#include "EditorLayer.h"
#include <Rynex/Utils/PlatformUtils.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Rynex {

    namespace Utils {

        bool InputText(const std::string& name, std::string& value, char* buffer, uint32_t buferSize, int *aktiveValue,int index,ProjectPannel::ProjectPathSelectDialog pathDLG)
        {
            ImGuiIO& io = ImGui::GetIO();
            auto boldFont = io.Fonts->Fonts[0];

            ImGui::PushID(name.c_str());

            ImGui::Text(name.c_str());
            strcpy(buffer, value.c_str());
            bool aktive = BIT_EQUAL(*aktiveValue, BIT(index));
            
            if (!aktive)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            bool result = (ImGui::InputText("##Path", buffer, buferSize));
            if (!aktive)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
            if(result)
                value = buffer;
                    
            ImGui::SameLine();
            if (ImGui::Checkbox("##CheckBox", &aktive))
                *aktiveValue = BIT_SET_ON(aktive, BIT(index), *aktiveValue);
            ImGui::SameLine();
           
            if(!aktive)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button("...", {30,22.5}))
            {
                switch (pathDLG)
                    {
                    case ProjectPannel::ProjectPathSelectDialog::Directory:
                    {
                        std::string result = FileDialoges::SelectFolder();
                        value = result != "" ? result : value;
                        break;
                    }
                    case ProjectPannel::ProjectPathSelectDialog::ProjectFile:
                    {
                        std::string result = FileDialoges::OpenFile("Rynex Project (*.rproj)\0*.rproj\0");
                        value = result != "" ? result : value;
                        break;
                    }
                    case ProjectPannel::ProjectPathSelectDialog::ScriptFile:
                    {
                        std::string result = FileDialoges::OpenFile("Scripting Binaries (*.dll)\0*.dll\0");
                        value = result != "" ? result : value;
                        break;
                    }
                    case ProjectPannel::ProjectPathSelectDialog::AssetRegestry:
                    {
                        std::string result = FileDialoges::OpenFile("Asset Registry(*.ryr)\0*.ryr\0");
                        value = result != "" ? result : value;
                        break;
                    }
                    case ProjectPannel::ProjectPathSelectDialog::SceneAsset:
                    {

                        std::string result = FileDialoges::OpenFile("Asset Registry(*.ryscene)\0*.ryscene\0");
                        value = result != "" ? result : value;
                        break;
                    }
                    default:
                        RY_CORE_ERROR("No such Project Selcter Avible");
                        break;
                    }
                
            }
            if (!aktive)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
            ImGui::PopID();
            return result;
        }
        
        bool InputSceneText(const std::string& name, std::string& value, char* buffer, uint32_t buferSize, int* aktiveValue, int index, ProjectPannel::ProjectPathSelectDialog pathDLG)
        {
            ImGuiIO& io = ImGui::GetIO();
            auto boldFont = io.Fonts->Fonts[0];

            ImGui::PushID(name.c_str());

            ImGui::Text(name.c_str());
            strcpy(buffer, value.c_str());
            bool aktive = BIT_EQUAL(*aktiveValue, BIT(index));

            if (!aktive)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            bool result = (ImGui::InputText("##Path", buffer, buferSize));

            if (result)
                value = buffer;

            if(aktive)
            {
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(AssetType::Scene).c_str()))
                    {
                        Ref<Project> project = Project::GetActive();
#if RY_EDITOR_ASSETMANGER_THREADE
                        Ref<EditorAssetManegerThreade> editorAssetManager = project->GetEditorAssetManger();
                        const AssetMetadata metadata = editorAssetManager->GetMetadata(*(AssetHandle*)payload->Data);
#else
                        Ref<EditorAssetManager> editorAssetManager = project->GetEditorAssetManger();
                        AssetMetadata& metadata = editorAssetManager->GetMetadata(*(AssetHandle*)payload->Data);
#endif
                        
                        const std::filesystem::path& path = metadata.FilePath;
                        std::string& pathStr = path.string();
                        value = pathStr.c_str();
                        result = true;
                    }
                    ImGui::EndDragDropTarget();
                }
            }
            
            if (!aktive)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
           
            ImGui::SameLine();

            if (ImGui::Checkbox("##CheckBox", &aktive))
                *aktiveValue = BIT_SET_ON(aktive, BIT(index), *aktiveValue);

            ImGui::SameLine();

            if (!aktive)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }

            if (ImGui::Button("...", { 30,22.5 }))
            {
                switch (pathDLG)
                {
                case ProjectPannel::ProjectPathSelectDialog::Directory:
                {
                    std::string result = FileDialoges::SelectFolder();
                    value = result != "" ? result : value;
                    break;
                }
                case ProjectPannel::ProjectPathSelectDialog::ProjectFile:
                {
                    std::string result = FileDialoges::OpenFile("Rynex Project (*.rproj)\0*.rproj\0");
                    value = result != "" ? result : value;
                    break;
                }
                case ProjectPannel::ProjectPathSelectDialog::ScriptFile:
                {
                    std::string result = FileDialoges::OpenFile("Scripting Binaries (*.dll)\0*.dll\0");
                    value = result != "" ? result : value;
                    break;
                }
                case ProjectPannel::ProjectPathSelectDialog::AssetRegestry:
                {
                    std::string result = FileDialoges::OpenFile("Asset Registry(*.ryr)\0*.ryr\0");
                    value = result != "" ? result : value;
                    break;
                }
                case ProjectPannel::ProjectPathSelectDialog::SceneAsset:
                {

                    std::string result = FileDialoges::OpenFile("Asset Registry(*.ryscene)\0*.ryscene\0");
                    value = result != "" ? result : value;
                    break;
                }
                default:
                    RY_CORE_ERROR("No such Project Selcter Avible");
                    break;
                }

            }

            if (!aktive)
            {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }

            ImGui::PopID();
            return result;
        }
    }

    ProjectPannel::ProjectPannel(const std::string& name)
        : m_Name(name)
    {
    }

    ProjectPannel::~ProjectPannel()
    {
    }

    void ProjectPannel::OnAttache(EditorLayer* editorLayer)
    {
        m_EditorLayer = editorLayer;
        GetCurentValues();
    }

    void ProjectPannel::OnDetache()
    {
        m_EditorLayer = nullptr;
    }

    void ProjectPannel::OnEvent(Event& e)
    {
    }

    void ProjectPannel::OpenWindow()
    {
        m_WindowOpen = true;
    }

    bool ProjectPannel::OnKeyPressed(KeyPressedEvent& e)
    {
        return false;
    }

    bool ProjectPannel::OnMousePressed(MouseButtenPressedEvent& e)
    {
        return false;
    }

    void ProjectPannel::OnUpdate(TimeStep ts)
    {
    }

    void ProjectPannel::OnImGuiRender()
    {
        if (m_WindowOpen)
        {
            
            if(ImGui::Begin(m_Name.c_str(), &m_WindowOpen, ImGuiWindowFlags_None))
            {
                if (m_CurentProject != Project::GetActive())
                    GetCurentValues();
                static char buffer[260];
                m_Change = Utils::InputSceneText("Start Scene", m_StartScene, buffer, sizeof(buffer), &m_ChangeValue, 0, ProjectPathSelectDialog::SceneAsset) || m_Change;
                m_Change = Utils::InputText("Project Name", m_ProjectName, buffer, sizeof(buffer), &m_ChangeValue, 1, ProjectPathSelectDialog::ProjectFile) || m_Change;
                m_Change = Utils::InputText("Project Directory", m_ProjectDir, buffer, sizeof(buffer), &m_ChangeValue, 2, ProjectPathSelectDialog::Directory) || m_Change;
                if (Utils::InputText("Project File", m_ProjectFile, buffer, sizeof(buffer), &m_ChangeValue, 3, ProjectPathSelectDialog::Directory))
                {
                    m_ProjectDir = std::filesystem::path(m_ProjectFile).parent_path().string();
                    m_ProjectName = std::filesystem::path(m_ProjectFile).filename().string();
                    m_Change = true;
                }
                m_Change = Utils::InputText("Script App", m_ScriptAppPath, buffer, sizeof(buffer), &m_ChangeValue, 4, ProjectPathSelectDialog::ScriptFile) || m_Change;
                m_Change = Utils::InputText("Script Core", m_ScriptCorePath, buffer, sizeof(buffer), &m_ChangeValue, 5, ProjectPathSelectDialog::ScriptFile) || m_Change;
                m_Change = Utils::InputText("Asset Path", m_AssetPath, buffer, sizeof(buffer), &m_ChangeValue, 6, ProjectPathSelectDialog::Directory) || m_Change;
                m_Change = Utils::InputText("Asset Regiestry", m_AssetRegeistry, buffer, sizeof(buffer), &m_ChangeValue, 7, ProjectPathSelectDialog::AssetRegestry) || m_Change;

                if (m_Change)
                {
                    if (ImGui::Button("Save"))
                        SetNewValue();
                    if (ImGui::Button("Chanel"))
                        GetCurentValues();
                }
            }
            ImGui::End();
        }
    }

    void ProjectPannel::SaveProject()
    {
        if (m_EditorLayer != nullptr)
            m_EditorLayer->SaveProject();
    }

    void ProjectPannel::GetCurentValues()
    {
        m_CurentProject = Project::GetActive();
        auto& config = m_CurentProject->GetConfig();
        m_StartScene = config.StartScene.string();
        m_ProjectName = config.Name;
        m_ProjectDir = config.ProjectPath.string();
        m_ProjectFile = (config.ProjectPath / config.Name).string();
        m_ScriptCorePath = config.ScriptCorePath.string();
        m_ScriptAppPath = config.ScriptAppPath.string();
        m_AssetPath = config.AssetDirectory.string();
        m_AssetRegeistry = config.AssetRegistryPath.string();        m_ChangeValue = 0;
        m_Change = false;
    }

    void ProjectPannel::SetNewValue()
    {
        m_CurentProject = Project::GetActive();
        auto& config = m_CurentProject->GetConfig();
        config.StartScene = m_StartScene;
        config.Name = m_ProjectName;
        config.ProjectPath = m_ProjectDir;
        config.ScriptCorePath = m_ScriptCorePath;
        config.ScriptAppPath = m_ScriptAppPath; 
        config.AssetDirectory = m_AssetPath;
        config.AssetRegistryPath = m_AssetRegeistry;
        SaveProject();
        m_ChangeValue = 0;
        m_Change = false;

    }

}
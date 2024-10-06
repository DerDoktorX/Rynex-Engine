#include "rypch.h"
#include "Project.h"

#include "Rynex/Serializers/ProjectSerialiazer.h"


namespace Rynex {

    Ref<Project> Project::New()
    {
        s_ActiveProject = CreateRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        RY_CORE_INFO("Load Project from Path: '{0}'",path.string().c_str());
        Ref<Project> project = CreateRef<Project>();

        ProjectSerialiazer serialiazer(project);
        if (serialiazer.Deserlize(path))
        {
            project->m_Config.ProjectPath = path.parent_path();
            s_ActiveProject = project;
            Ref<EditorAssetManager> editorAssetManager = CreateRef<EditorAssetManager>();
            s_ActiveProject->m_AssetManger = editorAssetManager; 
            
            editorAssetManager->DeserialzeAssetRegistry();
           
            return s_ActiveProject;
        }
        RY_CORE_ERROR("Project Loading Faild");
        return nullptr;
    }

    bool Project::SaveActive(const std::filesystem::path& path)
    {
        ProjectSerialiazer serializer(s_ActiveProject);
        if (serializer.Serlize(path))
        {
            s_ActiveProject->m_Config.ProjectPath = path.parent_path();
            //s_ActiveProject->m_AssetManger->OnDetach();
            return true;
        }

        return false;
    }



}


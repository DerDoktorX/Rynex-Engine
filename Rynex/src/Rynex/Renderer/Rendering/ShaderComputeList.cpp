#include "rypch.h"
#include "ShaderComputeList.h"
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex {
    void ShaderComputeList::Sort()
    {
        m_BindingLayout->Sort();
    }

    void ShaderComputeList::Clear()
    {
        RY_DESTROY_REF(m_Shader);
        RY_DESTROY_REF(m_DispatchBuffer);


        m_BindingLayout->Clear();
        RY_DESTROY_REF(m_BindingLayout);
    }

} // Rynex
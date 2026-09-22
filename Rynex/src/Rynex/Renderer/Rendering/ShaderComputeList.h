#pragma once

#include <Rynex/Renderer/Rendering/ShaderDrawList.h>
namespace Rynex {
    struct BindingShaderLayoutStatic;

    struct ShaderComputeList
    {
        Ref<Shader> m_Shader;
        Ref<IndirectBuffer> m_DispatchBuffer;
        glm::uvec3 m_DispatchGroups;

        Ref<BindingShaderLayoutStatic> m_BindingLayout;


        void Sort();
        void Clear();

    };
} // Rynex



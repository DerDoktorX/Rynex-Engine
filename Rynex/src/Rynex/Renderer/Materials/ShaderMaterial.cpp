#include "rypch.h"
#include "ShaderMaterial.h"

#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

    ShaderMaterial::ShaderMaterial(const ShaderMaterialDefaultNames& names)
        : m_Names(names)
        , m_Flags(0)
    {
    }

    ShaderMaterial::~ShaderMaterial()
    {
        m_Shader.reset();
        m_Uniform.reset();
    }

    void ShaderMaterial::SetColor(const glm::vec3& color)
    {
        SetDefaultName(m_Names.Alpha, glm::value_ptr(color));
    }

    void ShaderMaterial::SetAlpha(float v)
    {
        SetDefaultName(m_Names.Alpha, &v);
    }

    

    void ShaderMaterial::SetShader(const Ref<Shader>& shader)
    {
        m_Shader = shader;
    }

    const Ref<Shader>& ShaderMaterial::GetShader() const
    {
        return m_Shader;
    }

    void ShaderMaterial::SetFlage(int flage)
    {
        m_Flags = flage;
    }

    int ShaderMaterial::GetFlage()
    {
        return m_Flags;
    }

    void ShaderMaterial::SetMatrix(const glm::mat4& matrix)
    {
        SetDefaultName(m_Names.ModelMatrix, glm::value_ptr(matrix));
    }

    void ShaderMaterial::SetShadowTextures(const std::array<Ref<Texture>, 72>& shadowsTex, uint32_t size)
    {
        RY_CORE_ASSERT(false);
    }

    void ShaderMaterial::BindShadow()
    {
        RY_CORE_ASSERT(false);
    }

    void ShaderMaterial::Bind(int* entityIDs, uint32_t size)
    {
        
        m_Shader->Bind();
        m_Uniform->Bind();
    }

    void ShaderMaterial::UnBind()
    {
        m_Shader->UnBind();
    }

    void ShaderMaterial::InitAsync()
    {
        RY_CORE_ASSERT(false);
    }

    void ShaderMaterial::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer)
    {
        m_Uniform = uniformBuffer;
    }

    void ShaderMaterial::SetUniformValue(const BufferElement& element, void* data)
    {
        m_Uniform->SetLocelData(element, data, element.Size);
    }

    const Ref<UniformBuffer>& ShaderMaterial::GetUniformBuffer() const
    {
        return m_Uniform;
    }


    void ShaderMaterial::SetUniforme(ShaderMaterialDefaultNames names)
    {
        m_Names = names;
    }

    const ShaderMaterialDefaultNames& ShaderMaterial::GetUniforme() const
    {
        return m_Names;
    }

    Ref<Shader> ShaderMaterial::GetShadowShader()
    {
        return m_ShadowShader;
    }

    void* ShaderMaterial::GetUniformData()
    {
        return (void*)m_Uniform->GetBufferData().data();
    }

    void ShaderMaterial::SetDefaultName(BufferElement& elementName, const void* data)
    { 
        if(elementName.Name != "" && m_Uniform)
        {
            if (elementName.Offset == 0 || elementName.Size == 0)
            {
                 const BufferLayout& layoute = m_Uniform->GetLayout();
                 for (const BufferElement& element : layoute)
                 {
                     if (element == elementName)
                     {
                         elementName.Offset = element.Offset;
                         elementName.Size = element.Size;
                         m_Uniform->SetData(data, elementName.Offset, elementName.Size);
                         break;
                     }
                 }
            }
            else
            {
                m_Uniform->SetData(data, elementName.Offset, elementName.Size);
            }
        }
    }

}
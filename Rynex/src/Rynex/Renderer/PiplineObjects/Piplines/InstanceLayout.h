#pragma once
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/InstanceMeshPiplineRenderBase.h>

namespace Rynex {

    // Axis 1: instance data layout
    struct ShadeInstanceLayout
    {
        struct RenderObject
        {
            glm::mat4 ModelMatrix;
            glm::mat4 NormalMatrix;
            int EntityID;
            int Empty[3];
            // bool operator!=(const RenderObject&) const; // change-detection reuse
        };

        static RenderObject Make(const glm::mat4& model, int entityID)
        {
            return { model, glm::transpose(glm::inverse(model)), entityID, { -10, -11, -12 } };
        }

        static BufferLayout GetVertexLayout()
        {
            return BufferLayout({
                { SDT::Float4x4, "a_ModelMarix" },
                { SDT::Float4x4, "a_NormleMatrix" },
                { SDT::Int,      "a_EntityID" },
                { SDT::Int3,     "a_Empty" }
            }, /*instanceIncrease*/ 1u);
        }
    };

    struct DepthInstanceLayout
    {
        struct RenderObject { glm::mat4 m_ModelMatrix; };
        static RenderObject Make(const glm::mat4& model, int) { return { model }; }
        static BufferLayout GetVertexLayout()
        {
            return BufferLayout({
                { SDT::Float4, "a_ModelMarix[0]" },
                { SDT::Float4, "a_ModelMarix[1]" },
                { SDT::Float4, "a_ModelMarix[2]" },
                { SDT::Float4, "a_ModelMarix[3]" },
            }, 1u);
        }
    };

    // Axis 2: resource binding
    struct ShadeResources
    {
        enum
        {
            //
            TextureBinding_Albedo = 0,
            TextureBinding_Shadow = 1,

            UniformBinding_LightCamera = 2,
            UniformBinding_Materiel = 3,

        };

        Ref<UniformBuffer> m_MaterielUB, m_LightUB;
        Ref<Texture> m_Albedo, m_Shadow;

        void AcquireFrom(const Ref<Material>& material)
        {
            m_MaterielUB = material->GetMaterielUniformBuffer();
            m_Albedo = material->GetAlbedoTextures();
        }

        void Bind()
        {
            m_MaterielUB->Bind(UniformBinding_Materiel);
            m_LightUB->Bind(UniformBinding_LightCamera);
            m_Albedo->Bind(TextureBinding_Albedo);
            m_Shadow->Bind(TextureBinding_Shadow);
        }
        void Unbind() { /* mirror */ }
        void Clear()
        {
            RY_DESTROY_REF(m_MaterielUB);
            RY_DESTROY_REF(m_Albedo);
            RY_DESTROY_REF(m_Shadow);
        }
    };

    struct DepthResources
    {


        Ref<UniformBuffer> m_LightUB;
        void AcquireFrom(const Ref<Material>& material) {}
        void Bind()
        {
            m_LightUB->Bind(InstanceMeshPiplineRenderBase::UniformBinding_MainCamera);
        }
        void Unbind() { /* mirror */ }
        void Clear()  {}
    };

    // Axis 3: geometry selection
    struct ShadeGeometry
    {
        static const Ref<IndexBuffer>& IndexBuffer(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetShadeIndexBuffer(); }
        static const Mesh::PerDrawObject& DrawObject(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetShadePerDrawObjectIndirect(); }
        static bool CheckVAO(Ref<VertexArray>& vao, Ref<MeshSingle>& meshSingle)  { return PiplineRenderBase::CheckVAOFromMeshSingleShade(vao, meshSingle); }
    };
    struct DepthGeometry
    {
        static const Ref<IndexBuffer>& IndexBuffer(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetDepthIndexBuffer(); }
        static const Mesh::PerDrawObject& DrawObject(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetDepthPerDrawObjectIndirect(); }
        static bool CheckVAO(Ref<VertexArray>& vao, Ref<MeshSingle>& meshSingle) { return PiplineRenderBase::CheckVAOFromMeshSingleDepth(vao, meshSingle); }
    };
}

#pragma once

namespace Rynex {
    namespace Shade {
        struct InstanceLayout
        {
            struct RenderObject
            {
                glm::mat4 m_ModelMatrix;
                glm::mat4 m_NormalMatrix;
                int m_EntityID;
                int m_Empty[3];

                RenderObject()
                    : m_ModelMatrix(glm::mat4(-1.0f))
                    , m_NormalMatrix(glm::mat4(-1.0f))
                    , m_EntityID(-1)
                    , m_Empty{ -10, -11, -12 }
                {
                }

                RenderObject(const glm::mat4& modelMatrix, const glm::mat4& normalMatrix, int entityID, int empty[3])
                    : m_ModelMatrix(modelMatrix)
                    , m_NormalMatrix(normalMatrix)
                    , m_EntityID(entityID)
                    , m_Empty{ *empty }
                {
                }
            };

            static bool Equal(const RenderObject& object, const glm::mat4& model, const int entityID)
            {
                return object.m_EntityID == entityID && model == object.m_ModelMatrix;
            }

            static RenderObject Make(const glm::mat4& model, const int entityID)
            {
                const glm::mat4 inverse = glm::inverse(model);
                const glm::mat4 normalMatrix = glm::transpose(inverse);
                constexpr int empty[3]{ -10, -11, -12 };
                RenderObject renderObject;
                renderObject.m_ModelMatrix = model;
                renderObject.m_EntityID = entityID;
                renderObject.m_NormalMatrix = normalMatrix;
                renderObject.m_Empty[0] = empty[0];
                renderObject.m_Empty[1] = empty[1];
                renderObject.m_Empty[2] = empty[2];

                return renderObject;
            }


            static BufferLayout GetVertexLayout()
            {
                constexpr uint32_t instanceIndex = 1u;
                constexpr bool active = true;
                BufferLayout layout({
                    { SDT::Float4x4, "a_ModelMatrix" },
                    { SDT::Float4x4, "a_NormaleMatrix" },
                    { SDT::Int, "a_EntityID" },
                    { SDT::Int3, "a_Empty" }
                }, instanceIndex);
                layout.SetAutoCompress(true);
                return layout;
            }


        };

        struct Geometry
        {
            static const Ref<IndexBuffer>& GetIndexBuffer(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetShadeIndexBuffer(); }
            static const Mesh::PerDrawObject& GetDrawObject(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetShadePerDrawObjectIndirect(); }

            static bool HasVAOFromMeshSingleIndexBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
            {
                RY_CORE_ASSERT(nullptr != meshSingle, "Mesh is not valid!");
                RY_CORE_ASSERT(nullptr != vao, "VertexArrayObject is not valid!");

                const Ref<IndexBuffer>& iabShade = meshSingle->GetShadeIndexBuffer();
                const Ref<IndexBuffer>& iab = vao->GetIndexBuffer();

                return iabShade == iab;
            }
        };

        struct Resources
        {
            enum
            {
                TextureBinding_Albedo = 0,
                TextureBinding_Shadow = 1,

                UniformBinding_LightCamera = 2,
                UniformBinding_Materiel = 3,
            };

            Ref<UniformBuffer> m_MaterielUB;
            Ref<UniformBuffer> m_LightUB;
            Ref<Texture> m_Albedo;
            Ref<Texture> m_Shadow;

            Resources() = default;
            Resources(const Resources&) = default;


            void SubmitRenderTargetResource(ViewPassStorage& viewPass)
            {
                if (nullptr == m_Shadow || nullptr == m_LightUB)
                {
                    CamerRenderPackages& viewPassPackege = viewPass.m_CameraPackege;
                    CamerRenderPackages::CamerPackage& cameraPackage = viewPassPackege.GetCamerPackage();
                    const Ref<UniformBuffer>& cameraBuffer = cameraPackage.GetBuffer();
                    m_LightUB = cameraBuffer;

                    const RenderTarget& target = viewPassPackege.GetRenderTarget();
                    const Ref<Framebuffer>& fb = target.GetFramebuffer();
                    const Ref<Texture>& shadow = fb->GetDepthTexture();
                    m_Shadow = shadow;
                }
            }

            void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer)  { m_LightUB = buffer; }

            void SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture) { m_Shadow = texture; }

            int CheckObject(const Ref<Material>& materielTest, Ref<Material>& materialThis)
            {
                int result = Result_None;

                InstanceMeshPiplineRenderBase::CheckObject(materialThis, materielTest, result, Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);
                if (nullptr != materielTest)
                {
                    const Ref<Texture> texture = materielTest->GetAlbedoTextures();
                    InstanceMeshPiplineRenderBase::CheckObject(m_Albedo, texture, result, Result_NotAllowedTexture, Result_NoTextureSpaceLeft);
                }
                return result;
            }

            uint64_t GetStorageBufferNumber() const { return 0u; }

            uint64_t GetUniformBufferNumber() const
            {
                uint64_t number = 0ull;
                const uint64_t numberMateriel = reinterpret_cast<uint64_t>(m_MaterielUB.get());
                const uint64_t numberLight = reinterpret_cast<uint64_t>(m_LightUB.get());
                constexpr uint64_t bitsPerBinding = PiplineRenderBase::Hash_BindingPointMultiplyNumberBitMove;
                number |= numberMateriel << (UniformBinding_LightCamera * bitsPerBinding);
                number |= numberLight << (UniformBinding_Materiel * bitsPerBinding);
                return number;
            }

            uint64_t GetIndirectBufferNumber() const { return 0u; }

            uint64_t GetTextureNumber() const
            {
                uint64_t number = 0ull;
                const uint64_t numberAlbedo = reinterpret_cast<uint64_t>(m_Albedo.get());
                const uint64_t numberShadow = reinterpret_cast<uint64_t>(m_Shadow.get());
                constexpr uint64_t bitsPerBinding = PiplineRenderBase::Hash_BindingPointMultiplyNumberBitMove;
                number |= numberAlbedo << (TextureBinding_Albedo * bitsPerBinding);
                number |= numberShadow << (TextureBinding_Shadow * bitsPerBinding);
                return number;
            }

            void AcquireFromMaterial(const Ref<Material>& material)
            {
                m_MaterielUB = material->GetMaterielUniformBuffer();
                m_Albedo = material->GetAlbedoTextures();
            }

            int GetDrawMode(const Ref<Material>& material) const { return material->GetShadeRenderMode(); }

            void Bind()
            {
                m_MaterielUB->Bind(UniformBinding_Materiel);
                m_LightUB->Bind(UniformBinding_LightCamera);
                m_Albedo->Bind(TextureBinding_Albedo);
                m_Shadow->Bind(TextureBinding_Shadow);
            }

            void Bind(ShaderDrawResource& drawList)
            {
                drawList.GetBindUniform().at(UniformBinding_Materiel) = m_MaterielUB;
                drawList.GetBindUniform().at(UniformBinding_LightCamera) = m_LightUB;

                drawList.GetBindTextures().at(TextureBinding_Albedo) = m_Albedo;
                drawList.GetBindTextures().at(TextureBinding_Shadow) = m_Shadow;
            }

            void Unbind()
            {
                m_MaterielUB->UnBind(UniformBinding_Materiel);
                m_LightUB->UnBind(UniformBinding_LightCamera);
                m_Albedo->UnBind(TextureBinding_Albedo);
                m_Shadow->UnBind(TextureBinding_Shadow);
            }

            void Clear()
            {
                RY_DESTROY_REF(m_LightUB);
                RY_DESTROY_REF(m_Albedo);
                RY_DESTROY_REF(m_Shadow);
            }



            bool CheckRefNotValid() const
            {
                return nullptr == m_MaterielUB || nullptr == m_LightUB || nullptr ==  m_Albedo || nullptr == m_Shadow;
            }
        };
    }


}

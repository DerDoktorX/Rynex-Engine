#pragma once


namespace Rynex {

    namespace Depth {
        struct InstanceLayout
        {
            struct RenderObject
            {
                glm::mat4 m_ModelMatrix;
            };

            static bool Equal(const RenderObject& object, const glm::mat4& model, const int entityID)
            {
                return model == object.m_ModelMatrix;
            }

            static RenderObject Make(const glm::mat4& model, const int entityID)
            {
                return RenderObject{ model };
            }

            static BufferLayout GetVertexLayout()
            {
                constexpr uint32_t instanceAddIndex = 1u;
                constexpr bool aktive = true;
                constexpr bool normilze = false;
                constexpr uint32_t countElements = 0u;
                BufferLayout layout({
                    BufferElement( SDT::Float4, "a_ModelMatrix[0]", aktive, countElements, normilze ),
                    BufferElement( SDT::Float4, "a_ModelMatrix[1]", aktive, countElements, normilze ),
                    BufferElement( SDT::Float4, "a_ModelMatrix[2]", aktive, countElements, normilze ),
                    BufferElement( SDT::Float4, "a_ModelMatrix[3]", aktive, countElements, normilze )
                }, instanceAddIndex);
                // layout.SetAutoCompress(true);
                return layout;
            }


        };

        struct Geometry
        {
            static const Ref<IndexBuffer>& GetIndexBuffer(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetDepthIndexBuffer(); }
            static const Mesh::PerDrawObject& GetDrawObject(const Ref<MeshSingle>& meshSingle) { return meshSingle->GetDepthPerDrawObjectIndirect(); }


            static bool HasVAOFromMeshSingleIndexBuffer(const Ref<VertexArray>& vao, const Ref<MeshSingle>& meshSingle)
            {
                RY_CORE_ASSERT(nullptr != meshSingle, "Mesh is not valid!");
                RY_CORE_ASSERT(nullptr != vao, "VertexArrayObject is not valid!");

                const Ref<IndexBuffer>& iabDepth = meshSingle->GetDepthIndexBuffer();
                const Ref<IndexBuffer>& iab = vao->GetIndexBuffer();

                return iabDepth == iab;
            }
        };

        struct Resources
        {
            Resources() = default;
            Resources(const Resources&) = default;


            uint64_t GetStorageBufferNumber() const { return 0u; }
            uint64_t GetUniformBufferNumber() const { return 0u; }
            uint64_t GetTextureNumber() const { return 0u; }
            uint64_t GetIndirectBufferNumber() const { return 0u; }

            int CheckObject(const Ref<Material>& materielTest, Ref<Material>& materialThis)
            {
                int result = Result_None;

                if (nullptr == materielTest)
                    return (result | Result_NotAllowedShadeDefinition);

                if (nullptr == materialThis)
                    return result;

                if(materialThis->GetDepthRenderMode() != materielTest->GetDepthRenderMode())
                    result |= Result_NotAllowedShadeDefinition | Result_NoShadeDefinitionSpaceLeft;

                return result;
            }

            void AcquireFromMaterial(const Ref<Material>& material) { /* empty */  }
            void SubmitRenderTargetResource(ViewPassStorage& viewPass) { /* empty */ }
            void SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture) {  /* empty */ }
            void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer) { /* empty */ }


            int GetDrawMode(const Ref<Material>& material) const { return material->GetShadeRenderMode(); }

            void Bind() { /* empty */  }
            void Bind(ShaderDrawResource& drawList) { /* empty */ }

            void Unbind() { /* empty */ }
            void Clear() { /* empty */ }


            /**
             *
             * @return always @type{ false } because all, because it has no References that can't be valid.
             */
            bool CheckRefNotValid() const { return false; }
        };
    }


}
#pragma once

namespace Rynex {

    namespace Shape {
        struct Resources
        {
            enum
            {
                TextureBinding_Albedo = 0,
            };

            Ref<Texture> m_Albedo;

            Resources() = default;
            Resources(const Resources&) = default;


            uint64_t GetStorageBufferNumber() const { return 0u; }
            uint64_t GetUniformBufferNumber() const { return 0u; }
            uint64_t GetTextureNumber() const
            {
                uint64_t number = 0ull;
                const uint64_t numberAlbedo = reinterpret_cast<uint64_t>(m_Albedo.get());
                constexpr uint64_t bitsPerBinding = PiplineRenderBase::Hash_BindingPointMultiplyNumberBitMove;
                number |= numberAlbedo << (TextureBinding_Albedo * bitsPerBinding);
                return number;
            }
            uint64_t GetIndirectBufferNumber() const { return 0u; }



            int CheckObject(const Ref<Material>& materielTest, Ref<Material>& materialThis)
            {
                int result = Result_None;

                InstanceMeshPiplineRenderBase::CheckObject(materialThis, materielTest, result, Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);
                if (nullptr != materielTest)
                {
                    const Ref<Texture> texture = materielTest->GetAlbedoTextures();
                    InstanceMeshPiplineRenderBase::CheckObject(m_Albedo, texture, result, Result_NotAllowedTexture, Result_NoTextureSpaceLeft);
                }

                if (nullptr == materielTest)
                    return (result | Result_NotAllowedShadeDefinition);

                if (nullptr == materialThis)
                    return result;

                const int renderModeThis = GetDrawMode(materialThis);
                const int renderModeTest = GetDrawMode(materielTest);
                if(renderModeThis != renderModeTest)
                    result |= Result_NotAllowedShadeDefinition | Result_NoShadeDefinitionSpaceLeft;

                return result;
            }

            void AcquireFromMaterial(const Ref<Material>& material) { /* empty */  }
            void SubmitRenderTargetResource(ViewPassStorage& viewPass) { /* empty */ }
            void SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture) {  /* empty */ }
            void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer) { /* empty */ }


            int GetDrawMode(const Ref<Material>& material) const { return material->GetShadeRenderMode(); }

            void Bind()
            {

                m_Albedo->Bind(TextureBinding_Albedo);
            }
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
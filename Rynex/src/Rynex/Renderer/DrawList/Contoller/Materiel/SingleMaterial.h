#pragma once
// SingleMaterial created in Project Rynex-Rendering on 09/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/DrawList/Contoller/CommandBuildBase.h>
#include <Rynex/Renderer/DrawList/Contoller/InterfaceBase.h>
#include <Rynex/Renderer/DrawList/Contoller/IndexTables.h>
#include <Rynex/Renderer/Materials/Material.h>


namespace Rynex::Control  {



    namespace MaterielDefault {
#if 0

        class Interface : public InterfaceRefBase<Material>
        {
        public:
            Interface()
            {
            }

            ~Interface() override
            {
                this->m_SectionPtr = nullptr;
            }


            virtual BufferLayout GetBufferLayout(const Unit index) const override
            {
                const Ref<UniformBuffer> uniformBuffer = GetUniformBuffer(index);
                const BufferLayout& bufferLayout = uniformBuffer->GetLayout();
                return bufferLayout;
            }

            virtual Ref<UniformBuffer> GetUniformBuffer(const Unit index) const override
            {
                RY_CORE_ASSERT(0 == index, "index is invalid! valid index -> ( 0 )");
                const Ref<UniformBuffer>& uniformBuffer = Get()->GetMaterielUniformBuffer();
                return uniformBuffer;
            }

            virtual Ref<Texture> GetTexture(const Unit index) const override
            {
                RY_CORE_ASSERT(0 == index, "index is invalid! valid index -> ( 0 )");
                const Ref<Texture>& texture = Get()->GetAlbedoTextures();
                return texture;
            }

        private:
            const Ref<Material>& Get() const
            {
                Ref<Material>* materialPtr = this->m_SectionPtr;
                RY_CORE_ASSERT(nullptr != materialPtr, "Section is nullptr");
                return (*materialPtr);
            }
        };

        class CommandBuild : public CommandBuildBase<Interface>
        {
        public:
            enum TextureSlot : uint32_t
            {
                TextureSlot_Texture = 0,
            };
            enum UniformBufferSlot : uint32_t
            {
                UniformBufferSlot_Parameter = 1,
            };
            explicit CommandBuild(CachedBufferRegister& cache, Interface& interfaceType, ShaderDrawResource& drawResource)
                : CommandBuildBase(cache, interfaceType, drawResource)
            {}

            virtual void CollectVertexBuffer(Span<VertexArray::VertexElements>& vertexElementsSpan) override
            {
            }

            virtual void BindCommand() override
            {
                const Ref<Texture> texture = this->m_Interface.GetTexture(0);
                this->m_DrawResource.GetBindTextures()[TextureSlot_Texture] = texture;

                const Ref<UniformBuffer> uniform = this->m_Interface.GetUniformBuffer(0);
                this->m_DrawResource.GetBindUniform()[UniformBufferSlot_Parameter] = uniform;
            }
        };

        using DefaultCommand = MaterielControlImpl<Interface, CommandBuild>;
#else

        class Interface : public RefInterface<Material>
        {
        public:
            template<size_t Index>
            Ref<Texture> GetTexture()
            {
                Material& material = Get();
                return SelectByIndexCompiled<Index>(MaterialAccess::s_Texture, material);
            }

            template<typename Buffer, size_t Index>
            Ref<Buffer> GetBuffer()
            {
                Material& material = Get();
                if constexpr (std::is_same_v<Buffer, UniformBuffer>) {}
                {
                    return SelectByIndexCompiled<Index>(MaterialAccess::s_UniformBuffer, material);
                }
                static_assert(std::is_same_v<Buffer, UniformBuffer>, "No other valid buffer type!");
                return Ref<Buffer>{ nullptr };
            }


            template<size_t Index>
            int GetRenderMode()
            {
                Material& material = Get();
                return SelectByIndexCompiled<Index>(MaterialAccess::s_RenderMode, material);
            }
        };

        using Build = DeclaredBuild<
            Interface
            , BindingTexture<0ull, CacheBufferMode::None, 0ull>
            , BindingUniform<1ull, CacheBufferMode::None, 0ull>
            , BindingRenderMode<0ull>
        >;

        using DefaultCommand = ControlDescriptor<Interface, Build>;
#endif

    }
#if 0
    class SingleMaterial
    {
    public:
        enum SingleMaterialBufferSlot : uint32_t
        {
            None = 0,
            SingleMaterialBufferSlot_Texture = 0,
            SingleMaterialBufferSlot_Uniform = 1,
        };
        static constexpr uint32_t s_InstanceFields = InstanceField_None;
    // public member alias ----------------------------------------------------------------------------------------------------
        using Resources = Ref<Material>;
        using Indices = NoIndices;


        // --- public member methods -----------------------------------------------------------------------------------
        bool Set(const Resources& material)
        {
            RY_CORE_ASSERT(nullptr != material, "Material is not valid!");
            const Ref<Material>& currentMaterial = m_SectionList.Current();
            if(currentMaterial != material)
            {
                m_SectionList.Add(material);
                return true;
            }

            return false;
        }

        const Indices& GetIndices() const { return m_Indices; }


        template<typename Output, typename VertexElementArray, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, VertexElementArray& vertexElementArray, BufferRegister& bufferRegister)
        {
            Bind(section, output, bufferRegister);
        }

        template<typename Output, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, BufferRegister& bufferRegister)
        {
            Ref<Material>& materiel = m_SectionList.Current();
            output.GetBindTextures()[SingleMaterialBufferSlot_Texture] =  materiel->GetAlbedoTextures();
            output.GetBindUniform()[SingleMaterialBufferSlot_Uniform] =  materiel->GetMaterielUniformBuffer();
        }

        void Clear()
        {
            m_SectionList.Clear();
        }

    private:
        SectionList<Ref<Material>> m_SectionList;
        Indices m_Indices;
    };
#endif

    
}

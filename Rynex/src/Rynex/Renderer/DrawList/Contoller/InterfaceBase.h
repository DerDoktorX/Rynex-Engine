#pragma once
// InterfaceBase created in Project Rynex-Rendering on 01/11/2026.
#include <rypch.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>


namespace Rynex {

    template<typename Section, typename Source, typename Indices = NoIndices>
    class InterfaceBase 
    {
    public:
        using Unit = uint32_t;
    // public member operator --------------------------------------------------------------------------------------------------
        InterfaceBase()
            : m_SectionPtr(nullptr)
        {
        }
        
        virtual ~InterfaceBase()
        {
            m_SectionPtr = nullptr;
        }


        // methods to control sectionPtr
        virtual void SetSection(Section* sectionPtr)
        {
            m_SectionPtr = sectionPtr;
        }

        virtual Section* GetSection() const
        {
            return m_SectionPtr;
        }

        // methods called from ControlSlot to change data
        virtual DrawSplit HasSection(const Source& source)
        {
            RY_CORE_ASSERT(nullptr != m_SectionPtr, "Section is nullptr");
            return m_SectionPtr->Has(source);
        }

        virtual Indices GetIndices() const
        {
            RY_CORE_ASSERT(nullptr != m_SectionPtr, "Section is nullptr");
            return m_SectionPtr->GetIndices();
        }

        virtual bool AddSection(const Source& source)
        {
            RY_CORE_ASSERT(nullptr != m_SectionPtr, "Section is nullptr");
            return m_SectionPtr->Add(source);
        }

        virtual void SetSection(const Source& source)
        {
            RY_CORE_ASSERT(nullptr != m_SectionPtr, "Section is nullptr");
            m_SectionPtr->Set(source);
        }

        virtual void ClearSection()
        {
            RY_CORE_ASSERT(nullptr != m_SectionPtr, "Section is nullptr");
            m_SectionPtr->Clear();
        }


        // methods called from Command Build get access to Buffer data or directly access to Gpu Buffer even shaders.
        virtual BufferLayout GetBufferLayout(const Unit index) const
        {
            return m_SectionPtr->GetBufferLayout(index);
        }
        virtual Span<const Byte> GetBufferData(Unit index) const
        {
            return m_SectionPtr->GetBufferData(index);
        }
        virtual Ref<VertexBuffer> GetVertexBuffer(Unit index) const
        {
            return m_SectionPtr->GetVertexBuffer(index);
        }
        virtual Ref<IndexBuffer> GetIndexBuffer(Unit index) const
        {
            return m_SectionPtr->GetIndexBuffer(index);
        }
        virtual Ref<UniformBuffer> GetUniformBuffer(Unit index) const
        {
            return m_SectionPtr->GetUniformBuffer(index);
        }
        virtual Ref<StorageBuffer> GetStorageBuffer(Unit index) const
        {
            return m_SectionPtr->GetStorageBuffer(index);
        }
        virtual Ref<Texture> GetTexture(Unit index) const
        {
            return m_SectionPtr->GetTexture(index);
        }
        virtual Ref<LinkedTextureArray> GetLinkedTextureArray(Unit index) const
        {
            return m_SectionPtr->GetLinkedTextureArray(index);
        }
        virtual Ref<BindlessTextureArray> GetBindlessTextureArray(Unit index) const
        {
            return m_SectionPtr->GetBindlessTextureArray(index);
        }
        virtual Ref<Shader> GetShader(Unit index) const
        {
            return m_SectionPtr->GetShader(index);
        }
    protected:
        Section* m_SectionPtr;
    };

    template<typename RefSection>
    class InterfaceRefBase : public InterfaceBase<Ref<RefSection>, Ref<RefSection>, NoIndices>
    {
    public:
        using Unit = uint32_t;


    // public member operator --------------------------------------------------------------------------------------------------
        InterfaceRefBase()
            : InterfaceBase<Ref<RefSection>, Ref<RefSection>>()
        {
        }

        virtual ~InterfaceRefBase() override
        {
            this->m_SectionPtr = nullptr;
        }


        // methods called from ControlSlot to change data
        virtual DrawSplit HasSection(const Ref<RefSection>& source) override
        {
            Ref<RefSection>* sectionPtr = this->m_SectionPtr;
            if (nullptr == sectionPtr)
                return DrawSplit::NewDraw;
            Ref<RefSection>& sectionRef = this->m_SectionPtr;
            if (source == sectionRef)
                return DrawSplit::Same;
            return DrawSplit::NewDraw;
        }

        virtual NoIndices GetIndices() const override
        {
            return NoIndices{};
        }

        virtual bool AddSection(const Ref<RefSection>& source) override = 0;
        virtual void SetSection(const Ref<RefSection>& source) override
        {
            Ref<RefSection>* sectionPtr = this->m_SectionPtr;
            RY_CORE_ASSERT(nullptr != sectionPtr, "Section is nullptr");
            *sectionPtr = source;
        }

        virtual void ClearSection()
        {
            Ref<RefSection>* sectionPtr = this->m_SectionPtr;
            RY_CORE_ASSERT(nullptr != sectionPtr, "Section is nullptr");
            Ref<RefSection>& sectionRef = *sectionPtr;
            RY_DESTROY_REF(sectionRef);
        }


        virtual BufferLayout GetBufferLayout(const Unit index) const override = 0;
        virtual Span<const Byte> GetBufferData(const Unit index) const override = 0;

    };

}

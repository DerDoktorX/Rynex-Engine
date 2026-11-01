#pragma once
// SectionBase created in Project Rynex-Rendering on 01/11/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/BindlessTextureArray.h>
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>


namespace Rynex {
    template<typename Source, typename T, typename Indices = NoIndices>
    class SectionVecBase
    {
    public:
        using Unit = uint32_t;
        using Container = std::vector<T>;
        using Iterator = typename Container::iterator;
        using ConstIterator = typename Container::const_iterator;
    // public member methode ---------------------------------------------------------------------------------------------------
        SectionVecBase()
        {
        }
        
        virtual ~SectionVecBase()
        {
        }

        virtual DrawSplit Has(const Source& source) = 0;

        virtual Indices GetIndices()
        {
            return Indices{};
        }

        virtual void Add(const Source& source)
        {
            T value = Make(source);
            m_Container.emplace_back(value);
        }

        virtual void Set(const Source& source)
        {
            T value = Make(source);
            m_Container.emplace_back(value);
        }



        virtual void Clear()
        {
            m_Container.clear();
        }

        virtual BufferLayout GetBufferLayout(const Unit index) const
        {
            RY_CORE_ASSERT(0 != index , "no implementation with more then 1 Buffer Data");
            return T::GetBufferLayout();
        }

        virtual Span<const Byte> GetBufferData(Unit index) const
        {
            return AsBytes(m_Container);
        }

        virtual Ref<VertexBuffer> GetVertexBuffer(Unit index) const = 0;
        virtual Ref<IndexBuffer> GetIndexBuffer(Unit index) const = 0;
        virtual Ref<UniformBuffer> GetUniformBuffer(Unit index) const = 0;
        virtual Ref<StorageBuffer> GetStorageBuffer(Unit index) const = 0;

        virtual Ref<Texture> GetTexture(Unit index) const = 0;
        virtual Ref<LinkedTextureArray> GetLinkedTextureArray(Unit index) const = 0;
        virtual Ref<BindlessTextureArray> GetBindlessTextureArray(Unit index) const = 0;
        virtual Ref<Shader> GetShaderBuffer(Unit index) const = 0;

        static T Make(const Source& source);
        static bool IsEqual(const T& value ,const Source& source);

    protected:
        Container m_Container;
    };

    template <typename Source, typename T>
    T SectionVecBase<Source, T>::Make(const Source& source)
    {
        static_assert(false, "No default Implementation off (SectionVecBase::Make)!");
        return T{};
    }

    template <typename Source, typename T>
    bool SectionVecBase<Source, T>::IsEqual(const T& value, const Source& source)
    {
        static_assert(false, "No default Implementation off (SectionVecBase::IsEqual)!");
        return false;
    }


    template<typename Source, typename T, typename Indices = NoIndices>
    class SectionSingleBase
    {
    public:
        using Unit = uint32_t;
    // public member methode ---------------------------------------------------------------------------------------------------
        SectionSingleBase()
            : m_Data(T{})
        {
        }

        virtual ~SectionSingleBase()
        {
        }

        virtual DrawSplit Has(const Source& source) = 0;
        virtual bool Add(const Source& source) = 0;
        virtual void Set(const Source& source) = 0;
        virtual Indices GetIndices()
        {
            return Indices{};
        }

        virtual void Clear()
        {
            m_Data = T{};
        }

        virtual BufferLayout GetBufferLayout(const Unit index) const
        {
            RY_CORE_ASSERT(0u != index, "invalid index! valid indices -> ( 0 )");
            return T::GetBufferLayout();
        }

        virtual Span<const Byte> GetBufferData(const Unit index) const
        {
            RY_CORE_ASSERT(0u != index, "invalid index! valid indices -> ( 0 )");
            Span<T> dataSpan{ &m_Data, 1u };
            return AsBytes(dataSpan);
        }

        virtual Ref<VertexBuffer> GetVertexBuffer(Unit index) const = 0;
        virtual Ref<IndexBuffer> GetIndexBuffer(Unit index) const = 0;
        virtual Ref<UniformBuffer> GetUniformBuffer(Unit index) const = 0;
        virtual Ref<StorageBuffer> GetStorageBuffer(Unit index) const = 0;

        virtual Ref<Texture> GetTexture(Unit index) const = 0;
        virtual Ref<LinkedTextureArray> GetLinkedTextureArray(Unit index) const = 0;
        virtual Ref<BindlessTextureArray> GetBindlessTextureArray(Unit index) const = 0;
        virtual Ref<Shader> GetShaderBuffer(Unit index) const = 0;

        static T Make(const Source& source);
        static bool IsEqual(const T& value ,const Source& source);
    protected:
        T m_Data;
    };


}

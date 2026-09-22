#include "rypch.h"
#include "ShaderDrawList.h"




namespace Rynex {

    namespace {//SortVariantValidRef
       struct SortVariantValidRef
       {

           template<typename T>
           static bool IsRefValidType(const Ref<T>& ref)
           {
               return nullptr == ref;
           }

           bool operator()(const Ref<StorageBuffer>& ref) const {  return IsRefValidType(ref); }
           bool operator()(const Ref<BindlesTextureArray>& ref) const {  return IsRefValidType(ref);  }
           bool operator()(const Ref<Texture>& ref) const { return IsRefValidType(ref); }

           bool operator()(const Ref<LinkedTextureArray>& ref) const { return IsRefValidType(ref);  }
           bool operator()(const Ref<UniformBuffer>& ref) const { return IsRefValidType(ref); }
       };

       struct GetVariantPtrNumber
       {
           template<typename T>
           static uint64_t GetPtrNumber(const Ref<T>& ref)
           {
               T* ptr = ref.get();
               const int64_t ptrValue = reinterpret_cast<int64_t>(ptr);
               return ptrValue;
           }

           uint64_t operator()(const Ref<StorageBuffer>& ref) const {  return GetPtrNumber(ref); }
           uint64_t operator()(const Ref<BindlesTextureArray>& ref) const {  return GetPtrNumber(ref);  }
           uint64_t operator()(const Ref<Texture>& ref) const { return GetPtrNumber(ref); }

           uint64_t operator()(const Ref<LinkedTextureArray>& ref) const { return GetPtrNumber(ref);  }
           uint64_t operator()(const Ref<UniformBuffer>& ref) const { return GetPtrNumber(ref); }
       };

        struct SortTexture
       {
           Ref<Texture>& m_ValueA;

           explicit SortTexture(Ref<Texture>& valueA)
                : m_ValueA(valueA)
           {
           }

           bool operator()(Ref<Texture>& refB) const {  return nullptr != refB && nullptr == m_ValueA; }
           bool operator()(Ref<LinkedTextureArray>& refB) const {  return false; }
       };
        struct SortLinkedTextureArray
       {
           Ref<LinkedTextureArray>& m_ValueA;

           explicit SortLinkedTextureArray(Ref<LinkedTextureArray>& valueA)
                : m_ValueA(valueA)
           {
           }

           bool operator()(Ref<Texture>& refB) const {  return false; }
           bool operator()(Ref<LinkedTextureArray>& refB) const { return nullptr != refB && nullptr == m_ValueA; }
       };
        struct SortVariantTexture
       {
           TextureVariants& m_VariantB;

           explicit SortVariantTexture(TextureVariants& variantB)
                : m_VariantB(variantB)
           { }

           bool operator()(Ref<Texture>& ref) const {  return std::visit(SortTexture{ ref }, m_VariantB); }
           bool operator()(Ref<LinkedTextureArray>& ref) const {  return std::visit(SortLinkedTextureArray{ ref }, m_VariantB); }

       };


        struct SortStorageBuffer
        {
            Ref<StorageBuffer>& m_ValueA;

            explicit SortStorageBuffer(Ref<StorageBuffer>& valueA)
                 : m_ValueA(valueA)
            {
            }


            bool operator()(Ref<StorageBuffer>& refB) const {  return nullptr != refB && nullptr == m_ValueA; }
            bool operator()(Ref<BindlesTextureArray>& refB) const {  return false; }
        };

        struct SortBindlessTextureArray
        {
            Ref<BindlesTextureArray>& m_ValueA;

            explicit SortBindlessTextureArray(Ref<BindlesTextureArray>& valueA)
                 : m_ValueA(valueA)
            {
            }
            bool operator()(Ref<StorageBuffer>& refB) const {  return false; }
            bool operator()(Ref<BindlesTextureArray>& refB) const {  return nullptr != refB && nullptr == m_ValueA; }

        };
        struct SortVariantStorageBuffer
        {
            StorageBufferVarints& m_VariantB;

            explicit SortVariantStorageBuffer(StorageBufferVarints& variantB)
                 : m_VariantB(variantB)
            { }

            bool operator()(Ref<StorageBuffer>& ref) const {  return std::visit(SortStorageBuffer{ ref }, m_VariantB); }
            bool operator()(Ref<BindlesTextureArray>& ref) const {  return std::visit(SortBindlessTextureArray{ ref }, m_VariantB); }

        };
   }

    namespace Utils {
        static bool SortTexture(TextureVariants& aVariant, TextureVariants& bVariant)
        {
            return std::visit(SortVariantTexture{bVariant }, aVariant);
        }

        static  bool SortStorageBuffer(StorageBufferVarints& aVariant, StorageBufferVarints& bVariant)
        {
            return std::visit(SortVariantStorageBuffer{ bVariant }, aVariant);
        }
    }

    bool BindingShaderLayoutStatic::IsEqualSort(const BindingShaderLayoutStatic& right) const
    {
        constexpr size_t sTextureMultyplyer = 1;
        constexpr size_t sUniformMultyplyer = 1;
        constexpr size_t sStorageMultyplyer = 1;
        int eqaul = 0;
        int notEqual = 0;
        for (int i = 0; i < m_BindTextures.size(); i++)
        {
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
            const bool isThisValidSSBO = std::visit(SortVariantValidRef{}, this->m_BindStorage[i]);
            // bool isThisValidSSBO = std::visit([](const auto& refThis) { return nullptr == refThis; }, this->m_BindStorage[i]);
            const bool isRightValidSSBO = std::visit(SortVariantValidRef{}, right.m_BindStorage[i]);

            if (isThisValidSSBO && isRightValidSSBO)
                continue;
#else
            if (this->m_BindStorage[i] && right->m_BindStorage[i])
                continue;
#endif

#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
            if (this->m_BindTextures[i] == right.m_BindTextures[i])
#else
            if (this->m_BindTextures[i] == right->m_BindTextures[i])
#endif
                eqaul += sTextureMultyplyer;
            else
                notEqual += sTextureMultyplyer;
        }

        for (int i = 0; i < m_BindUniform.size(); i++)
        {
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS

            const bool isThisValidShaderStoagBufferObject = std::visit(SortVariantValidRef{}, this->m_BindStorage[i]);
            const bool isRightValidShaderStoagBufferObject = std::visit(SortVariantValidRef{}, right.m_BindStorage[i]);

            if (isThisValidShaderStoagBufferObject && isRightValidShaderStoagBufferObject)
                continue;
#else
            if (this->m_BindStorage[i] && right->m_BindStorage[i])
                continue;
#endif

            if (this->m_BindUniform[i] == right.m_BindUniform[i])
                eqaul += sUniformMultyplyer;
            else
                notEqual += sUniformMultyplyer;
        }

        for (int i = 0; i < m_BindStorage.size(); i++)
        {
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
            const int64_t thisPtrShaderStorageBufferObject = std::visit(GetVariantPtrNumber{}, this->m_BindStorage[i]);
            const int64_t rightPtrShaderStorageBufferObject = std::visit(GetVariantPtrNumber{}, right.m_BindStorage[i]);

            if (0ull != thisPtrShaderStorageBufferObject && 0ull != rightPtrShaderStorageBufferObject)
                continue;

            if (thisPtrShaderStorageBufferObject == rightPtrShaderStorageBufferObject)
                eqaul += sStorageMultyplyer;
            else
                notEqual += sStorageMultyplyer;
#else
            if (this->m_BindStorage[i] == rigth->m_BindStorage[i])
                eqaul += sUniformMultyplyer;
            else
                notEqual += sUniformMultyplyer;
#endif
        }
        bool result = eqaul > notEqual;
        return result;
    }

    bool BindingShaderLayoutStatic::IsEqualSort(const Ref<BindingShaderLayoutStatic>& right) const
    {
        constexpr size_t sTextureMultyplyer = 1;
        constexpr size_t sUniformMultyplyer = 1;
        constexpr size_t sStorageMultyplyer = 1;
        int eqaul = 0;
        int notEqual = 0;
        for (int i = 0; i < m_BindTextures.size(); i++)
        {
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
            bool isthisVaildSSBO = std::visit(SortVariantValidRef{}, this->m_BindStorage[i]);
            bool isRigthVaildSSBO = std::visit(SortVariantValidRef{}, right->m_BindStorage[i]);

            if (isthisVaildSSBO && isRigthVaildSSBO)
                continue;
#else
            if (this->m_BindStorage[i] && right->m_BindStorage[i])
                continue;
#endif

#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
            if (this->m_BindTextures[i] == right->m_BindTextures[i])
#else
            if (this->m_BindTextures[i] == right->m_BindTextures[i])
#endif
                eqaul += sTextureMultyplyer;
            else
                notEqual += sTextureMultyplyer;
        }

        for (int i = 0; i < m_BindUniform.size(); i++)
        {
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS

            const bool isThisValidSSBO = std::visit(SortVariantValidRef{}, this->m_BindStorage[i]);
            const bool isRightValidSSBO = std::visit(SortVariantValidRef{}, right->m_BindStorage[i]);

            if (isThisValidSSBO && isRightValidSSBO)
                continue;
#else
            if (this->m_BindStorage[i] && right->m_BindStorage[i])
                continue;
#endif

            if (this->m_BindUniform[i] == right->m_BindUniform[i])
                eqaul += sUniformMultyplyer;
            else
                notEqual += sUniformMultyplyer;
        }

        for (int i = 0; i < m_BindStorage.size(); i++)
        {
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
            const int64_t thisPtrSSBO = std::visit(GetVariantPtrNumber{}, this->m_BindStorage[i]);
            const int64_t rightPtrSSBO = std::visit(GetVariantPtrNumber{}, right->m_BindStorage[i]);

            if (thisPtrSSBO != 0ll && rightPtrSSBO != 0ll)
                continue;

            if (thisPtrSSBO == rightPtrSSBO)
                eqaul += sStorageMultyplyer;
            else
                notEqual += sStorageMultyplyer;
#else
            if (this->m_BindStorage[i] == rigth->m_BindStorage[i])
                eqaul += sUniformMultyplyer;
            else
                notEqual += sUniformMultyplyer;
#endif
        }
        const bool result = eqaul > notEqual;
        return result;
    }

    bool BindingShaderLayoutStatic::IsEqualComplete(const Ref<BindingShaderLayoutStatic>& right) const
    {
        constexpr size_t sTextureMultyplyer = 1u;
        constexpr size_t sUniformMultyplyer = 1u;
        constexpr size_t sStorageMultyplyer = 1u;
        int eqaul = 0;
        int notEqual = 0;
        for (int i = 0; i < m_BindTextures.size(); i++)
        {
            if (this->m_BindTextures[i] == right->m_BindTextures[i])
                notEqual += sTextureMultyplyer;
        }

        for (int i = 0; i < m_BindUniform.size(); i++)
        {
            if (this->m_BindUniform[i] != right->m_BindUniform[i])
                notEqual += sUniformMultyplyer;
        }

        for (int i = 0; i < m_BindStorage.size(); i++)
        {
            if (this->m_BindStorage[i] != right->m_BindStorage[i])
                notEqual += sStorageMultyplyer;
        }


        const bool result = 0 == notEqual;
        return result;
    }

    void BindingShaderLayoutStatic::Sort()
    {
#ifdef RY_TEXTURE_VARIANTS
       std::sort(m_BindTextures.begin(), m_BindTextures.end(), Utils::SortTexture);
#else
        std::sort(m_BindTextures.begin(), m_BindTextures.end(), [](Ref<Texture>& a, Ref<Texture>& b) {return nullptr != a && nullptr == b;  });
#endif

#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
        std::sort(m_BindStorage.begin(), m_BindStorage.end(), Utils::SortStorageBuffer);
#else
        std::sort(m_BindStorage.begin(), m_BindStorage.end(), [](Ref<StorageBuffer>& a, Ref<StorageBuffer>& b) {return nullptr != a && nullptr == b;  });
#endif
        std::sort(m_BindUniform.begin(), m_BindUniform.end(),
                  [](const Ref<UniformBuffer>& a, const Ref<UniformBuffer>& b) -> bool
                  {
                      return nullptr != a && nullptr == b;
                  }
        );
    }

    void BindingShaderLayoutStatic::Clear()
    {
#ifdef RY_TEXTURE_VARIANTS
        for (TextureVariants& texture : m_BindTextures)
        {
            std::visit([](auto& ref) -> void { RY_DESTROY_REF(ref); }, texture);
        }
#else
        for (Ref<Texture>& tex : m_BindTextures)
        {
            RY_DESTROY_REF(tex);
        }
#endif

        for (Ref<UniformBuffer>& uniform : m_BindUniform)
        {
            RY_DESTROY_REF(uniform);
        }
#ifdef RY_SHADER_STORAGE_BUFFER_OBJECT_VARIANTS
        for (StorageBufferVarints& storage : m_BindStorage)
        {
            std::visit([](auto& ref) -> void { RY_DESTROY_REF(ref); }, storage);
        }
#else
        for (Ref<StorageBuffer>& storage : m_BindStorage)
        {
            RY_DESTROY_REF(storage);
        }
#endif
    }

    ShaderDrawList::ShaderDrawList()
        : m_ShaderProgram(nullptr)
        , m_VAO(nullptr)
        , m_IndicesCount(0u)
        , m_DrawBuffer(nullptr)
        , m_DrawElement({0u, 0u, 0u, -1, 0u})
        , m_BindingLayout()
        , m_RenderMode(0)
    {
    }

    ShaderDrawList::ShaderDrawList(
        const Ref<Shader>& program,
        const Ref<VertexArray>& vao,
        const uint32_t indicesCount,
        const Ref<IndirectBuffer>& drawBuffer,
        const Mesh::PerDrawObject& drawElement,
        const BindingShaderLayoutStatic& bindingLayout,
        const int renderMode
    )
        : m_ShaderProgram(program)
        , m_VAO(vao)
        , m_IndicesCount(indicesCount)
        , m_DrawBuffer(drawBuffer)
        , m_DrawElement(drawElement)
        , m_BindingLayout(bindingLayout)
        , m_RenderMode(renderMode)
    {
    }

    void ShaderDrawList::Sort()
    {
        m_BindingLayout.Sort();
    }

    void ShaderDrawList::Clear()
    {
        RY_DESTROY_REF(m_ShaderProgram);
        RY_DESTROY_REF(m_DrawBuffer);
        RY_DESTROY_REF(m_VAO);
        m_BindingLayout.Clear();
    }

    TextureBindArray& ShaderDrawList::GetBindTextures()
    {
        return m_BindingLayout.m_BindTextures;
    }

    UniformBindArray& ShaderDrawList::GetBindUniform()
    {
        return m_BindingLayout.m_BindUniform;
    }

    StorageBindArray& ShaderDrawList::GetBindStorage()
    {
        return m_BindingLayout.m_BindStorage;
    }

    const TextureBindArray& ShaderDrawList::GetBindTextures() const
    {
        return m_BindingLayout.m_BindTextures;
    }

    const UniformBindArray& ShaderDrawList::GetBindUniform() const
    {
        return m_BindingLayout.m_BindUniform;
    }

    const StorageBindArray& ShaderDrawList::GetBindStorage() const
    {
        return m_BindingLayout.m_BindStorage;
    }
}

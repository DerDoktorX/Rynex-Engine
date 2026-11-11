#pragma once
// IndexTables created in Project Rynex-Rendering on 11/11/2026.

#include <rypch.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>


namespace Rynex {

    template<typename Result, typename Object>
    using AccessFunction = Result (*)(Object&);

    template<typename Object, typename Result, size_t Count>
    using AccessTable = std::array<AccessFunction<Result, Object>, Count>;

    template<size_t Index, typename Result, typename Object, size_t Count>
    constexpr Result SelectByIndexCompiled(const AccessTable<Result, Object, Count>& table, Object& object)
    {
        static_assert(Index < Count, "index is invalid!");
        const AccessFunction<Result, Object> function = table[Index];
        return function(object);
    }

    template<size_t Index, typename Result, typename Object, size_t Count>
    constexpr Result SelectByIndexCompiled(const std::array<AccessFunction<Result, Object>, Count>& table, Object& object)
    {
        static_assert(Index < Count, "index is invalid!");
        const AccessFunction<Result, Object> function = table[Index];
        return function(object);
    }


    template<size_t Count, size_t Index>
    constexpr void AssertIndexCompiled()
    {
        static_assert(Index < Count, "outside valid range!");
    }


    struct MeshSingleAccess
    {


        static Ref<VertexBuffer> VertexOf(MeshSingle& mesh) { return mesh.GetVertexBuffer(); }
        static Ref<IndexBuffer> ShadeIndexOf(MeshSingle& mesh) { return mesh.GetShadeIndexBuffer(); }
        static Ref<IndexBuffer> DepthIndexOf(MeshSingle& mesh) { return mesh.GetDepthIndexBuffer(); }

        static constexpr std::array<AccessFunction<Ref<VertexBuffer>, MeshSingle>, 1u> s_VertexBuffers{ &(MeshSingleAccess::VertexOf) };
        static constexpr std::array<AccessFunction<Ref<IndexBuffer>, MeshSingle>, 2u> s_IndexBuffers = { &(MeshSingleAccess::ShadeIndexOf), &(MeshSingleAccess::DepthIndexOf) };
    };

    struct MaterialAccess
    {
        template<typename Result, size_t Count>
        using Table = AccessTable<Result, Material, Count>;

        static Ref<UniformBuffer> UniformBufferOf(Material& material) { return material.GetMaterielUniformBuffer(); }
        static Ref<Texture> AlbedoIndexOf(Material& material) { return material.GetAlbedoTextures(); }
        static Ref<Texture> SpecularIndexOf(Material& material) { return material.GetSpecularTextures(); }
        static Ref<Texture> HeightIndexOf(Material& material) { return material.GetSpecularTextures(); }

        static int ShadeRenderModeOf(Material& material) { return material.GetShadeRenderMode(); }
        static int DeathRenderModeOf(Material& material) { return material.GetDepthRenderMode(); }


        static constexpr std::array<AccessFunction<Ref<UniformBuffer>, Material>, 1u> s_UniformBuffer = { &(MaterialAccess::UniformBufferOf) };
        static constexpr std::array<AccessFunction<Ref<Texture>, Material>, 3u> s_Texture = { &(MaterialAccess::AlbedoIndexOf), &(MaterialAccess::SpecularIndexOf), &(MaterialAccess::HeightIndexOf) };
        static constexpr std::array<AccessFunction<int, Material>, 2u> s_RenderMode = { &(MaterialAccess::ShadeRenderModeOf), &(MaterialAccess::DeathRenderModeOf) };
    };


}

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


    template<size_t Count, size_t Index>
    constexpr void AssertIndexCompiled()
    {
        static_assert(Index < Count, "outside valid range!");
    }


    struct MeshSingleAccess
    {
        template<typename Result, size_t Count>
        using Table = AccessTable<Result, MeshSingle, Count>;


        static Ref<VertexBuffer> VertexOf(MeshSingle& mesh) { return mesh.GetVertexBuffer(); }
        static Ref<IndexBuffer> ShadeIndexOf(MeshSingle& mesh) { return mesh.GetShadeIndexBuffer(); }
        static Ref<IndexBuffer> DepthIndexOf(MeshSingle& mesh) { return mesh.GetDepthIndexBuffer(); }

        static constexpr Table<Ref<VertexBuffer>, 1u> s_VertexBuffers{ { &VertexOf } };
        static constexpr Table<Ref<IndexBuffer>, 2u> s_IndexBuffers{ { &ShadeIndexOf, &DepthIndexOf } };
    };

    struct MaterialAccess
    {
        template<typename Result, size_t Count>
        using Table = AccessTable<Result, Material, Count>;

        static Ref<UniformBuffer> UniformBufferOf(Material& material) { return material.GetUniformBuffer(); }
        static Ref<Teture> AlbedoIndexOf(Material& material) { return material.GetAlbedoTextures(); }
        static Ref<Texture> SpecularIndexOf(Material& material) { return material.GetSpecularTextures(); }
        static Ref<Texture> HeightIndexOf(Material& material) { return material.GetSpecularTextures(); }

        static int ShadeRenderModeOf(Material& material) { return material.GetShadeRenderMode(); }
        static int DeathRenderModeOf(Material& material) { return material.GetDephtRenderMode(); }

        static constexpr Table<Ref<UniformBuffer>, 1ull> s_VertexBuffers{ { &UniformBufferOf } };
        static constexpr Table<Ref<Texture>, 3ull> s_IndexBuffers{ { &AlbedoIndexOf, &SpecularIndexOf, &HeightIndexOf } };
        static constexpr Table<int, 2ull> s_RenderMode{ { &ShadeRenderModeOf, &DeathRenderModeOf } };
    };
}

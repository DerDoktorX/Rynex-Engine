#pragma once
// DrawListImpl created in Project Rynex-Rendering on 03/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>


namespace Rynex {

    RY_DEFINE_HAS_STATIC_MEMBER(Make(std::declval<const glm::mat4&>(), std::declval<int>()), InstanceControlMethodeRenderObject);
    RY_DEFINE_HAS_STATIC_MEMBER(s_Fields, InstanceControlVaribleFields);
    RY_DEFINE_HAS_STATIC_MEMBER(s_InstanceFields, MaterielControlVaribleInstanceFields);
    // RY_DEFINE_HAS_INSTANCE_MEMBER();

    // template<typename Instance, typename GeometryControl, typename MaterielControl, typename EffectControl>
    template<typename InstanceControl, typename GeometryControl, typename MaterielControl>
    class DrawListImpl
    {
        static_assert(std::is_trivially_copyable_v<typename  InstanceControl::Instance>, "No instance date type provided! By InstanceControl");
        static_assert(0 < sizeof(typename  InstanceControl::Instance), "No data in InstanceValue!");
        static_assert(HasMemberStatic_InstanceControlVaribleFields_v<InstanceControl>, "No static variable (s_Fields) provided! By InstanceControl");
        static_assert(HasMemberStatic_MaterielControlVaribleInstanceFields_v<MaterielControl>, "No static variable (s_InstanceFields) provided! By MaterielControl");

        static_assert(0u == (InstanceControl::s_Fields & ~(MaterielControl::s_InstanceFields)), "MaterielControl hands out indices the instance record cannot store!");

        static_assert(std::is_trivially_copyable_v<typename MaterielControl::Indices>, "No indices date type provided! By MaterielControl");
        static_assert(InstanceControl::Instance);
        static_assert(InstanceControl::Fields, "No instance Fields value provided!");
        static_assert(MaterielControl::Provides, "No materiel Fields value provided!");
    public:
        using InstanceValue = typename InstanceControl::Instance;
        using GeometryResources = typename GeometryControl::Resources;      // input data like Ref<MeshSingle>, std::vector<Vertex>, AssetHandle or other stuff.
        using MaterielIndices = typename MaterielControl::Indices;          // input data like Ref<Materiel>, { Ref<Materiel>, Ref<UniformBuffer> }, AssetHandle or other stuff. Maby in futter some stuff gets into the EffectController things like Lighting
        using MaterielResources = typename GeometryControl::Resources;
        using Unit = uint32_t;

    // public static constexpr ------------------------------------------------------------------------------------------------
        static constexpr uint32_t s_MaterielProvides = MaterielControl::s_Provides;
        static constexpr uint32_t s_InstanceFields = InstanceControl::s_Fields;
    // public member methode --------------------------------------------------------------------------------------------------
        DrawListImpl()
        {
        }

        ~DrawListImpl()
        {
        }

        void SetGeometry(const GeometryResources& geometry)
        {

        }

        void SetMateriel(const MaterielResources& material)
        {

        }

        void Add(const glm::mat4& modelMatrix, const uint32_t entityID)
        {
            InstanceValue value = InstanceControl::Make(modelMatrix, entityID);
            m_InstanceVec.emplace_back(value);
        }

        Unit GetCount() const
        {
            return m_InstanceVec.size();
        }

        Unit GetByteSize() const
        {
            return GetCount() * sizeof(InstanceValue);
        }

        bool IsEmpty() const
        {
            return 0u == GetCount();
        }

        void Clear()
        {
            m_InstanceVec.clear();
            m_Commands.clear();
            m_GeometryControl.Clear();
            m_MaterielControl.Clear();
        }

        void UploadCommands(std::vector<ShaderDrawResource>& out)
        {

        }

        /**
         * this methode uploads all draw call data in buffers and set them to the expected Bind slot index.
         * this methode can create multiple draw calls or under thr right condition just one cpu call. Withe multiple Instances, Geometry and Materials.
         * @tparam BufferRegister Some Kind off buffer Register Type for caching and reusing off buffer
         * @param out a output vector off draw calls
         * @param bufferRegister register to get or create Buffer
         */
        template<typename BufferRegister>
        void UploadCommands(std::vector<ShaderDrawResource>& out, BufferRegister& bufferRegister)
        {

        }


    // public static methode ---------------------------------------------------------------------------------------------------
    // public member operator --------------------------------------------------------------------------------------------------
    private:
    // private static methode --------------------------------------------------------------------------------------------------
    // private member methode --------------------------------------------------------------------------------------------------
    // private member variable -------------------------------------------------------------------------------------------------

        std::vector<InstanceValue> m_InstanceVec;
        std::vector<DrawElementsIndirectCommand> m_Commands;
        GeometryControl m_GeometryControl;
        MaterielControl m_MaterielControl;
    };


}

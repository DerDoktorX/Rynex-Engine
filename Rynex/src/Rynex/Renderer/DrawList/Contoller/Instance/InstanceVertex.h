#pragma once
// InstanceVertexArray created in Project Rynex-Rendering on 05/10/2026.

#include <Rynex/Math/Math.h>
#include <Rynex/Renderer/DrawList/Contoller/CommandBuildBase.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>
#include <Rynex/Renderer/RenderMath.h>
#include <Rynex/Renderer/DrawList/Contoller/VectorInterface.h>

namespace Rynex::Control  {




    namespace InstanceDefault {

#if 0
        class Section
        {
        public:
            using Unit = uint32_t;
            struct InstanceData
            {
                glm::mat4 m_ModelMatrix;
                glm::mat4 m_NormaleMatrix;
                int32_t m_EntityID;
                int32_t m_GeometryID;
                int32_t m_ParameterIndex;
                int32_t m_TextureIndex;

                InstanceData()
                    : m_ModelMatrix(glm::mat4(-1.f))
                    , m_NormaleMatrix(glm::mat4(-1.f))
                    , m_EntityID(-1)
                    , m_GeometryID(-1)
                    , m_ParameterIndex(-1)
                    , m_TextureIndex(-1)
                {
                }

                template<typename Source>
                void Add(const Source& source)
                {
                    InstanceData data = Source::Make<InstanceData>(source);
                    m_InstanceData.emplace_back(data);
                }

                void Clear()
                {
                    m_InstanceData.clear();
                }

                Span<const Byte> GetBufferData(Unit index) const
                {
                    switch (index)
                    {
                        case 0: return AsBytes(m_InstanceData);
                        default:
                            RY_CORE_ASSERT(false, "Invalid BufferData index! valid Index -> ( 0 )");
                    }
                    return Span<const Byte>{ nullptr, 0 };
                }
            private:
                std::vector<InstanceData> m_InstanceData;
            };
        };

#endif

        struct Source
        {
            glm::mat4 m_Transform;
            int m_EntityID;
            int m_MaterielIndex;
            int m_ParameterIndex;
            int m_TextureIndex;

            Source() = default;
            Source(const Source&) = default;

            Source& operator=(const Source&) = default;

            bool operator==(const Source& source) const
            {
                return m_EntityID == source.m_EntityID
                && m_Transform == source.m_Transform
                && m_TextureIndex == source.m_TextureIndex
                && m_MaterielIndex == source.m_MaterielIndex
                && m_ParameterIndex == source.m_ParameterIndex;
            }

        };

        class Policy
        {
        public:
            constexpr static size_t s_MaxElements = 100;
            struct Element
            {
                glm::mat4 m_Transform;
                glm::mat4 m_Normals;

                Element(const Element&) = default;

                Element()
                    : m_Transform(1.0f)
                    , m_Normals(1.0f)
                {
                }

                explicit Element(const glm::mat4& transform)
                    : m_Transform(transform)
                    , m_Normals(Math::CalculateNormaleMatrix(transform))
                {
                }
            };


            static Element Make(const Source& source)
            {
                return Element{
                    source.m_Transform
                };
            }

            static BufferLayout GetBufferLayout()
            {
                BufferLayout layout{
                    BufferElement{ SDT::Float4x4,   "m_Transform"  },
                    BufferElement{ SDT::Int,        "m_EntityID"},
                    BufferElement{ SDT::Int,        "m_MaterielIndex"},
                    BufferElement{ SDT::Int,        "m_ParameterIndex"},
                    BufferElement{ SDT::Int,        "m_TextureIndex"}
                };
                layout.SetAutoCompress(true);
                return layout;
            }
        };


        class Interface : public VectorInterface<Source, Policy>
        {
        public:
            using Indices = int;
            Interface()
                : VectorInterface<Source, Policy>()
            {
            }

            ~Interface()
            {
            }

            Indices GetInstance()
            {
                return 0;
            }

        };




        using Build = DeclaredBuild<
            Interface
            , BindingTexture<0ull, CacheBufferMode::None, 0ull>
            , BindingUniform<1ull, CacheBufferMode::None, 0ull>
            , BindingRenderMode<0ull>
        >;

        using DefaultCommand = ControlDescriptor<Interface, Build>;


    }

#if 0
    class InstanceVertex
    {
    public:
        enum VertexBufferSlot : uint32_t
        {
            VertexBufferSlot_Instance = 1u,
        };
        static constexpr uint32_t s_VertexBufferCount = 1u;
        static constexpr uint32_t s_UniformBufferCount = 0u;
        static constexpr uint32_t s_StorageBufferCount = 0u;
        static constexpr uint32_t s_AttributeLocationMask = BIT(VertexBufferSlot_Instance);
        static constexpr uint32_t s_UniformSlotMask = 0u;
        static constexpr uint32_t s_StorageSlotMask = 0u;
        static constexpr uint32_t s_TextureSlotMask = 0u;
        static constexpr bool s_GpuReadable = false;
        static constexpr bool s_SupportsRetained = false;
    // public member struct ---------------------------------------------------------------------------------------------------
        struct Instance
        {
            glm::mat4 m_Model;
            glm::mat4 m_Normal;
            int32_t m_Entity;
            int32_t m_MaterialIndex;
            int32_t m_ParameterIndex;
            int32_t m_TextureIndex;

            Instance() = default;
            Instance(const Instance&) = default;
            Instance(Instance&& instance) noexcept
                : m_Model(instance.m_Model)
                , m_Normal(instance.m_Normal)
                , m_Entity(instance.m_Entity)
                , m_MaterialIndex(instance.m_MaterialIndex)
                , m_ParameterIndex(instance.m_ParameterIndex)
                , m_TextureIndex(instance.m_TextureIndex)
            {
            }

            static Instance Make(const InstanceSource& instanceSource)
            {
                Instance instance;
                instance.m_Model = instanceSource.m_Model;
                instance.m_Normal = glm::transpose(instanceSource.m_Model);
                instance.m_Entity = instanceSource.m_EntityID;
                instance.m_MaterialIndex = -1;
                instance.m_ParameterIndex = -1;
                instance.m_TextureIndex = -1;
                return instance;
            }

            template<typename InstanceMateriel>
            static Instance Make(const InstanceSource& instanceSource, const InstanceMateriel& instanceMateriel)
            {
                Instance instance = Make(instanceSource);
                if constexpr (0u != (InstanceMateriel::s_FieldMask & InstanceField_MaterialIndex))
                    instance.m_MaterialIndex = instanceMateriel.m_MaterialIndex;
                if constexpr (0u != (InstanceMateriel::s_FieldMask & InstanceField_ParameterIndex))
                    instance.m_ParameterIndex = instanceMateriel.m_ParameterIndex;
                if constexpr (0u != (InstanceMateriel::s_FieldMask & InstanceField_TextureIndex))
                    instance.m_TextureIndex = instanceMateriel.m_TextureIndex;

                return instance;
            }

            static BufferLayout GetLayout()
            {
                constexpr uint32_t instanceIncrease = 1;
                BufferLayout layout({
                    {SDT::Float4x4, "m_Model"},
                    {SDT::Float4x4, "m_Normal"},
                    {SDT::Int, "m_Entity"},
                    {SDT::Int, "m_MaterialIndex"},
                    {SDT::Int, "m_ParameterIndex"},
                    {SDT::Int, "m_TextureIndex"},
                }, instanceIncrease);
                layout.SetAutoCompress(true);

                return layout;
            }
        };
        using Section = std::vector<Instance>;
    // public member methode --------------------------------------------------------------------------------------------------
        InstanceVertex()
            : m_SectionList()
        {
        }



        DrawListControllerResult Add(const InstanceSource& instanceSource)
        {
            std::vector<Instance>& instanceVec = m_SectionList.Current();
            Instance instance = Instance::Make(instanceSource);
            instanceVec.emplace_back(instance);
            return DrawListControllerResult::Same;
        }

        template<typename InstanceMateriel>
        DrawListControllerResult Add(const InstanceSource& instanceSource, const InstanceMateriel& instanceMateriel)
        {
            std::vector<Instance>& instanceVec = m_SectionList.Current();
            instanceVec = Instance::Make(instanceSource, instanceMateriel);
            return DrawListControllerResult::Same;
        }

        uint32_t GetSectionCount() const
        {
            return m_SectionList.Count();
        }

        template<typename Output, typename VertexElementArray, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, VertexElementArray& vertexElementArray, BufferRegister& bufferRegister)
        {
            const std::vector<Instance>& instanceVec = m_SectionList.Get(section);
            const Span<const Instance> instanceSpan(instanceVec.data(), instanceVec.size());
            Ref<VertexBuffer> vertexBuffer = bufferRegister.AcquireVertexBuffer(instanceSpan);
            vertexElementArray[VertexBufferSlot_Instance] = VertexArray::VertexElements{ Instance::GetLayout(), vertexBuffer };
        }

        template<typename Output, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, BufferRegister& bufferRegister)
        {
            static_assert(false, "we need to use the methoed implemtion with VertexElementArray");
        }

        void Clear()
        {
            m_SectionList.Clear();
        }

        uint32_t GetCapacity() const
        {
            return std::numeric_limits<uint32_t>::max();
        }

        uint32_t Allocate()
        {
            static_assert(false, "this mode methode is not implemted!");
            return std::numeric_limits<uint32_t>::max();
        }

        void Write(uint32_t slot, const Instance& instance)
        {
            static_assert(false, "this mode methode is not implemted!");
        }

        void Free(uint32_t slot)
        {
            static_assert(false, "this mode methode is not implemted!");
        }

    private:
        SectionList<Section> m_SectionList;
    };
#endif

}

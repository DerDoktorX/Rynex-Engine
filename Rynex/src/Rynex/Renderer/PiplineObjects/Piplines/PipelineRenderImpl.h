#pragma once
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/RenderCommand.h>

#include <Rynex/Renderer/PiplineObjects/Piplines/InstanceMeshPiplineRenderBase.h>

namespace Rynex {

    template<typename InstanceLayout, typename Resources, typename Geometry>
    class PipelineRenderImpl : public InstanceMeshPiplineRenderBase
    {
    public:

        using RenderObject = typename InstanceLayout::RenderObject;
        static_assert(0 < sizeof(RenderObject), "RenderObject needs Data!");
        PipelineRenderImpl()
            : m_RenderObjectVec()
            , m_EntityIDVec()
            , m_Resources(Resources{})
            , m_UpdateModelBuffer(true)
        {
        }

        PipelineRenderImpl(const PipelineRenderImpl&) = default;


        virtual ~PipelineRenderImpl()
        {
            Clear();
        }

        virtual void SubmitRenderTargetResource(ViewPassStorage& viewPass) override
        {
            m_Resources.SubmitRenderTargetResource(viewPass);
        }

        virtual void SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture) override
        {
            m_Resources.SubmitRenderTargetResourceReadImg(texture);
        }

        virtual void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer) override
        {
            m_Resources.SubmitRenderTargetResourceReadUB(buffer);
        }

        virtual void SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID) override
        {
            if (m_InstanceCount != storeIndex)
                storeIndex = m_InstanceCount;

            SetObject(storeIndex, model, entityID);
            m_InstanceCount++;
            m_DrawsAfterLastUpdate = 0u;
        }

        virtual bool Empty() const override
        {
            return 0u == m_InstanceCount;
        }

        virtual bool IsFull() const override
        {
            if (State_MaxEntityRender < m_InstanceCount)
            {
                uint32_t toManyObjects = m_InstanceCount - State_MaxEntityRender;
                RY_CORE_WARN("This RenderPipline has {} more Stored then allowed", toManyObjects);
                return true;
            }
            return State_MaxEntityRender == m_InstanceCount;
        }

        virtual uint64_t GetVertexBufferNumber() const override
        {
            const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
            const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
            uint64_t number = 0ull;
            uint32_t i = 0;


            const uint64_t numberVAB = reinterpret_cast<uint64_t>(vab.get());
            number |= numberVAB << (i * Hash_BindingPointMultiplyNumberBitMove);
            i++;

            return number;
        }

        virtual uint64_t GetIndexBufferNumber() const override
        {
            const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
            const Ref<IndexBuffer>& ib = Geometry::GetIndexBuffer(meshSingle);
            const uint64_t number = reinterpret_cast<uint64_t>(ib.get());
            return number;
        }

        virtual uint64_t GetIndirectBufferNumber() const override
        {
            const uint64_t number = m_Resources.GetIndirectBufferNumber();
            return number;
        }


        virtual uint64_t GetTextureNumber() const override
        {
            const uint64_t number = m_Resources.GetTextureNumber();
            return number;
        }

        virtual uint64_t GetUniformBufferNumber() const override
        {
            uint64_t number = 0ull;
            const uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
            number |= cameraNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_MainCamera);
            number |= m_Resources.GetUniformBufferNumber();
            return number;
        }

        virtual uint64_t GetStorageBufferNumber() const override
        {
            const uint64_t number = m_Resources.GetStorageBufferNumber();
            return number;
        }


        bool HasVAOFromMeshSingleSomeBuffer()
        {
            Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
            RY_CORE_ASSERT(nullptr != meshSingle);

            if (nullptr == m_VertexArray)
                return false;

            const bool result = Geometry::HasVAOFromMeshSingleIndexBuffer(m_VertexArray, meshSingle)
                || HasVAOFromMeshSingleVertexBuffer(m_VertexArray, meshSingle);
            return result;
        }

        PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, const int entityID) override
        {
            int result = CheckSubmitMeshObject(shader, singleMesh);
            if (State_MaxEntityRender <= m_InstanceCount)
            {
                result = result | Result_NoRenderObjectSpaceLeft;
                return static_cast<PiplineResultState>(result);
            }
            if (0 == BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft))
            {
                if(m_InstanceCount != storeIndex)
                    storeIndex = m_InstanceCount;

                m_SingleMeshObject = singleMesh;
                m_Shader = shader;

                m_Resources.AcquireFromMaterial(m_SingleMeshObject.m_Material);


                if(CheckVAO())
                    SetObjectForce(storeIndex, model, entityID);
                else
                    SetObject(storeIndex, model, entityID);

                m_InstanceCount++;

                m_DrawsAfterLastUpdate = 0u;
                result = Result_Success;
                return static_cast<PiplineResultState>(result);
            }
            const PiplineResultState resultState = static_cast<PiplineResultState>(result);
            // PrintPlineResult(resultState);
            return resultState;
        }

        virtual void Clear() override
        {
            InstanceMeshPiplineRenderBase::Clear();
            m_Resources.Clear();
            RY_DESTROY_REF(m_SingleMeshObject.m_Material);
            RY_DESTROY_REF(m_SingleMeshObject.m_MeshSingle);

        }

        void DrawNow(int flags) override
        {
            if (nullptr == m_Shader
                || nullptr == m_VertexArray
                || nullptr == m_CameraBuffer
                || m_Resources.CheckRefNotValid())
            {
                RY_CORE_ERROR("Draw call failed, because core resources are not set!");
                return;
            }
            BeforeDrawCall();     // shared: grows/uploads m_ModelBufferVAO using InstanceLayout::GetVertexLayout()

            ShaderDrawResource drawList = CreateShaderDrawResource();
            drawList.m_RenderMode = m_SingleMeshObject.m_Material->GetShadeRenderMode();
            drawList.m_ShaderProgram = m_Shader;
            drawList.m_VAO = m_VertexArray;
            drawList.m_DrawElement = Geometry::GetDrawObject(m_SingleMeshObject.m_MeshSingle);
            drawList.m_DrawElement.m_InstancesCount = m_InstanceCount;

            BindResources(drawList);      // base binds shader+camera, then m_Resources.Bind()


            std::vector<ShaderDrawResource>& shaderDrawResourceVec = Renderer::GetShaderDrawResourceMain();
            shaderDrawResourceVec.emplace_back(drawList);
        }

        virtual Ref<PiplineRenderBase> Copy() const override
        {
            using PipelineRenderImplType = PipelineRenderImpl<InstanceLayout, Resources, Geometry>;
            return CreateRef<PipelineRenderImplType>(*this);
        }


    protected:
        bool CheckVAO()
        {

            if (HasVAOFromMeshSingleSomeBuffer())
                return false;

            RY_DESTROY_REF(m_VertexArray);
            const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
            const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
            const Ref<IndexBuffer>& indexBufferAttributeDepth = Geometry::GetIndexBuffer(meshSingle);

            m_VertexArray = VertexArray::Create();
            m_VertexArray->AddVertexBuffer(vab);
            m_VertexArray->SetIndexBuffer(indexBufferAttributeDepth);

            return true;
        }

        virtual void BeforeDrawCall() override
        {
            if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
            {
                RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
            }
            m_DrawsAfterLastUpdate++;

            if (nullptr == m_ModelBufferVAO)
            {
                const RenderObject* dataPtr = m_RenderObjectVec.data();
                const uint32_t count = m_RenderObjectVec.size();
                RY_CORE_ASSERT(m_InstanceCount <= count, "To many Object, No data for that many Objects!");
                const uint32_t bytesSize = m_InstanceCount * sizeof(RenderObject);

                const BufferLayout layout = InstanceLayout::GetVertexLayout();

                m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
                const uint32_t bufferCount = m_VertexArray->GetVertexBuffersCount();
                RY_CORE_ASSERT(1 == bufferCount, "Expected Exact 1 Vertex Buffers in VAO! (GeometryBuffer/MeshBuffer) To add The Vertex Object Buffer");
                m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);

                m_UpdateModelBuffer = false;
            }
            else if (m_UpdateModelBuffer)
            {
                const RenderObject* dataPtr = m_RenderObjectVec.data();
                const uint32_t count = m_RenderObjectVec.size();
                RY_CORE_ASSERT(m_InstanceCount <= count, "To many Object, No data for that many Objects!");
                const uint32_t bytesSize = m_InstanceCount * sizeof(RenderObject);
                const uint32_t bufferBytesSize = m_ModelBufferVAO->GetByteSize();
                const uint32_t halfByteSize = bufferBytesSize / 2u;

                if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
                {
                    m_ModelBufferVAO->SetData(dataPtr, bytesSize);

                    if (2 != m_VertexArray->GetVertexBuffersCount())
                        m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
                }
                else
                {
                    m_ModelBufferVAO->ResizeBuffer(dataPtr, bytesSize);

                    if (2 != m_VertexArray->GetVertexBuffersCount())
                        m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);

                }
                m_UpdateModelBuffer = false;
            }
        }

        virtual void BindResources() override
        {
            InstanceMeshPiplineRenderBase::BindResources();
            m_Resources.Bind();
        }

        void BindResources(ShaderDrawResource& drawList)
        {
            drawList.GetBindUniform().at(UniformBinding_MainCamera) = m_CameraBuffer;
            m_Resources.Bind(drawList);
        }

        virtual void UnbindResources() override
        {
            InstanceMeshPiplineRenderBase::UnbindResources();
            m_Resources.Unbind();
        }



        int CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
        {
            int result = Result_None;

            CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
            CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);

            const Ref<Material>& materiel = singleMesh.m_Material;
            Ref<Material>& materielThis = m_SingleMeshObject.m_Material;

            result |= m_Resources.CheckObject(materiel, materielThis);
            return result;
        }

        void CheckBatchIndex(uint32_t& instanceIndex)
        {
            constexpr uint32_t MAX_UINT32 = std::numeric_limits<uint32_t>::max();
            const bool indexBevorOutOffBounce = MAX_UINT32 != instanceIndex;
            if (instanceIndex < m_EntityIDVec.size() && indexBevorOutOffBounce)
            {
                RY_CORE_FATAL_IF(!indexBevorOutOffBounce, "index get out off Bounce, from uint32!");
                return;
            }

            instanceIndex = m_EntityIDVec.size();
            m_RenderObjectVec.template emplace_back<RenderObject>(RenderObject{});
            m_EntityIDVec.emplace_back<int>(-1);
        }

        void SetObject(uint32_t& instanceIndex, const glm::mat4& modelMatrix, const int entityID)
        {
            CheckBatchIndex(instanceIndex);
            int& entity = m_EntityIDVec.at(instanceIndex);
            RenderObject& object = m_RenderObjectVec.at(instanceIndex);
            if (entity != entityID || !InstanceLayout::Equal(object, modelMatrix, entityID))
            {
                object = InstanceLayout::Make(modelMatrix, entityID);
                entity = entityID;

                m_UpdateModelBuffer = true;
            }
        }

        void SetObjectForce(uint32_t& instanceIndex, const glm::mat4& modelMatrix, const int entityID)
        {
            CheckBatchIndex(instanceIndex);
            int& entity = m_EntityIDVec.at(instanceIndex);
            RenderObject& object = m_RenderObjectVec.at(instanceIndex);

            object = InstanceLayout::Make(modelMatrix, entityID);
            entity = entityID;
            m_UpdateModelBuffer = true;
        }
    private:
        std::vector<RenderObject> m_RenderObjectVec;
        std::vector<int>          m_EntityIDVec;
        Resources                 m_Resources;

        bool                      m_UpdateModelBuffer;
    };


#if 0


#ifdef RY_INSTANCE_MESH_PIPLINE_RENDER_SHADE_TEMPLATE
    using InstanceMeshPiplineRenderShade = PipelineRenderImpl<Shade::InstanceLayout, Shade::Resources, Shade::Geometry>;
#endif

#ifdef RY_INSTANCE_MESH_PIPLINE_RENDER_DEPTH_TEMPLATE
    using InstanceMeshPiplineRenderDepth = PipelineRenderImpl<Depth::InstanceLayout, Depth::Resources,  Depth::Geometry>;
#endif

#ifdef RY_INSTANCE_MESH_PIPLINE_RENDER_SHAPE_TEMPLATE
    using InstanceMeshPiplineRenderShape = PipelineRenderImpl<Depth::InstanceLayout, Shape::Resources, Shade::Geometry>;
#endif


#endif
}




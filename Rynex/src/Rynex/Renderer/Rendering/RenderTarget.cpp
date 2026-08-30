#include "rypch.h"
#include "RenderTarget.h"
#include <Rynex/Renderer/RenderCommand.h>
#include <Rynex/Renderer/Rendering/Renderer.h>

#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

#define RY_CHECK_FOR_DOUBLE_PIPLINES 0
namespace Rynex {

    namespace Utils {

        template<typename T, typename N>
        static bool EmplaceSet(const Ref<N>& resourceSet, T& resourceArray)
        {
            for (Ref<N>& resource : resourceArray)
            {
                if (nullptr == resource || resource == resourceSet)
                {
                    resource = resourceSet;
                    return true;
                }
            }
            RY_CORE_ASSERT("No Empty Place Found!");
            return false;
        }
#ifdef RY_SSBO_VARIENTS

        template<>
        bool EmplaceSet(const Ref<StorageBuffer>& resourceSet, StorageBindArray& resourceArray)
        {
            for (auto& resource : resourceArray)
            {
                if (std::visit([&resourceSet](auto& resource) {return nullptr == resource || resource == resourceSet; }, resource))
                {
                    resource = resourceSet;
                }
            }
            RY_CORE_ASSERT("No Empty Place Found!");
            return false;
        }

        template<>
        bool EmplaceSet(const Ref<BindlesTextureArray>& resourceSet, StorageBindArray& resourceArray)
        {
            for (auto& resource : resourceArray)
            {
                if (std::visit([&resourceSet](auto& resource) {return nullptr == resource || resource == resourceSet; }, resource))
                {
                    resource = resourceSet;
                }
            }
            RY_CORE_ASSERT("No Empty Place Found!");
            return false;
        }
#endif

#ifdef RY_TEXTURE_VARIENTS
        template<>
        bool EmplaceSet(const Ref<Texture>& resourceSet, TextureBindArray& resourceArray)
        {
            for (auto& resource : resourceArray)
            {
                if (std::visit([&resourceSet](auto& resource) 
                    {
                        if constexpr (std::is_same_v<std::decay_t<decltype(resource)>, Ref<Texture>>)
                            return nullptr == resource || resource == resourceSet;
                        return false;
                    }, resource))
                {
                    return true;
                }
            }
            RY_CORE_ASSERT("No Empty Place Found!");
            return false;
        }

        template<>
        bool EmplaceSet(const Ref<LinkedTextureArray>& resourceSet, TextureBindArray& resourceArray)
        {
            for (auto& resource : resourceArray)
            {
                if (std::visit([&resourceSet](auto& resource)
                    {
                        if constexpr (std::is_same_v<std::decay_t<decltype(resource)>, Ref<LinkedTextureArray>>)
                            return nullptr == resource || resource == resourceSet;
                        return false;
                    }, resource))
                {
                    return true;
                }
            }
            RY_CORE_ASSERT("No Empty Place Found!");
            return false;
        }
#endif

        template<typename T>
        static void FindEmptyAndEmplaceSet(const T& resourceEmptyFind, T& resourceArray)
        {
            for (const auto& resource : resourceEmptyFind)
            {
                if (nullptr != resource)
                    EmplaceSet<T>(resource, resourceArray);
            }
        }
#ifdef RY_SSBO_VARIENTS
        template<>
        void FindEmptyAndEmplaceSet<StorageBindArray>(const StorageBindArray& resourceEmptyFind, StorageBindArray& resourceArray)
        {
            for (const auto& resource : resourceEmptyFind)
            {
               
                std::visit([&resourceArray](const auto& refResource)
                    {
                        if (nullptr != refResource)
                            return;
                        EmplaceSet(refResource, resourceArray);
                    }, resource);
            }
        }
#endif

#ifdef RY_TEXTURE_VARIENTS
        template<>
        void FindEmptyAndEmplaceSet<TextureBindArray>(const TextureBindArray& resourceEmptyFind, TextureBindArray& resourceArray)
        {
            for (const auto& resource : resourceEmptyFind)
            {

                std::visit([&resourceArray](const auto& refResource)
                    {
                        if (nullptr != refResource)
                            return;
                        EmplaceSet(refResource, resourceArray);

                    }, resource);
                
            }
        }
#endif

#if defined(RY_TEXTURE_VARIENTS) || defined(RY_SSBO_VARIENTS)
        static void FindEmptyEqualPalaceShaderDraw(const Ref<ShaderDrawList>& setShaderDraw, std::vector<Weak<ShaderDrawList>>& shaderDrawList)
        {
            for (Weak<ShaderDrawList>& shaderDraw : shaderDrawList)
            {
                if (Ref<ShaderDrawList> drawList = shaderDraw.lock())
                {
                    if (drawList->shaderProgramm == setShaderDraw->shaderProgramm)
                    {
                        FindEmptyAndEmplaceSet(setShaderDraw->GetBindeTextures(), drawList->GetBindeTextures());
                        FindEmptyAndEmplaceSet(setShaderDraw->GetBindeUniform(), drawList->GetBindeUniform());
                        FindEmptyAndEmplaceSet(setShaderDraw->GetBindeStorage(), drawList->GetBindeStorage());
                        return;
                    }
                    else if (drawList->shaderProgramm == nullptr)
                    {
                        shaderDraw = setShaderDraw;
                        return;
                    }
                }
            }
            shaderDrawList.emplace_back(setShaderDraw);
        }
#endif


        template<typename N>
        static void BindOnArrayIndex(N& bindArray)
        {
#ifdef RY_TEXTURE_VARIENTS
            static_assert(!std::is_same_v<N, TextureBindArray>, "This type is not aloowed here!");
#endif

#ifdef RY_SSBO_VARIENTS
            static_assert(!std::is_same_v<N, StorageBuffer>, "This type is not aloowed here!");
#endif
            uint32_t index = 0u;
            for (auto& resource : bindArray)
            {
                if (nullptr != resource)
                {
                    // if(resource->IsTransferd())
                        resource->Bind(index);
                }

                index++;
            }
        }


        template<typename N>
        static void UnBindOnArrayIndex(N& bindArray)
        {
#ifdef RY_TEXTURE_VARIENTS
            static_assert(!std::is_same_v<N, TextureBindArray>, "This type is not aloowed here!");
#endif
            uint32_t index = 0u;
            for (const auto& resource : bindArray)
            {
                if (nullptr != resource)
                    resource->UnBind(index);

                index++;
            }
        }
        template<typename N>
        static void BindOnArrayIndexOrder(const N& bindArray)
        {
#ifdef RY_TEXTURE_VARIENTS
            static_assert(!std::is_same_v<N, TextureBindArray>, "This type is not aloowed here!");
#endif
            uint32_t index = 0u;
            for (const auto& resource : bindArray)
            {
                if (nullptr == resource)
                    return;
                resource->Bind(index);
                index++;
            }
        }

#ifdef RY_TEXTURE_VARIENTS
        template<>
        void UnBindOnArrayIndex<TextureBindArray>(TextureBindArray& bindArray)
        {
            uint32_t index = 0u;
            for (const auto& resource : bindArray)
            {
                if (std::visit([index](auto& resourceValue)
                    {
                        if(nullptr == resourceValue)
                            return true;
                        resourceValue->UnBind(index);
                        return false;
                    }, resource))
                    return;

                index++;
            }
        }

        template<>
        void BindOnArrayIndex<TextureBindArray>(TextureBindArray& bindArray)
        {

            uint32_t index = 0u;
            for (auto& resource : bindArray)
            {
               std::visit([index](auto& resourceValue)
                    {
                        if (nullptr != resourceValue)
                            resourceValue->Bind(index);
                    }, resource);

                index++;
            }
        }

        template<>
        void BindOnArrayIndexOrder<TextureBindArray>(const TextureBindArray& bindArray)
        {
            uint32_t index = 0u;
            for (const auto& resource : bindArray)
            {
                if (std::visit([index](auto& resourceValue)
                    {
                        if (nullptr == resourceValue)
                            return true;
                        resourceValue->Bind(index);
                        return false;
                    }, resource))
                    return;

                index++;
            }
        }
#endif

#ifdef RY_SSBO_VARIENTS
        template<>
        void UnBindOnArrayIndex<StorageBindArray>(StorageBindArray& bindArray)
        {
            uint32_t index = 0u;
            for (const auto& resource : bindArray)
            {
                if (std::visit([index](auto& resourceValue)
                    {
                        if (nullptr == resourceValue)
                            return true;
                        resourceValue->UnBind(index);

                        if constexpr (std::is_same_v<decltype(resourceValue), BindlesTextureArray>)
                            resourceValue->DeactivateTextures();

                        return false;
                    }, resource))
                    return;

                index++;
            }
        }

        template<>
        void BindOnArrayIndex<StorageBindArray>(StorageBindArray& bindArray)
        {

            uint32_t index = 0u;
            for (auto& resource : bindArray)
            {
                std::visit([index](auto& resourceValue)
                    {
                        if (nullptr == resourceValue)
                            return;

                        if constexpr (std::is_same_v<decltype(resourceValue), BindlesTextureArray>)
                            resourceValue->AktivateTextures();


                        resourceValue->Bind(index);


                    }, resource);

                index++;
            }
        }

        template<>
        void BindOnArrayIndexOrder<StorageBindArray>(const StorageBindArray& bindArray)
        {
            uint32_t index = 0u;
            for (const auto& resource : bindArray)
            {
                if (std::visit([index](auto& resourceValue)
                    {
                        if (nullptr == resourceValue)
                            return true;

                        if constexpr (std::is_same_v<decltype(resourceValue), BindlesTextureArray>)
                            resourceValue->AktivateTextures();

                        resourceValue->Bind(index);
                        return false;
                    }, resource))
                    return;

                index++;
            }
        }
#endif
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        static void BindResources(ShaderDrawResource_RefPtr setShaderDraw)
        {
            setShaderDraw->shaderProgramm->Bind();
            
            BindOnArrayIndex<StorageBuffer>(setShaderDraw->GetBindeStorage());
            BindOnArrayIndex<Texture>(setShaderDraw->GetBindeTextures());
            BindOnArrayIndex<UniformBuffer>(setShaderDraw->GetBindeUniform());
        }

        static void UnBindResources(ShaderDrawResource_RefPtr setShaderDraw)
        {
            setShaderDraw->shaderProgramm->UnBind();

            UnBindOnArrayIndex<Texture>(setShaderDraw->GetBindeTextures());
            UnBindOnArrayIndex<StorageBuffer>(setShaderDraw->GetBindeStorage());
            UnBindOnArrayIndex<UniformBuffer>(setShaderDraw->GetBindeUniform());
        }

        static void DrawObjectBufferListShader(ShaderDrawResource_RefPtr setShaderDraw, int renderMode)
        {
            if (nullptr == setShaderDraw->shaderProgramm || nullptr == setShaderDraw->vao)
                return;
            RenderCommand::SetMode(renderMode);
            BindResources(setShaderDraw);

            const Ref<IndirectBuffer>& drawBuffer = setShaderDraw->drawBuffer;
            const Mesh::PerDrawObject& drawElement = setShaderDraw->drawElement;

            if (nullptr != drawBuffer)
            {
                RenderCommand::DrawMultyMeshIndriect(setShaderDraw->vao, drawBuffer);
                drawBuffer->UnBind();
            }
            else if(0u != drawElement.Count)
            {
                RenderCommand::DrawElement(setShaderDraw->vao, drawElement);
            }
            else
            {
                RenderCommand::DrawIndexedMesh(setShaderDraw->vao, setShaderDraw->indicesCount);
            }
            UnBindResources(setShaderDraw);
            setShaderDraw->vao->UnBind();
            setShaderDraw->shaderProgramm->UnBind();
        }

        static void DrawObjectBufferListShader(ShaderDrawResource_RefPtr setShaderDraw)
        {
            DrawObjectBufferListShader(setShaderDraw, setShaderDraw->renderMode);
        }

        static void DrawSingleObjectShader(ShaderDrawResource_RefPtr setShaderDraw, int renderMode)
        {
            if (nullptr == setShaderDraw->shaderProgramm || nullptr == setShaderDraw->vao)
                return;

            RenderCommand::SetMode(renderMode);
            BindResources(setShaderDraw);
            
            const Ref<IndirectBuffer>& drawBuffer = setShaderDraw->drawBuffer;
            const Mesh::PerDrawObject& drawElement = setShaderDraw->drawElement;

            if (nullptr != drawBuffer)
            {
                uint32_t count = drawBuffer->GetCount();
                RY_CORE_NOT_IMPL();
            }
            else if(0u != drawElement.Count)
            {
                RenderCommand::DrawElement(setShaderDraw->vao, drawElement);
            }
            else
            {
                RenderCommand::DrawIndexedMesh(setShaderDraw->vao, setShaderDraw->indicesCount);
            }
            setShaderDraw->vao->UnBind();
            setShaderDraw->shaderProgramm->UnBind();
        }
        static void DrawSingleObjectShader(ShaderDrawResource_RefPtr setShaderDraw)
        {
            DrawSingleObjectShader(setShaderDraw, setShaderDraw->renderMode);
        }

        static void DrawObjectShaderOrder(Const_ShaderDrawResource_RefPtr setShaderDraw)
        {
            if (nullptr != setShaderDraw->shaderProgramm)
                return;
            
            setShaderDraw->shaderProgramm->Bind();

            BindOnArrayIndexOrder<Texture>(setShaderDraw->GetBindeTextures());
            BindOnArrayIndexOrder<UniformBuffer>(setShaderDraw->GetBindeUniform());
            BindOnArrayIndexOrder<StorageBuffer>(setShaderDraw->GetBindeStorage());
            

            if (nullptr != setShaderDraw->drawBuffer)
            {
                RenderCommand::DrawMultyMeshIndriect(setShaderDraw->vao, setShaderDraw->drawBuffer);
                setShaderDraw->drawBuffer->UnBind();
            }
            else
                RenderCommand::DrawIndexedMesh(setShaderDraw->vao, setShaderDraw->indicesCount);
            
            setShaderDraw->vao->UnBind();
            setShaderDraw->shaderProgramm->UnBind();
        }
#else
        static void BindResources(ShaderDrawList& setShaderDraw)
        {
            setShaderDraw.shaderProgramm->Bind();
            
            BindOnArrayIndex(setShaderDraw.GetBindeStorage());
            BindOnArrayIndex<TextureBindArray>(setShaderDraw.GetBindeTextures());
            BindOnArrayIndex(setShaderDraw.GetBindeUniform());
        }

        static void UnBindResources(ShaderDrawList& setShaderDraw)
        {
            setShaderDraw.shaderProgramm->UnBind();

            UnBindOnArrayIndex(setShaderDraw.GetBindeStorage());
            UnBindOnArrayIndex<TextureBindArray>(setShaderDraw.GetBindeTextures());
            UnBindOnArrayIndex(setShaderDraw.GetBindeUniform());
        }

        static void DrawObjectBufferListShader(ShaderDrawList& setShaderDraw, int renderMode)
        {
            if (nullptr == setShaderDraw.shaderProgramm || nullptr == setShaderDraw.vao)
                return;
            RenderCommand::SetMode(renderMode);
            BindResources(setShaderDraw);

            const Ref<IndirectBuffer>& drawBuffer = setShaderDraw.drawBuffer;
            const Mesh::PerDrawObject& drawElement = setShaderDraw.drawElement;

            if (nullptr != drawBuffer)
            {
                RenderCommand::DrawMultyMeshIndriect(setShaderDraw.vao, drawBuffer);
#ifdef RY_UNBIND_AFTER_DRAW
                drawBuffer->UnBind();
#endif
            }
            else if(0u != drawElement.Count)
            {
                RenderCommand::DrawElement(setShaderDraw.vao, drawElement);
            }
            else
            {
                RenderCommand::DrawIndexedMesh(setShaderDraw.vao, setShaderDraw.indicesCount);
            }
#ifdef RY_UNBIND_AFTER_DRAW
            UnBindResources(setShaderDraw);
            setShaderDraw.vao->UnBind();
            setShaderDraw.shaderProgramm->UnBind();
#endif

        }

        static void DrawObjectBufferListShader(ShaderDrawList& setShaderDraw)
        {
            DrawObjectBufferListShader(setShaderDraw, setShaderDraw.renderMode);
        }

        static void DrawSingleObjectShader(ShaderDrawList& setShaderDraw, int renderMode)
        {
            if (nullptr == setShaderDraw.shaderProgramm || nullptr == setShaderDraw.vao)
                return;

            RenderCommand::SetMode(renderMode);
            BindResources(setShaderDraw);
            
            const Ref<IndirectBuffer>& drawBuffer = setShaderDraw.drawBuffer;
            const Mesh::PerDrawObject& drawElement = setShaderDraw.drawElement;

            if (nullptr != drawBuffer)
            {
                uint32_t count = drawBuffer->GetCount();
                RY_CORE_NOT_IMPL();
            }
            else if(0u != drawElement.Count)
            {
                RenderCommand::DrawElement(setShaderDraw.vao, drawElement);
            }
            else
            {
                RenderCommand::DrawIndexedMesh(setShaderDraw.vao, setShaderDraw.indicesCount);
            }
#ifdef RY_UNBIND_AFTER_DRAW
            setShaderDraw.vao->UnBind();
            setShaderDraw.shaderProgramm->UnBind();
#endif
        }
        static void DrawSingleObjectShader(ShaderDrawList& setShaderDraw)
        {
            DrawSingleObjectShader(setShaderDraw, setShaderDraw.renderMode);
        }

        static void DrawObjectShaderOrder(const ShaderDrawList& setShaderDraw)
        {
            if (nullptr != setShaderDraw.shaderProgramm)
                return;
            
            setShaderDraw.shaderProgramm->Bind();

            BindOnArrayIndexOrder(setShaderDraw.GetBindeUniform());
            BindOnArrayIndexOrder<TextureBindArray>(setShaderDraw.GetBindeTextures());
            BindOnArrayIndexOrder(setShaderDraw.GetBindeStorage());
            

            if (nullptr != setShaderDraw.drawBuffer)
            {
                RenderCommand::DrawMultyMeshIndriect(setShaderDraw.vao, setShaderDraw.drawBuffer);
#ifdef RY_UNBIND_AFTER_DRAW
                setShaderDraw.drawBuffer->UnBind();
#endif
            }
            else
                RenderCommand::DrawIndexedMesh(setShaderDraw.vao, setShaderDraw.indicesCount);
#ifdef RY_UNBIND_AFTER_DRAW
            setShaderDraw.vao->UnBind();
            setShaderDraw.shaderProgramm->UnBind();
#endif
        }
#endif


    }
    RenderTarget::RenderTarget()
        : m_FB(nullptr)
        , m_RenderViewSize(glm::vec4{ 1.0f, 1.0f, 0.0f,0.0f })
    {
    }

    RenderTarget::RenderTarget(const Ref<Framebuffer>& fb)
        : m_FB(fb)
        , m_RenderViewSize(glm::vec4{ 1.0f, 1.0f, 0.0f,0.0f})
    {
        uint32_t count = m_FB->GetAttachmentTexturesSize();
        m_ClearColorFuncVec.resize(count);
        const FramebufferSpecification& fbSpec = fb->GetFramebufferSpecification();
        m_RenderViewSize = glm::vec4{ fbSpec.Width, fbSpec.Height, 0.0f, 0.0f };
    }

    Ref<RenderTarget> RenderTarget::Copy(const Ref<RenderTarget>& src)
    {
       
        if(nullptr == src)
            return nullptr;
        Ref<RenderTarget> dst = CreateRef<RenderTarget>();

        dst->m_RenderViewSize = src->m_RenderViewSize;
        if(nullptr != src->m_FB)
        {
            Ref<Framebuffer> fbDst = Framebuffer::Create(src->m_FB->GetFramebufferSpecification());
            dst->SetFramebuffer(fbDst);
        }
        
        return dst;
    }

    RenderTarget::~RenderTarget()
    {
        RY_DESTROY_REF(m_FB);
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        m_ShadeDrawList.Clear();
#else
        m_ShadeDrawList.clear();
#endif
    }

    void RenderTarget::AddDrawPassVec(std::vector<ShaderDrawResource>& drawListVec)
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (const ShaderDrawResource& drawList : drawListVec)
        {
            AddDrawPass(drawList);
        }
#else
        m_ShadeDrawList.insert(m_ShadeDrawList.end(), drawListVec.begin(), drawListVec.end());
        
#endif

    }

    void RenderTarget::AddDrawPass(Const_ShaderDrawResource_Ref drawList)
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR

        Ref<Shader> shader = drawList->shaderProgramm;
        if (shader == nullptr)
            return;
        if (!m_ShadeDrawList.HasObject(drawList))
            m_ShadeDrawList.AddObject(drawList);
#else
        m_ShadeDrawList.push_back(drawList);
#endif

    }



    ShaderDrawResource RenderTarget::GetDrawPassOrAdd(const Ref<Shader>& shader)
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (Weak<ShaderDrawList>& shaderDrawList : m_ShadeDrawList)
        {
            if(Ref<ShaderDrawList> drawList = shaderDrawList.lock())
            {
                const Ref<Shader>& shaderCall = drawList->shaderProgramm;
                if (shaderCall.get() == shader.get())
                {
                    return drawList;
                }
            }
        }
        ShaderDrawResource drawList = CreateShaderDrawResource();
        drawList->shaderProgramm = shader;
#else
        for (Const_ShaderDrawResource_Ref drawList : m_ShadeDrawList)
        {
            const Ref<Shader>& shaderCall = drawList.shaderProgramm;
            if (shaderCall.get() == shader.get())
            {
                return drawList;
            }
        }
        ShaderDrawResource drawList = CreateShaderDrawResource();
        drawList.shaderProgramm = shader;
#endif
        return drawList;
    }

    bool RenderTarget::HasDrawPass(Const_ShaderDrawResource_Ref drawList)
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR

        return m_ShadeDrawList.HasObject(drawList);
#else
        RY_CORE_NOT_IMPL();
        return false;
#endif

    }
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
    bool RenderTarget::HasDrawPass(Const_ShaderDrawResourceWeak_Ref drawList)
    {
        return m_ShadeDrawList.HasObject(drawList);
    }
#endif

    bool RenderTarget::HasDrawPass(const Ref<Shader>& shader, uint32_t index)const
    {
        

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        RY_CORE_ASSERT(index < m_ShadeDrawList.Size(), "To hige Index");
        const Weak<ShaderDrawList>& shaderDraw = m_ShadeDrawList.GetDataFromIndex(index);
        if (Ref<ShaderDrawList> drawList = shaderDraw.lock())
        {
            const Ref<Shader>& shaderCall = drawList->shaderProgramm;
            bool is = shaderCall.get() == shader.get();
            return is;
        }
        return false;
#else
        RY_CORE_ASSERT(index < m_ShadeDrawList.size(), "To hige Index");
        Const_ShaderDrawResource_Ref shaderDraw = m_ShadeDrawList.at(index);
        
         const Ref<Shader>& shaderCall = shaderDraw.shaderProgramm;
         bool is = shaderCall.get() == shader.get();
         return is;
#endif
    }

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
    Weak<ShaderDrawList>& RenderTarget::GetDrawPass(uint32_t index)
    {
        RY_CORE_ASSERT(index < m_ShadeDrawList.Size(), "To hige Index");
        Weak<ShaderDrawList>& shaderDraw = m_ShadeDrawList.GetDataFromIndex(index);
        return shaderDraw;
    }
#else
    ShaderDrawList& RenderTarget::GetDrawPass(uint32_t index)
    {
        RY_CORE_ASSERT(index < m_ShadeDrawList.size(), "To hige Index");
        ShaderDrawList& shaderDraw = m_ShadeDrawList.at(index);
        return shaderDraw;
    }
#endif


    uint32_t RenderTarget::AddNewDrawPass(const Ref<Shader>& shader, Const_ShaderDrawResourceWeak_Ref shaderDrawList)
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        if (m_ShadeDrawList.HasObject(shaderDrawList))
        {
            uint32_t index = m_ShadeDrawList.GetObjectIndex(shaderDrawList);
            return index;
        }

        uint32_t index = m_ShadeDrawList.AddObjectIndex(shaderDrawList);
        if(Ref<ShaderDrawList> drawList = shaderDrawList.lock())
        {
            drawList->shaderProgramm = shader;
        }
        RY_CORE_ASSERT(index < m_ShadeDrawList.Size());
#else

        uint32_t index = MAXUINT32;
#endif
        RY_REMBER_FUNC_CHANGE("We need maby a suolution for this funtion or need to delted!");
        return index;
    }

    void RenderTarget::BindFramebuffer() const
    {
        m_FB->Bind(m_RenderViewSize.x, m_RenderViewSize.y, m_RenderViewSize.z, m_RenderViewSize.w);
    }


  
    
    void RenderTarget::UnbindFB() const
    {
        m_FB->UnBind();
    }

   

   

    int RenderTarget::DrawBufferList()
    {
        int renderModeBevorDraw = Renderer::GetMode();
        uint32_t drawListCount = 0u;

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (Weak<ShaderDrawList>& shaderDraw : m_ShadeDrawList)
        {
            if (Ref<ShaderDrawList> drawList = shaderDraw.lock())
            {
                Utils::DrawObjectBufferListShader(drawList);
                drawListCount++;
            }
        }
#else
        for (ShaderDrawList& drawList : m_ShadeDrawList)
        {
            Utils::DrawObjectBufferListShader(drawList);
            drawListCount++;
        }
#endif
        CheckExexuteDrawList(drawListCount);

        return renderModeBevorDraw;
    }

    int RenderTarget::DrawBufferList(int renderMode)
    {
        int renderModeBevorDraw = Renderer::GetMode();
        uint32_t drawListCount = 0u;
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (Weak<ShaderDrawList>& shaderDraw : m_ShadeDrawList)
        {
            if (ShaderDrawResource drawList = shaderDraw.lock())
            {
                Utils::DrawObjectBufferListShader(drawList, renderMode);
                drawListCount++;
            }
        }
        CheckExexuteDrawList(drawListCount);

#else
        for (ShaderDrawList& drawList : m_ShadeDrawList)
        {
            Utils::DrawObjectBufferListShader(drawList, renderMode);
#if 0
            drawListCount++;
        }
        CheckExexuteDrawList(drawListCount);
#else
        }
#endif
#endif

        return renderModeBevorDraw;
    }


    int RenderTarget::DrawSingle()
    {
        int renderModeBevorDraw = Renderer::GetMode();
        uint32_t drawListCount = 0u;

#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (Weak<ShaderDrawList>& shaderDraw : m_ShadeDrawList)
        {
            if (ShaderDrawResource drawList = shaderDraw.lock())
            {
                Utils::DrawSingleObjectShader(drawList);
                drawListCount++;
            }
        }
#else
        for (ShaderDrawList& drawList : m_ShadeDrawList)
        {
            Utils::DrawObjectBufferListShader(drawList);
            drawListCount++;
        }
#endif

        CheckExexuteDrawList(drawListCount);

        return renderModeBevorDraw;
    }

    int RenderTarget::DrawSingle(int renderMode) 
    {
        int renderModeBevorDraw = Renderer::GetMode();
        uint32_t drawListCount = 0u;
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (Weak<ShaderDrawList>& shaderDraw : m_ShadeDrawList)
        {
            if (Ref<ShaderDrawList> drawList = shaderDraw.lock())
            {
                Utils::DrawSingleObjectShader(drawList, renderMode);
                drawListCount++;
            }
        }
#else
        for (ShaderDrawList& drawList : m_ShadeDrawList)
        {
            Utils::DrawObjectBufferListShader(drawList, renderMode);
            drawListCount++;
        }
#endif

        CheckExexuteDrawList(drawListCount);

        return renderModeBevorDraw;
    }


    

    void RenderTarget::DrawSort() const
    {     
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR

        for (const Weak<ShaderDrawList>& shaderDraw : m_ShadeDrawList)
        {
            if (Ref<ShaderDrawList> drawList = shaderDraw.lock())
            {
                Utils::DrawObjectShaderOrder(drawList);
            }
        }
#else
        for (const ShaderDrawList& drawList : m_ShadeDrawList)
        {
            Utils::DrawObjectShaderOrder(drawList);
        }
#endif

    }

    void RenderTarget::SortShaderDraw()
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (Weak<ShaderDrawList>& shaderDraw : m_ShadeDrawList)
        {
            if (Ref<ShaderDrawList> drawList = shaderDraw.lock())
            {
#else
        for (ShaderDrawList& drawListRef : m_ShadeDrawList)
        {
            ShaderDrawList* drawList = &drawListRef;
#endif

                std::sort(drawList->GetBindeTextures().begin(), drawList->GetBindeTextures().end(),
#ifdef RY_TEXTURE_VARIENTS
                    [](auto& refA, auto& refB)
                    {
                        bool a = std::visit([](auto& value) { return nullptr != value.get(); }, refA);
                        bool b = std::visit([](auto& value) { return nullptr == value.get(); }, refB);
#else
                    [](Ref<Texture>& refA, Ref<Texture>& refB)
                    {
                        bool a = nullptr != refA.get();
                        bool b = nullptr == refB.get();
#endif
                        bool result = a && b;
                        return  result;
                    });

                std::sort(drawList->GetBindeUniform().begin(), drawList->GetBindeUniform().end(),
                    [](Ref<UniformBuffer>& refA, Ref<UniformBuffer>& refB)
                    {
                        bool a = nullptr != refA.get();
                        bool b = nullptr == refB.get();
                        bool result = a && b;
                        return  result;
                    });

                std::sort(drawList->GetBindeStorage().begin(), drawList->GetBindeStorage().end(),
#ifdef RY_SSBO_VARIENTS
                    [](auto& refA, auto& refB)
                    {
                        bool a = std::visit([](auto& value) { return nullptr != value.get(); }, refA);
                        bool b = std::visit([](auto& value) { return nullptr == value.get(); }, refB);
#else

                    [](Ref<StorageBuffer>& refA, Ref<StorageBuffer>& refB)
                    {
                        bool a = nullptr != refA.get();
                        bool b = nullptr == refB.get();
#endif
                        bool result = a && b;
                        return  result;
                    });
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
            }
#endif
        }
    }

    void RenderTarget::SortList()
    {     
#if 0
        std::sort(m_ShadeDrawList.begin(), m_ShadeDrawList.end(), [](Weak<ShaderDrawList>& aWeak, Weak<ShaderDrawList>& bWeak)
            {
                Ref<ShaderDrawList> aRef = aWeak.lock();
                Ref<ShaderDrawList>& bRef = bWeak.lock();

                return aRef->Shader.get() == bRef->Shader.get();
            });
#else
        std::sort(m_ShadeDrawList.begin(), m_ShadeDrawList.end(), &RenderTarget::SortRenderList);
#endif
    }

    void RenderTarget::SortePilineList()
    {
        std::sort(m_PilineBaseVec.begin(), m_PilineBaseVec.end(), &PiplineRenderBase::SortePiplineRenderBase);
    }

    void RenderTarget::SortePilineAlphaList()
    {
        std::sort(m_PilineAlphaBaseVec.begin(), m_PilineAlphaBaseVec.end(), &AlphaPiplineBase::SortByDistenz);
    }

    

    void RenderTarget::ResizeView(const glm::ivec4& renderViewSize)
    {
        glm::vec4 renderSize = glm::vec4(renderViewSize);
        if (m_RenderViewSize != renderSize)
        {
            m_RenderViewSize = renderSize;
            if (nullptr != m_FB)
            {
                glm::uvec2 size = m_FB->GetFrambufferSize();
                glm::uvec2 size2 = glm::uvec2(m_RenderViewSize);
                if(size != size2)
                {
                    m_FB->Resize2D(m_RenderViewSize.x, m_RenderViewSize.y);
                }
            }
        }

        
    }

    void RenderTarget::ResizeView(const glm::vec4& renderViewSize)
    {
        m_RenderViewSize = renderViewSize;
    }

    void RenderTarget::ClearShaderDrawList()
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        m_ShadeDrawList.Clear();
#else
        m_ShadeDrawList.clear();
#endif
    }

    void RenderTarget::SetFramebuffer(const Ref<Framebuffer>& fb)
    {
        m_ClearColorFuncVec.clear();
        m_FB = fb;
        if (nullptr == m_FB)
            return;
        uint32_t count = m_FB->GetAttachmentTexturesSize();
        m_FB->GetAttachmentsTextures();
        m_ClearColorFuncVec.resize(count);
        for (std::function<void(const Ref<Framebuffer>&, uint32_t)>& func : m_ClearColorFuncVec)
        {
            func = [](const Ref<Framebuffer>& fb, uint32_t index) {
                fb->ClearAttachmentNull(index);
            };
        }
       
    }

    void RenderTarget::SetClearColorAttachment(uint32_t index, glm::vec4 clearColor)
    {
        RY_CORE_ASSERT(index < m_ClearColorFuncVec.size(), "Buffer overflow");
        const FramebufferSpecification& fbSpec = m_FB->GetFramebufferSpecification();


        const FramebufferTextureSpecification& frameTexSpec = fbSpec.Attachments[index];
        if (IsDataTypeValidToAtachment(frameTexSpec, clearColor))
        {
            std::function<void(const Ref<Framebuffer>&, uint32_t)>& func = m_ClearColorFuncVec.at(index);
            func = [clearColor](const Ref<Framebuffer>& fb, uint32_t index) {
                glm::vec4 clearColorLocale = clearColor;
#if 0
                if (index != 0)
                    fb->ClearAttachment(index, clearColorLocale);
                else
                {
                    RenderCommand::SetClearColor(clearColorLocale);
                }
#else
                fb->ClearAttachment(index, clearColorLocale);
#endif
            };
        }
        else
        {
            RY_CORE_ERROR("The set attchment index is not in any expexted Texture formate to Clear it!");
        }
    }

    void RenderTarget::SetClearColorAttachment(uint32_t index, int clearColor)
    {
        RY_CORE_ASSERT(index < m_ClearColorFuncVec.size(), "Buffer overflow");
        const FramebufferSpecification& fbSpec = m_FB->GetFramebufferSpecification();


        const FramebufferTextureSpecification& frameTexSpec = fbSpec.Attachments[index];
        if (IsDataTypeValidToAtachment(frameTexSpec, clearColor))
        {
            std::function<void(const Ref<Framebuffer>&, uint32_t)>& func = m_ClearColorFuncVec.at(index);
            func = [clearColor](const Ref<Framebuffer>& fb, uint32_t index) {
                fb->ClearAttachment(index, clearColor);
            };
        }
        else
        {
            RY_CORE_ERROR("The set attchment index is not in any expexted Texture formate to Clear it!");
        }
    }

    std::vector<Ref<PiplineRenderBase>> RenderTarget::GetPiplineRefVecCopy() const
    {
        std::vector<Ref<PiplineRenderBase>> piplineVecCopy;
        piplineVecCopy.reserve(m_PilineBaseVec.size() + m_PilineAlphaBaseVec.size());

        for (const Ref<PiplineRenderBase>& pipline : m_PilineBaseVec)
        {
            piplineVecCopy.emplace_back(pipline->Copy());
        }

        for (const AlphaPiplineBase& alphaPiplineBase : m_PilineAlphaBaseVec)
        {
            piplineVecCopy.emplace_back(alphaPiplineBase.Pipline->Copy());
        }

        return piplineVecCopy;
    }

   

    bool RenderTarget::HasShader(const Ref<Shader>& shader) const
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        for (const Weak<ShaderDrawList>& shaderDraw : m_ShadeDrawList)
        {
            if (Ref<ShaderDrawList> drawList = shaderDraw.lock())
            {
#else
        for (const ShaderDrawList& drawListRef : m_ShadeDrawList)
        { 
            const ShaderDrawList* drawList = &drawListRef;
#endif
                if (drawList->shaderProgramm.get() == shader.get())
                    return true;
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
            }
#endif

        }
        return false;
    }

    void RenderTarget::AddPipline(Ref<PiplineRenderBase> pipline)
    {
#if RY_CHECK_FOR_DOUBLE_PIPLINES
        for (const Ref<PiplineRenderBase>& e : m_PilineBaseVec)
        {
            if (e == pipline)
                return;
        }
#endif
        m_PilineBaseVec.emplace_back(pipline);
    }

    void RenderTarget::AddPiplineAlpha(Ref<PiplineRenderBase> pipline, float distenz)
    {
        m_PilineAlphaBaseVec.emplace_back(pipline, distenz);
    }



#if 0
    Ref<PiplineRenderBase>& RenderTarget::GetPipline(uint32_t& index)
    {
        uint32_t count = m_PilineBaseVec.size();
        Ref<PiplineRenderBase> pipline = nullptr;
        if (index < count)
            pipline = m_PilineBaseVec.at(index);

        return pipline;


    }
   
    Ref<PiplineRenderBase>& RenderTarget::GetPiplineIndexOrPush(uint32_t& index)
    {

        uint32_t count = m_PilineBaseVec.size();
        if (count <= index)
        {
            m_PilineBaseVec.emplace_back<Ref<PiplineRenderBase>>(
                Ref<PiplineRenderBase>(nullptr)
            );
            index = count;
        }
        Ref<PiplineRenderBase>& pipline = m_PilineBaseVec.at(index);
        return pipline;
    }

    Ref<PiplineRenderBase> RenderTarget::GetPipline(uint32_t index)
    {
        uint32_t count = m_PilineBaseVec.size();
        Ref<PiplineRenderBase> pipline = nullptr;
        if (index < count)
            pipline = m_PilineBaseVec.at(index);

        return pipline;
    }


    Ref<PiplineRenderBase> RenderTarget::GetPiplineAlpha(uint32_t index)
    {

        uint32_t count = m_PilineAlphaBaseVec.size();
        Ref<PiplineRenderBase> pipline = nullptr;
        if (index < count)
        {
            RenderTarget::AlphaPiplineBase& alphaPiplineBase = m_PilineAlphaBaseVec.at(index);
            pipline = alphaPiplineBase.Pipline;
        }
        return pipline;
      

    }
    
    Ref<PiplineRenderBase>& RenderTarget::GetPiplineAlphaIndexOrPush(uint32_t& index, float distenz)
    {

        uint32_t count = m_PilineAlphaBaseVec.size();
        if (count <= index)
        {
            m_PilineAlphaBaseVec.emplace_back<RenderTarget::AlphaPiplineBase>(
                RenderTarget::AlphaPiplineBase{
                    Ref<PiplineRenderBase>(nullptr)
                    , distenz
                }
            );
            index = count;
        }
        RenderTarget::AlphaPiplineBase& alphaPiplineBase = m_PilineAlphaBaseVec.at(index);
        Ref<PiplineRenderBase>& pipline = alphaPiplineBase.Pipline;

        return pipline;
    }
#elif 0
    PiplineRefBaseVec& RenderTarget::GetPiplineBaseVec()
    {
        return m_PilineBaseDataVec;
    }


    int RenderTarget::DrawPilinesBase()
    {
        int renderModeBevorDraw = Renderer::GetMode();
        for (Ref<PiplineRenderBase>& pipline : m_PilineBaseDataVec)
        {
            pipline->DrawNow();
        }
        return renderModeBevorDraw;
    }

    int RenderTarget::DrawPilinesBase(int mode)
    {
        int renderModeBevorDraw = Renderer::GetMode();
        for (Ref<PiplineRenderBase>& pipline : m_PilineBaseDataVec)
        {
            pipline->DrawNow(mode);
        }
        return renderModeBevorDraw;

    }

    Ref<PiplineRenderBase>& RenderTarget::GetPipline(uint32_t& index)
    {
        Ref<PiplineRenderBase>& pipline = m_PilineBaseDataVec.GetPiplineBaseRef(index);
        return pipline;
    }

   
#endif

    


    void RenderTarget::SetPiplineAlphaDistenz(uint32_t index, float distenz)
    {
        uint32_t count = m_PilineAlphaBaseVec.size();
        RY_CORE_ASSERT(index < count, "Buffer Overflow!");
        RenderTarget::AlphaPiplineBase& alphaPiplineBase = m_PilineAlphaBaseVec.at(index);
        float& distenceFrom = alphaPiplineBase.Distenz;
        alphaPiplineBase.Distenz = distenz < distenceFrom ? distenz : distenceFrom;
    }

    

    int RenderTarget::DrawPilines()
    {
        int renderModeBevorDraw = Renderer::GetMode();
#if RY_TEST_SORT_EFICENTS
        uint64_t lastVAO = 0;
        int countVAOInRow = 0;
        int countVAOInRowMax = 0;
        for (Ref<PiplineRenderBase>& pipline : m_PilineBaseVec)
        {
            pipline->DrawNow();
            uint64_t curentVAO = pipline->GetTextureNumber();
            if (lastVAO != curentVAO)
            {
                lastVAO = curentVAO;
                countVAOInRowMax = countVAOInRowMax < countVAOInRow ? countVAOInRow : countVAOInRowMax;
                countVAOInRow = 0;
            }
            else
            {
                countVAOInRow++;
            }
        }
        RY_CORE_INFO("DrawFinished max is {}", countVAOInRowMax);
#else
        for (Ref<PiplineRenderBase>& pipline : m_PilineBaseVec)
        {
            pipline->DrawNow();
        }
#endif
        return renderModeBevorDraw;

    }

    int RenderTarget::DrawPilines(int modes)
    {
        int renderModeBevorDraw = Renderer::GetMode();
#if RY_TEST_SORT_EFICENTS
        uint64_t last = 0;
        int countInRow = 0;
        int countInRowMax = 0;
        for (Ref<PiplineRenderBase>& pipline : m_PilineBaseVec)
        {
            pipline->DrawNow();
            uint64_t curent = pipline->GetTextureNumber();
            if (last != curent)
            {
                last = curent;
                countInRowMax = countInRowMax < countInRow ? countInRow : countInRowMax;
                countInRow = 0;
            }
            else
            {
                countInRow++;
            }
        }
        RY_CORE_INFO("DrawFinished max is {}", countInRowMax);
#else
        for (Ref<PiplineRenderBase>& pipline : m_PilineBaseVec)
        {
            pipline->DrawNow(modes);
        }
#endif
        return renderModeBevorDraw;
    }

    int RenderTarget::DrawAlphaPilines()
    {
        int renderModeBevorDraw = Renderer::GetMode();
        for (AlphaPiplineBase& e : m_PilineAlphaBaseVec)
        {
            Ref<PiplineRenderBase>& pipline = e.Pipline;
            pipline->DrawNow();
        }
        return renderModeBevorDraw;
    }

    int RenderTarget::DrawAlphaPilines(int mode)
    {
        int renderModeBevorDraw = Renderer::GetMode();
        for (AlphaPiplineBase& e : m_PilineAlphaBaseVec)
        {
            Ref<PiplineRenderBase>& pipline = e.Pipline;
            pipline->DrawNow(mode);
        }
        return renderModeBevorDraw;
    }

    void RenderTarget::ClearPilines()
    {
        m_PilineBaseVec.clear();
    }

    void RenderTarget::ClearAlphaPilines()
    {
        m_PilineAlphaBaseVec.clear();
    }

    void RenderTarget::ClearFramebufferImges()
    {
        uint32_t i = 0;
        for (std::function<void(const Ref<Framebuffer>&, uint32_t)>& func : m_ClearColorFuncVec)
        {
            func(m_FB, i);
            i++;
        }
    }

    void RenderTarget::ClearFramebufferDepth()
    {
#if 0
        RenderCommand::ClearDepth();
#else
        if(nullptr != m_FB->GetDepthTexture() )
            m_FB->ClearDeathAttachment();
#endif
    }

    bool RenderTarget::SortRenderList(ShaderDrawResourceWeak_Ref aWeak, ShaderDrawResourceWeak_Ref bWeak)
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR

        Ref<ShaderDrawList> aRef = aWeak.lock();
        Ref<ShaderDrawList> bRef = bWeak.lock();
#else
        ShaderDrawResource* aRef = &aWeak;
        ShaderDrawResource* bRef = &bWeak;
#endif
        if (SortRenderListNotEqual(aRef, bRef))
            return false;

        glm::u64vec2 changesTex = RenderTarget::SortRenderListChangeBindBointCount(aRef->GetBindeTextures(), bRef->GetBindeTextures());
        glm::u64vec2 changesUni = RenderTarget::SortRenderListChangeBindBointCount<Ref<UniformBuffer>, g_UniformBindArrayCount>(aRef->GetBindeUniform(), bRef->GetBindeUniform());
        glm::u64vec2 changesSSBO = RenderTarget::SortRenderListChangeBindBointCount(aRef->GetBindeStorage(), bRef->GetBindeStorage());
        glm::u64vec2 changes = (changesTex * 60ull) + (changesUni * 10ull) + (changesSSBO * 30ull);

        int aRanderMode = aRef->renderMode;
        int bRanderMode = bRef->renderMode;

        if (aRanderMode != bRanderMode)
            changes.x++;
        else
            changes.y++;

        return changes.x > changes.y;
    }

    bool RenderTarget::SortRenderListNotEqualShader(const Ref<Shader>& aShader, const Ref<Shader>& bShader)
    {
        return aShader.get() != bShader.get();
    }

    bool RenderTarget::SortRenderListNotEqual(Const_ShaderDrawResource_RefPtr aRef, Const_ShaderDrawResource_RefPtr bRef)
    {

        const Ref<Shader>& aShader = aRef->shaderProgramm;
        const Ref<Shader>& bShader = bRef->shaderProgramm;

        if (aShader.get() != bShader.get())
            return true;

        const Ref<VertexArray>& aVertexArray = aRef->vao;
        const Ref<VertexArray>& bVertexArray = bRef->vao;

        if (aVertexArray.get() != bVertexArray.get())
            return true;

        const Ref<IndirectBuffer>& aDrawBuffer = aRef->drawBuffer;
        const Ref<IndirectBuffer>& bDrawBuffer = bRef->drawBuffer;

        if (aDrawBuffer.get() != bDrawBuffer.get())
            return true;


        return (aShader.get() != bShader.get())
            && (aVertexArray.get() != bVertexArray.get())
            && (aDrawBuffer.get() != bDrawBuffer.get());
    }

   

    void RenderTarget::CheckExexuteDrawList(uint32_t drawListCount)
    {
#ifdef RY_SHADER_DRAW_LIST_SHEARD_PTR
        uint32_t drawListSize = m_ShadeDrawList.Size();
        if (drawListCount != drawListSize)
            m_ShadeDrawList.Clear();
#else
        uint32_t drawListSize = m_ShadeDrawList.size();
        if (drawListCount != drawListSize)
            m_ShadeDrawList.clear();
#endif
    }

    

    bool RenderTarget::IsDataTypeValidToAtachment(const FramebufferTextureSpecification& frameTexSpec, const glm::vec4& value)
    {
        constexpr TexFrom vaildTexFormatsArray[] = { 
            TexFrom::R8
            ,  TexFrom::RG8

            ,  TexFrom::RGB8
            ,  TexFrom::S_RGB8
            ,  TexFrom::RGB16F
            ,  TexFrom::RGB32F

            ,  TexFrom::RGBA8
            ,  TexFrom::S_RGBA8
            ,  TexFrom::RGBA16F
            ,  TexFrom::RGBA32F
        };
        const TexFrom& fromate = frameTexSpec.TextureFormat;
        for (TexFrom texform : vaildTexFormatsArray)
        {
            if (texform == fromate)
                return true;
        }
        return false;
    }

    bool RenderTarget::IsDataTypeValidToAtachment(const FramebufferTextureSpecification& frameTexSpec, const int& value)
    {
        constexpr TexFrom vaildTexFormatsArray[] = { TexFrom::RED_INTEGER };
        const TexFrom& fromate = frameTexSpec.TextureFormat;
        for (TexFrom texform : vaildTexFormatsArray)
        {
            if (texform == fromate)
                return true;
        }
        return false;
    }

}
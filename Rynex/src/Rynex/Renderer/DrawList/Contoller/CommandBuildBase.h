#pragma once
// CommandBuildBase created in Project Rynex-Rendering on 01/11/2026.
#include <rypch.h>
#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>


namespace Rynex {
    template<typename Interface, bool VertexBuffer = false, bool BindBuffers = true>
    class CommandBuildBase 
    {
    public:
        static constexpr bool s_HasVertexBuffer = VertexBuffer;
        static constexpr bool s_HasBindBuffers = BindBuffers;
    // public member methods --------------------------------------------------------------------------------------------------
        CommandBuildBase() = delete;
        CommandBuildBase(const CommandBuildBase&) = delete;
        explicit CommandBuildBase(CachedBufferRegister& cache, Interface& interfaceType, ShaderDrawResource& drawResource)
            : m_Interface(interfaceType)
            , m_Cache(cache)
            , m_DrawResource(drawResource)
        {}
        
        virtual ~CommandBuildBase()
        {
        }

        virtual void CollectVertexBuffer(Span<VertexArray::VertexElements>& vertexElementsSpan) = 0;
        virtual void BindCommand() = 0;
    protected:
        Interface& m_Interface;
        CachedBufferRegister& m_Cache;
        ShaderDrawResource& m_DrawResource;
    };


}

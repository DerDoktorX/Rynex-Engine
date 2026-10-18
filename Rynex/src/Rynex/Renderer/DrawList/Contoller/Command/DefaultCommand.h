#pragma once
// DefaultCommand created in Project Rynex-Rendering on 16/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>


namespace Rynex {

    class DefaultCommand
    {
    public:
        using DrawObject = DrawElementsIndirectCommand;
    // public constexpr member ------------------------------------------------------------------------------------------------
        static constexpr uint64_t s_AttributeLocationMask = 0ull;
        static constexpr uint64_t s_VertexBufferMask = 0ull;
        static constexpr uint64_t s_UniformSlotMask = 0ull;
        static constexpr uint64_t s_StorageSlotMask = 0ull;
        static constexpr uint64_t s_TextureSlotMask = 0ull;
        static constexpr uint32_t s_InstanceMultiplier = 0ul;
        static constexpr bool s_AllowsMultipleCommands = false;
    // public member methode --------------------------------------------------------------------------------------------------
        DefaultCommand() { }
        ~DefaultCommand() { }


        DrawSplit Set() { }

        void OpenSection(const DrawObject& first) { };
        void BeginCommand(const DrawObject& next, uint32_t instanceIndexInChunk) {}
        uint32_t GetCapacity() { return std::numeric_limits<uint32_t>::max(); }
        void Clear() { }

        template<typename Output, typename VertexElementArray, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, VertexElementArray& vertexElementArray, BufferRegister& bufferRegister) { }

        template<typename Output, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, BufferRegister& bufferRegister) { }
    };


}


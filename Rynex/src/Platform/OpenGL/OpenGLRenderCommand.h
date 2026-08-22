#pragma once
#include <Platform/OpenGL/OpenGLRendererAPI.h>

namespace Rynex {
#define RY_OPENGL_BIND_RENDER_COMAND 1
	class OpenGLRenderCommand
	{
	public:
		inline static void Init(OpenGLRendererAPI* renderAPIPtr)
		{
			s_RendererAPI = renderAPIPtr;
		}

		inline static void ShutDown()
		{
			s_RendererAPI = nullptr;
		}

		inline static void BindShader(uint32_t renderID)
		{
			s_RendererAPI->BindShader(renderID);
		}
		inline static void BindFramebuffer(uint32_t renderID)
		{
			s_RendererAPI->BindFrambuffer(renderID);
		}
		inline static void BindVertexArray(uint32_t renderID)
		{
			s_RendererAPI->BindVertexArray(renderID);
		}
		inline static void BindBuffer(uint32_t target, uint32_t renderID)
		{
			s_RendererAPI->BindBuffer(target, renderID);
		}

		inline static void BindBufferSlot(uint32_t target, uint32_t slot, uint32_t renderID)
		{
			s_RendererAPI->BindBufferSlot(target, slot, renderID);
		}

		inline static void BindIndexBuffer(uint32_t renderID)
		{
			s_RendererAPI->BindIndexBuffer(renderID);
		}

		inline static void BindIndrectBuffer(uint32_t renderID)
		{
			s_RendererAPI->BindIndrectBuffer(renderID);
		}

		inline static void BindTextureSlot(uint32_t slot, uint32_t renderID)
		{
			s_RendererAPI->BindTextureSlot(slot, renderID);
		}

		inline static void BindSamplerSlot(uint32_t slot, uint32_t renderID)
		{
			s_RendererAPI->BindSamplerSlot(slot, renderID);
		}

		inline static void BindVertexBufferSlot(uint32_t slot, uint32_t renderID)
		{
			s_RendererAPI->BindVertexBufferSlot(slot, renderID);
		}

		inline static void BindUniformBufferSlot(uint32_t slot, uint32_t renderID)
		{
			s_RendererAPI->BindUniformBufferSlot(slot, renderID);
		}

		inline static void BindStorageBufferSlot(uint32_t slot, uint32_t renderID)
		{
			s_RendererAPI->BindStorageBufferSlot(slot, renderID);
		}

		inline static uint32_t GetDefaultFrambufferRenderID()
		{
			return s_RendererAPI->GetDefaultFrambufferRenderID();
		}
	private:
		static OpenGLRendererAPI* s_RendererAPI;
	};

}
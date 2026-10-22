#pragma once
#include <Rynex/Renderer/API/GraphicsContext.h>

struct GLFWwindow;

namespace Rynex{
	
	

	class OpenGLContext : public GraphicsContext
	{
	public:
        explicit OpenGLContext(GLFWwindow* windowHandle);
		virtual ~OpenGLContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };

		std::mutex& GetFramebufferOperationLock();
		std::mutex& GetBufferOperationLock();
		std::mutex& GetVertexArrayOperationLock();

		std::mutex& GetTextureOperationLock();
		std::mutex& GetShaderOperationLock();
		std::mutex& GetFancyOperationLock();
	    bool IsMainContextThread() const;
		void AddCurrentAllocMemory(uint64_t add);

		void RemoveCurrentAllocMemory(const uint64_t remove) { m_CurrentAllocMemory -= remove; m_DifferentBuffer--; }

		virtual uint64_t GetCurentAllocMemory() const override { return m_CurrentAllocMemory; }
		virtual int64_t GetCurentAllocBuffers() const override { return m_DifferentBuffer; }
		virtual uint64_t GetMaxGPUMemory() const override { return m_DifferentBuffer; }
		virtual uint64_t GetMemoryUsage() const override;
	    static OpenGLContext& Get();
	private:
		GLFWwindow* m_WindowHandle;
	    std::mutex m_BufferOperationIDLock;
	    std::mutex m_TextureOperationIDLock;
	    std::mutex m_ShaderOperationIDLock;
	    std::mutex m_FramebufferOperationIDLock;
	    std::mutex m_FancyOperationIDLock;
	    std::mutex m_VertexArrayOperationIDLock;
		uint64_t m_MaxGPUMemory;
		uint64_t m_MaxVideoMemory;
		uint64_t m_CurrentAllocMemory;
		int64_t m_DifferentBuffer;
        std::thread::id m_MainContextThread;
	    bool m_ActiveMultyThreading;

	    static OpenGLContext* s_Instance;
	};

}

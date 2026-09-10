#pragma once
#include <Rynex/Renderer/API/GraphicsContext.h>

struct GLFWwindow;

namespace Rynex{
	
	

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		~OpenGLContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };

		static std::mutex& GetFrambufferOperationLock();
		static std::mutex& GetBufferOperationLock();
		static std::mutex& GetVertexArrayOperationLock();

		static std::mutex& GetTextureOperationLock();
		static std::mutex& GetShaderOperationLock();
		static std::mutex& GetFanceOperationLock();

		void AddCurentAllocMemory(uint64_t add) 
		{ 
			m_CurentAllocMemory += add; 
			m_DifferentBuffer++; 

			if (FRACTURE_ONE(m_CurentAllocMemory,4) >= m_MaxGPUMemory)
			{
				RY_CORE_WARN("Alloc Memory is Cirticle Close to Max {} KB / {} KB", m_CurentAllocMemory, m_MaxGPUMemory);
			}
		}
		void RemoveCurentAllocMemory(uint64_t remove) { m_CurentAllocMemory -= remove; m_DifferentBuffer--; }
		virtual uint64_t GetCurentAllocMemory() const override { return m_CurentAllocMemory; }
		virtual int64_t GetCurentAllocBuffers() const override { return m_DifferentBuffer; }
		virtual uint64_t GetMaxGPUMemory() const override { return m_DifferentBuffer; }
		virtual uint64_t GetMemoryUsage() const override;
	private:
		GLFWwindow* m_WindowHandle = nullptr;
		uint64_t m_MaxGPUMemory;
		uint64_t m_MaxVidoeMemory;
		uint64_t m_CurentAllocMemory;
		int64_t m_DifferentBuffer;

		
	};

}

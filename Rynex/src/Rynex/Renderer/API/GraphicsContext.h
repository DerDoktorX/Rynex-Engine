#pragma once
#include "Rynex/Asset/Base/Asset.h"

#include "Rynex/Renderer/RendererAPI.h"

namespace Rynex {

	class RYNEX_API GraphicsContext
	{
	public:
		virtual void Init() = 0;

		virtual void SwapBuffers() = 0;

		virtual const RendererAPI::API GetRendererAPI() const = 0;

		virtual uint64_t GetCurentAllocMemory() const = 0;
		virtual int64_t  GetCurentAllocBuffers() const = 0;
		virtual uint64_t GetMaxGPUMemory() const = 0;
		virtual uint64_t GetMemoryUsage() const = 0;
	};

	


}
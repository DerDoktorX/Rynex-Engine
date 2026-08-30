#include "rypch.h"
#include "BatchedSimplePiplineRender.h"
#include <Rynex/Renderer/API/Buffer.h>

namespace Rynex {
	namespace BatchedRender {
		
		void SetBufferData(Ref<VertexBuffer>& buffer, const void *const dataPtr, uint32_t byteSize)
		{
			buffer->SetData(dataPtr, byteSize);
		}

		void SetBufferData(Ref<IndexBuffer>& buffer, const uint32_t *const dataPtr, uint32_t byteSize)
		{
			uint32_t modolu = byteSize % sizeof(uint32_t);
			RY_CORE_ASSERT(modolu == 0, "Index-Buffer the byte size has a not expexted size!");
			uint32_t count = byteSize / sizeof(uint32_t);
			buffer->SetData(dataPtr, byteSize);
		}

		void SetBufferData(Ref<IndexBuffer>& buffer, const uint16_t *const dataPtr, uint32_t byteSize)
		{
			constexpr uint32_t size = sizeof(uint16_t);;
			uint32_t modolu = byteSize % size;
			RY_CORE_ASSERT(modolu == 0, "Index-Buffer the byte size has a not expexted size!");
			uint32_t count = byteSize / size;
			buffer->SetData(dataPtr, count);
		}
		
		void SetBufferData(Ref<UniformBuffer>& buffer, const void *const dataPtr, uint32_t byteSize)
		{
			buffer->SetData(dataPtr, byteSize);
		}

		void SetBufferData(Ref<IndirectBuffer>& buffer, const void *const dataPtr, uint32_t byteSize)
		{
			buffer->SetData(dataPtr, byteSize);
		}

		void SetBufferData(Ref<StorageBuffer>& buffer, const void *const dataPtr, uint32_t byteSize)
		{
			buffer->SetData(dataPtr, byteSize);
		}



	

		void ResizeBufferData(Ref<VertexBuffer>& buffer, const void *const dataPtr, uint32_t byteSize)
		{
			buffer->SetData(dataPtr, byteSize);
		}

		void ResizeBufferData(Ref<IndirectBuffer>& buffer, const void *const dataPtr, uint32_t byteSize)
		{
			buffer->ResizeBuffer(dataPtr, byteSize);
		}

		void ResizeBufferData(Ref<StorageBuffer>& buffer, const void *const dataPtr, uint32_t byteSize)
		{
			buffer->ResizeBuffer(dataPtr, byteSize);
		}

	}
}
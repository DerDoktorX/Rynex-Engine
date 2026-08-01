#pragma once
#include <Rynex/Renderer/API/Buffer.h>
#define RY_NEW_BUFFER_ORG 0


namespace Rynex {
#if RY_NEW_BUFFER_ORG

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer();
		~OpenGLIndexBuffer();


		virtual uint32_t GetCount() const = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void InitAsync() = 0;
		virtual void SetData(const uint32_t* indices, uint32_t count) = 0;
		virtual void SetData(const uint16_t* indices, uint32_t count) = 0;

		virtual void CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<IndexBuffer> ib) = 0;
		virtual void AddCopyData(Ref<IndexBuffer> ib) = 0;

		virtual const std::vector<uint32_t>& GetBufferData() = 0;
		virtual void FreeBufferData() = 0;
		virtual uint32_t GetElementByte() const = 0;
		virtual uint32_t GetByteSize() const = 0;
		virtual uint32_t GetRenderID() const = 0;

	private:

	};
#endif
}



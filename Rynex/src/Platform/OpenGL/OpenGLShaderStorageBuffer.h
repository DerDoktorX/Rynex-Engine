#pragma once
#include <Platform/OpenGL/OpenGLFence.h>

namespace Rynex {
	class OpenGLShaderStorageBuffer
	{
	public:
		OpenGLShaderStorageBuffer(uint32_t target, std::nullptr_t, uint32_t byteSize, uint32_t flags);
		OpenGLShaderStorageBuffer(uint32_t target, const uint8_t* dataPtr, uint32_t byteSize, uint32_t flags);
		OpenGLShaderStorageBuffer(uint32_t target, std::vector<uint8_t>&& data, uint32_t flags);
		OpenGLShaderStorageBuffer(uint32_t target, const OpenGLShaderStorageBuffer& buffer);
		OpenGLShaderStorageBuffer(const OpenGLShaderStorageBuffer&) = delete;

		~OpenGLShaderStorageBuffer();

		void Bind(uint32_t target) const;
		void UnBind(uint32_t target) const;

		void BindSlot(uint32_t target, uint32_t slot) const;
		void UnBindSlot(uint32_t target, uint32_t slot) const;

		void SetData(uint32_t target, const uint8_t* dataPtr, uint32_t offset, uint32_t byteSize);
		void ResizeData(uint32_t target, uint32_t byteSize);
		void ResizeData(uint32_t target, const uint8_t* dataPtr, uint32_t byteSize, uint32_t flags);

		uint32_t GetByteSize() const { return m_ByteSize; }
		uint32_t GetRenderID() const { return m_RendererID; }
		uint32_t GetFlags() const { return m_Flags; }

		bool IsTransfered();
		void SetupFance();

		std::vector<uint8_t>& GetData() { return m_Data; }
		const std::vector<uint8_t>& GetData() const { return m_Data; }

		void LoadeGPUDataOnCPU();
		void ClearLocaleDataStore();
	private:
		void CreateID(uint32_t target);
		void DestroyID();
		void InvalideData(uint32_t target);

		void AddCopyMemoryData(const uint8_t* dataPtr, uint32_t sibyteSizeze);
		void CopyMemoryData(const uint8_t* dataPtr, uint32_t sibyteSizeze);
		void CopyOffsetMemoryData(const uint8_t* dataPtr, uint32_t offset, uint32_t sizbyteSizee);
	private:
		std::vector<uint8_t> m_Data;
		OpenGLFence m_FanceObject;

		uint32_t m_RendererID;
		uint32_t m_ByteSize;
		uint32_t m_Flags;
	};
}
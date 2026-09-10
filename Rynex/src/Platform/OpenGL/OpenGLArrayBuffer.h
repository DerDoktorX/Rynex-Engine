#pragma once
#include <Platform/OpenGL/OpenGLFence.h>
#define RY_USE_OPENGL_NAMED_BUFFER


namespace Rynex {


	class OpenGLArrayBuffer
	{
	public:
		template<typename T>
		OpenGLArrayBuffer(uint32_t target, std::vector<T>&& data, uint32_t usage)
			: m_Usage(usage)
			, m_RendererID(0u)
		{
			std::vector<T> dataCopy = std::move(data);
			ConvertVector<T, uint8_t>(dataCopy, m_Data);
			InvalideData(target);
		};

		OpenGLArrayBuffer(uint32_t target, std::vector<uint8_t>&& data, uint32_t usage);
		OpenGLArrayBuffer(uint32_t target, const uint8_t* data, uint32_t size, uint32_t usage);
		OpenGLArrayBuffer(uint32_t target, std::nullptr_t, uint32_t size, uint32_t usage);

		OpenGLArrayBuffer(uint32_t target, const OpenGLArrayBuffer& buffer);

		OpenGLArrayBuffer(const OpenGLArrayBuffer&) = delete;
		~OpenGLArrayBuffer();


		void Bind(uint32_t target) const;
		void UnBind(uint32_t target) const;

		void BindSlot(uint32_t target, uint32_t slot) const;
		void UnBindSlot(uint32_t target, uint32_t slot) const;

		void SetData(uint32_t target, const uint8_t* data, uint32_t size);
		void SetData(uint32_t target, const uint8_t* data, uint32_t offset, uint32_t size);
		void ResizeData(uint32_t target, const uint8_t* data, uint32_t size, uint32_t usage);

		template<typename T>
		void ResizeData(uint32_t target, std::vector<T>&& data, uint32_t size, uint32_t usage)
		{
			m_Usage = usage;
			std::vector<T> dataCopy = std::move(data);
			OpenGLArrayBuffer::ConvertVector<T, uint8_t>(dataCopy, m_Data);
			InvalideData(target);
		};

		void LoadeDataGPU();


		uint32_t GetByteSize() const { return m_ByteSize; }
		uint32_t GetRenderID() const
		{
			return m_RendererID;
		}
		uint32_t GetUsage() const { return m_Usage; }

		std::vector<uint8_t>& GetData() { return m_Data; }
		const std::vector<uint8_t>& GetData() const { return m_Data; }

		template<typename T>
		std::vector<T> GetCopyData() const { return OpenGLArrayBuffer::CopyVector<T, uint8_t>(m_Data); }
		bool IsTransfered();
		void SetupFance();


		const uint8_t* GetDataPtr() const { return m_Data.data(); }
		uint8_t* GetDataPtr() { return m_Data.data(); }

		void LoadeGPUDataOnCPU();
		void ClearLocaleDataStore();
	private:
		void CreateID(uint32_t target);
		void DestroyID();
		void InvalideData(uint32_t target);


		void AddCopyMemoryData(const uint8_t* data, uint32_t size);
		void CopyMemoryData(const uint8_t* data, uint32_t size);
		void CopyOffsetMemoryData(const uint8_t* data, uint32_t offset, uint32_t size);
		


		template<typename T, typename N>
		static void ConvertVector(std::vector<T>& from, std::vector<N>& to)
		{
			uint32_t fromSize = from.size();
			uint32_t toSize = to.size();

			uint32_t fromByteSize = fromSize * sizeof(T);
			uint32_t toByteSize = toSize * sizeof(N);
			RY_CORE_ASSERT(fromByteSize == toByteSize);
			std::memcpy(to.data(), from.data(), fromByteSize);
		}

		template<typename T, typename N>
		static std::vector<T> CopyVector(std::vector<N>& data)
		{

			uint32_t sizeByteData = data.size() * sizeof(N);
			uint32_t elementSizeCopy = sizeByteData / sizeof(T);
			uint32_t byteSizeCopy = elementSizeCopy * sizeof(T);
			RY_CORE_ASSERT(sizeByteData == byteSizeCopy);

			std::vector<T> copy;
			copy.resize(elementSizeCopy);
			ConvertVector<T, N>(data, copy);
			return copy;
		}
	private:
		static constexpr const uint32_t s_OpenGLDefaultBuffer = 0u;
	private:
		OpenGLFence m_FanceObject;
		std::vector<uint8_t> m_Data;

		uint32_t m_RendererID;
		uint32_t m_ByteSize;
		uint32_t m_Usage;
	};	

}
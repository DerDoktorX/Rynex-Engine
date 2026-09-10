#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <magic_enum/magic_enum.hpp>
// #define RY_HOLD_LAYOUT_ELEMENT_INCREAS_INSTANCE
// #define RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE


namespace Rynex {
	
	enum class DataType : uint8_t
	{

		None = 0,
		Default = 1,
		Btye,							// Single signed 8-bit integer per component. (For GL_RED: [R0, R1, R2, ...])
		Short,							// Single signed 16-bit integer per component. (For GL_RED: [R0(16b), R1(16b), ...])
		
		Int,							// Single signed 32-bit integer per component. (For GL_RED: [R0(32b), R1(32b), ...])
		HalfFloat,						// Single 16-bit half-precision float per component. (For GL_RGB: [R0(16f), G0(16f), B0(16f), R1(16f), ...])
		Float,							// Single 32-bit single-precision float per component. (For GL_RGBA: [R0(32f), G0(32f), B0(32f), A0(32f), ...])
		UnsingnedBtye,					// Single unsigned 8-bit integer per component. (For GL_RGBA: [R0, G0, B0, A0, R1, G1, ...])
		UnsingnedBtye_3_3_2,			// 8 bits total: 3 bits Red, 3 bits Green, 2 bits Blue.  (For GL_RGB: [Byte: 0bRRRGGGBB, ...])
		UnsingnedBtye_2_3_3_Rev,		// 8 bits total: Reversed order of 3_3_2.  (For GL_RGB: [Byte: 0bBBGGGRRR, ...])

		UnsingnedShort,					// Single unsigned 16-bit integer per component. (For GL_RGB: [R0(16b), G0(16b), R1(16b), ...])
		UnsingnedShort_5_6_5,			// 16 bits total: 5 bits Red, 6 bits Green, 5 bits Blue. (For GL_RG: [Short: 0bRRRRRGGGGGGBBBBB, ...] or [{ Byte: 0bRRRRRGGG, Byte: 0bGGGBBBBB }, ... ])
		UnsingnedShort_5_6_5_Rev,		// 16 bits total: Reversed order of 5_6_5. (For GL_RGB: [Short: 0bBBBBBGGGGGGRRRRR, ...] or [{ Byte: 0bBBBBBGGG, Byte: 0bGGGRRRRR }, ... ])
		UnsingnedShort_4_4_4_4,			// 16 bits total: 4 bits each for Red, Green, Blue, Alpha. (For GL_RGBA: Short: 0bRRRRGGGGBBBBAAAA or [{ Byte: 0bRRRRGGGG, Byte: 0bBBBBAAAA}, ... ])
		UnsingnedShort_4_4_4_4_Rev,		// 16 bits total: Reversed order of 4_4_4_4. (For GL_RGBA: Short: 0bAAAABBBBGGGGRRRR or [{ Byte: 0bAAAABBBB, Byte: 0bGGGGRRRR }, ... ])
		UnsingnedShort_5_5_5_1,			// 16 bits total: 5 bits each for R,G,B, 1 bit for Alpha.  (For GL_RGBA: Short: 0bRRRRRGGGGGBBBBBA or [{ Byte: 0bRRRRRGGGG, Byte: 0bGBBBBBA }, ... ])
		UnsingnedShort_1_5_5_5_Rev,		// 16 bits total: Reversed order of 5_5_5_1 (1 bit Alpha first).. (For GL_RGBA: Short: 0bARRRRRGGGGGBBBBB or [{ Byte: 0bARRRRRGGG, Byte: 0bGGBBBBBA }, ... ])

		UnsingnedInt,					// Single signed 32-bit integer per component.. (For GL_RED: [R0(32b), R1(32b), ...])
		UnsingnedInt_8_8_8_8,			// 32 bits total: 8 bits each for Red, Green, Blue, Alpha. (Int: 0xRRGGBBAA (in memory: [RR, GG, BB, AA, ... ]))
		UnsingnedInt_8_8_8_8_Rev,		// 32 bits total: Reversed component order (often BGRA). (Int: 0xRRGGBBAA (in memory: [AA, BB, GG, RR, ... ]))
		UnsingnedInt_10_10_10_2,		// 32 bits total: 10 bits each for R,G,B, 2 bits for Alpha. (For GL_RGBA: [Int: 0bRRRRRRRRRRGGGGGGGGGGBBBBBBBBBBAA, ...] or [{Byte: 0bRRRRRRRR, Byte: 0bRRGGGGGG, Byte: GGGGBBBB, Byte: BBBBBBAA}, ... ])
		UnsingnedInt_2_10_10_10_Rev,	// 32 bits total: Reversed order (2 bit Alpha, then 10-bit B,G,R). (For GL_RGBA:[Int: 0bAABBBBBBBBBBGGGGGGGGGGRRRRRRRRRR] or [{Byte: 0b AABBBBBB, Byte: 0bBBBBGGGG, Byte: 0bGGGGGGRR, Byte: 0bRRRRRRRR}, ... ])
		
	};

	enum class ShaderDataType : uint8_t
	{
		None = 0, 
		Float, Float2, Float3, Float4, Float3x3, Float4x4,
		Int, Int2, Int3, Int4, Int3x3, Int4x4,
		Uint, Uint2, Uint3, Uint4, Uint3x3, Uint4x4,
		Texture,
		Texture2D,
		TextureCube,
		TextureArray,

	};
	

	enum class BufferDataUsage : uint8_t
	{
		None = 0,
		DynamicDraw,
		StaticDraw,
	};

	using SDT = ShaderDataType;
	using BDU = BufferDataUsage;

	enum class ShaderResourceType;


	enum class BufferType : uint8_t
	{
		None = 0,

		Uniform, Vertex, Index,
		ShaderStorage,
		DrawIndrirect
	};
	namespace BufferFlag {
		enum BufferFlagBit : uint8_t
		{
			None = 0, // None is like Static, like no changes Optimize flage
			Dynamic = BIT(0),
			Read = BIT(1),
			Write = BIT(2),
			Presistent = BIT(3),
			Coherent = BIT(4),
			Client = BIT(5)
		};
		inline static constexpr const size_t s_Count = 7;
	}
	using BufferFlagGPU = uint8_t;

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		uint32_t ellements = 1;
		switch (type)
		{
			case ShaderDataType::Float:			return sizeof(float);
			case ShaderDataType::Float2:		return sizeof(float) * 2u;
			case ShaderDataType::Float3:		return sizeof(float) * 3u;
			case ShaderDataType::Float4:		return sizeof(float) * 4u;
			case ShaderDataType::Float3x3:		return sizeof(float) * 3u * 3u;
			case ShaderDataType::Float4x4:		return sizeof(float) * 4u * 4u;
			case ShaderDataType::Int:			return sizeof(int);
			case ShaderDataType::Int2:			return sizeof(int) * 2u;
			case ShaderDataType::Int3:			return sizeof(int) * 3u;
			case ShaderDataType::Int4:			return sizeof(int) * 4u;
			case ShaderDataType::Int3x3:		return sizeof(int) * 3u * 3u;
			case ShaderDataType::Int4x4:		return sizeof(int) * 4u * 4u;
			case ShaderDataType::Uint:			return sizeof(uint32_t);
			case ShaderDataType::Uint2:			return sizeof(uint32_t) * 2u;
			case ShaderDataType::Uint3:			return sizeof(uint32_t) * 3u;
			case ShaderDataType::Uint4:			return sizeof(uint32_t) * 4u;
			case ShaderDataType::Uint3x3:		return sizeof(uint32_t) * 3u * 3u;
			case ShaderDataType::Uint4x4:		return sizeof(uint32_t) * 4u * 4u;
			default: 
				std::string_view nameType = magic_enum::enum_name(type);
				RY_CORE_ERROR("ShaderDataType {} not defined", nameType.data());
		}
		RY_CORE_ASSERT(false, "Uknokn ShaderDataType!");
		return MAXUINT32;
	};



	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		bool active;

		uint32_t offset;
		uint32_t size;
		bool normilized;
		uint32_t count;
		
		BufferElement()
			: name("Unkown now"), type(SDT::None), active(false)
			, offset(0u), size(0u), normilized(false), count(0u)
		{
		}

		BufferElement(const BufferElement&) = default;

		BufferElement(const BufferElement& buffer, uint32_t count)
			: name(buffer.name), type(buffer.type), active(buffer.active), size(buffer.size), offset(0u), normilized(buffer.normilized), count(count)
		{
		}

		BufferElement(ShaderDataType type, const std::string& name, bool active = true, uint32_t count = 1u, bool normilized = false)
			: name(name), type(type), active(active), size(ShaderDataTypeSize(type)), offset(0u), normilized(normilized), count(count)
		{
		}

		uint64_t GetHash() const
		{
			uint64_t hashType = static_cast<uint64_t>(this->type);
			uint64_t hashActive = this->active ? 1ull : 0ull;

			uint64_t hashOffset = static_cast<uint64_t>(this->offset);

			uint64_t hashSize = static_cast<uint64_t>(this->size);
			uint64_t hashNormilized = this->normilized ? 1ull : 0ull;
			uint64_t hashCount = static_cast<uint64_t>(this->count);

			uint64_t hash = 0;
			hash += 1 * hashType;
			hash += 2 * hashActive;
			hash += 3 * hashOffset;

			hash += 4 * hashSize;
			hash += 5 * hashNormilized;
			hash += 6 * hashCount;

			return hash;
		}
		
		
		uint32_t GetCompontsCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:			return 1;
			case ShaderDataType::Float2:		return 2;
			case ShaderDataType::Float3:		return 3;
			case ShaderDataType::Float4:		return 4;
			case ShaderDataType::Float3x3:		return 3 * 3;
			case ShaderDataType::Float4x4:		return 4 * 4;
			case ShaderDataType::Int:			return 1;
			case ShaderDataType::Int2:			return 2;
			case ShaderDataType::Int3:			return 3;
			case ShaderDataType::Int4:			return 4;
			case ShaderDataType::Int3x3:		return 3 * 3;
			case ShaderDataType::Int4x4:		return 4 * 4;
			case ShaderDataType::Uint:			return 4;
			case ShaderDataType::Uint2:			return 4;
			case ShaderDataType::Uint3:			return 4;
			case ShaderDataType::Uint4:			return 4;
			case ShaderDataType::Uint3x3:		return 4 * 3;
			case ShaderDataType::Uint4x4:		return 4 * 4;
			}
			return 0;
		}

		bool operator==(const BufferElement& elemnet) const
		{
			RY_CORE_ASSERT(this->count == elemnet.count, "Decied If we can left that!");
			bool result =(this->name ==  elemnet.name)
				&& ( this->type == elemnet.type) 
				&& ( this->size == elemnet.size) 
				&& ( this->count == elemnet.count) 
				&& ( this->active ==  elemnet.active);
			RY_CORE_ASSERT(!result || this->GetHash() == elemnet.GetHash());

			return result;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout()
			: m_Elements()
			, m_HashNumber(0ull)
			, m_Length(0ull)
			, m_BufferCount(0ull)
			, m_Stride(0ull)
			, m_InstanceIncreas(0u)
			, m_EnabelAutoCompress(false)
		{

		}
		
		BufferLayout(const BufferLayout&) = default;

		BufferLayout(const std::initializer_list<BufferElement>& element, uint32_t instanceIncreas = 0u)
			: m_Elements(element)
			, m_HashNumber(0ull)
			, m_Length(0ull)
			, m_BufferCount(0ull)
			, m_Stride(0ull)
			, m_InstanceIncreas(instanceIncreas)
			, m_EnabelAutoCompress(false)
		{
			CaculateOffsetAndStride();
		}

		BufferLayout(const std::vector<BufferElement>& element, uint32_t instanceIncreas = 0u)
			: m_Elements(element)
			, m_HashNumber(0ull)
			, m_Length(0ull)
			, m_BufferCount(0ull)
			, m_Stride(0ull)
			, m_InstanceIncreas(instanceIncreas)
			, m_EnabelAutoCompress(false)
		{
			CaculateOffsetAndStride();
		}

		inline size_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

		size_t GetLength() const { return m_Length; };
		size_t GetBufferCount() const { return m_BufferCount; };
		bool Empty() const { return m_Elements.empty(); }
		bool AutoCompress() const { return m_EnabelAutoCompress; }
		void SetAutoCompress(bool autoCompres) {m_EnabelAutoCompress = autoCompres;}
		uint32_t GetInstastancIncreas() const { return m_InstanceIncreas; }
		bool operator==(const BufferElement& elemnet) const
		{
			for(const BufferElement& elements : m_Elements)
			{
				if (elements == elemnet)
					return true;
			}
			return false;
		}

		bool operator==(const BufferLayout& layouteRigth) const
		{
			bool result = true;

			result = m_HashNumber == layouteRigth.GetHash();
			return result;
		}

		
		bool operator!=(const BufferLayout& layouteRigth) const
		{
			bool result = m_HashNumber != layouteRigth.GetHash();
			return result;
		}

		void AddBufferElement(BufferElement& ellements)
		{
			m_Elements.push_back(ellements);
			CaculateOffsetAndStride();
		}


		BufferElement& operator[](size_t elementIndex)
		{
			return m_Elements[elementIndex];
		}

		const BufferElement& operator[](size_t elementIndex) const
		{
			return m_Elements[elementIndex];
		}
		void Clear()
		{
			m_Elements.clear();
			m_Stride = 0ull;
			m_Length = 0ull;
			m_BufferCount = 0ull;
		}

		uint64_t GetHash() const
		{
			return m_HashNumber;
		}
	private:
		void CaculateOffsetAndStride() 
		{
			uint32_t offset = 0u;
			m_Stride = 0ull;
			m_Length = 0ull;
			m_BufferCount = 0ull;
			m_HashNumber = 0ull;
			uint64_t i = 1u;

			for (BufferElement& elements : m_Elements)
			{
				elements.offset = offset;
				m_HashNumber += i * elements.GetHash();
				offset += elements.size;
				m_Stride += elements.size;
				m_Length++;
				m_BufferCount += elements.GetCompontsCount();
				i++;
			}
		}
		
	private:
		std::vector<BufferElement> m_Elements;
		uint64_t m_HashNumber = 0ull;
		size_t m_Length = 0ull;
		size_t m_BufferCount = 0ull;
		size_t m_Stride = 0ull;
		uint32_t m_InstanceIncreas = 0u;
		bool m_EnabelAutoCompress = false;
	};


	class RYNEX_API VertexBuffer : public Asset
	{
	public:
		virtual ~VertexBuffer() {}

		virtual bool IsTransferd() = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout & layout) = 0;

		virtual void SetData(const void* data, uint32_t byteSize) = 0;
		virtual void ResizeBuffer(const void* data, uint32_t byteSize) = 0;

		virtual void CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<VertexBuffer> vb) = 0;
		virtual void AddCopyData(Ref<VertexBuffer> vb) = 0;
		virtual uint32_t GetByteSize() const = 0;
		virtual uint32_t GetRenderID() const = 0;

		static Ref<VertexBuffer> Create(const uint32_t size);
		static Ref<VertexBuffer> Create(const void* vertices, uint32_t size);

		static Ref<VertexBuffer> Create(const void* vertices, uint32_t size, BufferFlagGPU flag);
		static Ref<VertexBuffer> Create(const void* vertices, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout);
		static Ref<VertexBuffer> CreateAsync(std::vector<uint8_t>&& data, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout);
		static Ref<VertexBuffer> Default();

		virtual const std::vector<uint8_t>& GetBufferData() const = 0;
		virtual void FreeBufferData() = 0;
		virtual uint32_t GetVertexCount() const = 0;

		static AssetType GetStaticType() { return AssetType::VertexBuffer; }
		AssetType GetType() const override { return GetStaticType(); }
	};

	class RYNEX_API IndexBuffer : public Asset
	{
	public:
		virtual ~IndexBuffer() {}

		virtual uint32_t GetCount() const = 0;

		virtual bool IsTransferd() = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		
		virtual void SetData(const uint32_t* indices, uint32_t count) = 0;
		virtual void SetData(const uint16_t* indices, uint32_t count) = 0;

		virtual void CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<IndexBuffer> ib) = 0;
		virtual void AddCopyData(Ref<IndexBuffer> ib) = 0;

		virtual const std::vector<uint8_t>& GetBufferData() const = 0;
		virtual void FreeBufferData() = 0;
		virtual uint32_t GetElementByte() const = 0;
		virtual uint32_t GetByteSize() const = 0;
		virtual uint32_t GetRenderID() const = 0;

		static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count, BufferFlagGPU flag = BufferFlag::None);
		static Ref<IndexBuffer> Create(const uint16_t* indices, uint32_t count, BufferFlagGPU flag = BufferFlag::None);
		static Ref<IndexBuffer> CreateAsync(std::vector<uint32_t>&& data, uint32_t size, BufferFlagGPU flag = BufferFlag::None);

		static AssetType GetStaticType() { return AssetType::IndexBuffer; }
		AssetType GetType() const override { return GetStaticType(); }

	
		
	};

	class RYNEX_API StorageBuffer : public Asset
	{		
	public:
		static Ref<StorageBuffer> Create(const void* data, uint32_t byteSize, BufferType bufferType, BufferFlagGPU flag);
		static Ref<StorageBuffer> Create(uint32_t byteSize, BufferFlagGPU flag);
		static Ref<StorageBuffer> Create(const void* data, uint32_t byteSize, BufferFlagGPU flag);


		virtual ~StorageBuffer() {}

		virtual bool IsTransferd() = 0;
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void UnBind(uint32_t slot) = 0;


		virtual void SetData(const void* data, uint32_t byteSize) = 0;
		virtual void SetData(const void* data, uint32_t offset, uint32_t byteSize) = 0;

		virtual void ResizeBuffer(uint32_t byteSize) = 0;
		virtual void ResizeBuffer(const void* data, uint32_t byteSize) = 0;
		
		virtual uint32_t GetByteSize() const = 0;
		static AssetType GetStaticType() { return AssetType::StorageBuffer; }


		AssetType GetType() const override { return GetStaticType(); }
	};

	struct DrawElementsIndirectCommand
	{
		uint32_t indexCount;
		uint32_t instancesCount;
		uint32_t firstIndex;
		int baseVertex;
		uint32_t baseInstance;

		void Default()
		{
			indexCount = 0u;
			instancesCount = 0u;
			firstIndex = 0u;
			baseVertex = -1;
			baseInstance = 0u;
		}
	};

	class RYNEX_API IndirectBuffer : public Asset
	{
	public:
		static Ref<IndirectBuffer> Create(const void* data, uint32_t byteSize);
		static Ref<IndirectBuffer> Create(const DrawElementsIndirectCommand* data, uint32_t count);
		static Ref<IndirectBuffer> Create(uint32_t byteSize);
		static Ref<IndirectBuffer> Create(const void* data, uint32_t byteSize, const BufferLayout& layout);
		static Ref<IndirectBuffer> Create(uint32_t byteSize, const BufferLayout& layout);

		virtual ~IndirectBuffer() {}

		virtual bool IsTransferd() = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void SetData(const void* data, uint32_t byteSize) = 0;
		virtual void ResizeBuffer(uint32_t byteSize) = 0;
		virtual void ResizeBuffer(const void* data, uint32_t byteSize) = 0;
		virtual uint32_t GetStrideSize() = 0;
		virtual uint32_t GetCount() = 0;
		virtual uint32_t GetByteSize() = 0;
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;


		virtual const void* GetDataPtr() const = 0;

		static AssetType GetStaticType() { return AssetType::IndirectBuffer; }
		AssetType GetType() const override { return GetStaticType(); }
	};

	class RYNEX_API UniformBuffer : public Asset
	{
	public:
		virtual ~UniformBuffer() {}


		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual bool IsTransferd() = 0;
		virtual void Bind(uint32_t slot = 15) = 0;
		virtual void UnBind(uint32_t slot) = 0;

		virtual void SetData(const void* data, uint32_t byteSize) = 0;
		virtual void SetData(const void* data, uint32_t offset, uint32_t byteSize) = 0;

		virtual uint32_t GetByteSize() const = 0;
		virtual uint32_t GetRenderID() const = 0;


		static Ref<UniformBuffer> Create(uint32_t byteSize);
		static Ref<UniformBuffer> Create(const void* data, uint32_t byteSize);
		static Ref<UniformBuffer> Create(const void* data, uint32_t byteSize, const BufferLayout& layout);
		static Ref<UniformBuffer> Create(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferFlagGPU flag);
		static Ref<UniformBuffer> CreateAsync(std::vector<uint8_t>&& data, const BufferLayout& layout, BufferFlagGPU flag);
		static Ref<UniformBuffer> CreateCopy(const Ref<UniformBuffer>& uniformBuffer);

		virtual const std::vector<uint8_t>& GetBufferData() const = 0;
		virtual void FreeBufferData() = 0;

		static AssetType GetStaticType() { return AssetType::UniformBuffer; }
		AssetType GetType() const override { return GetStaticType(); }
	};

	
}


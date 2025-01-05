#pragma once
#include "Rynex/Asset/Base/Asset.h"



namespace Rynex {
	
	enum class RYNEX_API ShaderDataType
	{
		None = 0, 
		Float, Float2, Float3, Float4, Float3x3, Float4x4, FloatArray,
		Int, Int2, Int3, Int4, Int3x3, Int4x4, IntArray,
		Uint, Uint2, Uint3, Uint4, Uint3x3, Uint4x4, UintArray,
		Texture,
	};
	

	enum class RYNEX_API BufferDataUsage {
		None = 0,
		DynamicDraw,
		StaticDraw,
	};

	using SDT = ShaderDataType;
	using BDU = BufferDataUsage;

	enum class RYNEX_API ShaderResourceType;

	static uint32_t ShaderDataTypeSize(ShaderDataType type, uint32_t ellements = 1)
	{
		switch (type)
		{
			case ShaderDataType::Float:			return sizeof(float);
			case ShaderDataType::Float2:		return sizeof(float) * 2;
			case ShaderDataType::Float3:		return sizeof(float) * 3;
			case ShaderDataType::Float4:		return sizeof(float) * 4;
			case ShaderDataType::Float3x3:		return sizeof(float) * 3 * 3;
			case ShaderDataType::Float4x4:		return sizeof(float) * 4 * 4;
			case ShaderDataType::FloatArray:	return ellements * sizeof(float);
			case ShaderDataType::Int:			return sizeof(int);
			case ShaderDataType::Int2:			return sizeof(int) * 2;
			case ShaderDataType::Int3:			return sizeof(int) * 3;
			case ShaderDataType::Int4:			return sizeof(int) * 4;
			case ShaderDataType::Int3x3:		return sizeof(int) * 3 * 3;
			case ShaderDataType::Int4x4:		return sizeof(int) * 4 * 4;
			case ShaderDataType::Texture:		return 0;
			case ShaderDataType::IntArray:		return ellements * sizeof(int);
			// case ShaderDataType::Uint:		return 4;
			// case ShaderDataType::Uint2:	return 4;
			// case ShaderDataType::Uint3:	return 4;
			// case ShaderDataType::Uint4:	return 4;
			// case ShaderDataType::Uint3x3:	return 4;
			// case ShaderDataType::Uint4x4:	return 4;
			// case ShaderDataType::UintArray	return 4;
			// case ShaderDataType::Bool:		 return sizeof(bool);
			// case ShaderDataType::Struct:	return 4;
		}
		RY_CORE_ASSERT(false, "Uknokn ShaderDataType!")
	};




	struct RYNEX_API BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normilized;
		uint32_t Count;
		
		BufferElement() = default;
		BufferElement(const BufferElement&) = default;
		BufferElement(ShaderDataType type, const std::string& name,  uint32_t count = 0, bool normilized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normilized(normilized), Count(count)
		{
		}

		
		
		
		uint32_t GetCompontsCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:			return 1;
			case ShaderDataType::Float2:		return 2;
			case ShaderDataType::Float3:		return 3;
			case ShaderDataType::Float4:		return 4;
			case ShaderDataType::Float3x3:		return 3 * 3;
			case ShaderDataType::Float4x4:		return 4 * 4;
			//caseShaderDataType::FloatArray:	 return 4;
			case ShaderDataType::Int:			return 1;
			case ShaderDataType::Int2:			return 2;
			case ShaderDataType::Int3:			return 3;
			case ShaderDataType::Int4:			return 4;
			case ShaderDataType::Int3x3:		return 3 * 3;
			case ShaderDataType::Int4x4:		return 4 * 4;
			//case ShaderDataType::IntArray:	return 4;
			//case ShaderDataType::Uint:		return 4;
			//case ShaderDataType::Uint2:		return 4;
			//case ShaderDataType::Uint3:		return 4;
			//case ShaderDataType::Uint4:		return 4;
			//case ShaderDataType::Uint3x3:		return 4;
			//case ShaderDataType::Uint4x4:		return 4;
			//case ShaderDataType::UintArray	return 4;
			// case ShaderDataType::Bool:			return 1;
			//case ShaderDataType::Struct:		return 4;
			}
			return 0;
		}

		bool operator==(const BufferElement& elemnet) const
		{
			RY_CORE_ASSERT(Count == elemnet.Count, "Decied If we can left that!");
			return (elemnet.Name == Name && elemnet.Type == Type && elemnet.Size == Size && Count == elemnet.Count);
		}
	};

	class RYNEX_API BufferLayout
	{
	public:
		BufferLayout() = default;
		
		BufferLayout(const BufferLayout&) = default;

		BufferLayout(const std::initializer_list<BufferElement>& element)
			: m_Elements(element)
		{
			CaculateOffsetAndStride();
		}
		BufferLayout(const std::vector<BufferElement>& element)
			: m_Elements(element)
		{
			CaculateOffsetAndStride();
		}
		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

		size_t GetLength() const { return m_Length; };
		size_t GetBufferCount() const { return m_BufferCount; };

		bool operator==(const BufferElement& elemnet) const
		{
			for(const BufferElement& elements : m_Elements)
			{
				if (elements == elemnet)
					return true;
			}
			return false;
		}

		void AddBufferElement(BufferElement& ellements)
		{
			m_Elements.push_back(ellements);
			CaculateOffsetAndStride();
		}

		void Clear()
		{
			m_Elements.clear();
			m_Stride = 0;
			m_Length = 0;
			m_BufferCount = 0;
		}

	private:
		void CaculateOffsetAndStride() 
		{
			uint32_t offset = 0;
			m_Stride = 0;
			m_Length = 0;
			m_BufferCount = 0;
			for (BufferElement& elements : m_Elements)
			{
				elements.Offset = offset;
				offset += elements.Size;
				m_Stride += elements.Size;
				m_Length++;
				m_BufferCount += elements.GetCompontsCount();
			}
		}
		
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
		size_t m_Length = 0;
		size_t m_BufferCount = 0;
	};

	class RYNEX_API BufferAPI
	{
	public:
		static void Init();
		static void Shutdown();	
		
		static ShaderDataType GetShaderDataTypeFromString(const std::string& type);
		static std::string GetStringFromShaderData(ShaderDataType type);
		static  std::map<ShaderDataType,std::string>& GetShaderDataStringMap();
		static std::map<std::string, ShaderDataType>& GetShaderDataFromStringMap();

		static std::map<ShaderResourceType, std::string>& BufferAPI::GetShaderResourceTypeStringMap();
		static std::map<std::string, ShaderResourceType>& BufferAPI::GetShaderResourceTypeFromStringMap();
	};

	class RYNEX_API VertexBuffer : public Asset
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout & layout) = 0;

		virtual void InitAsync() = 0;
		virtual void SetData(const void* data, uint32_t byteSize) = 0;
		virtual uint32_t GetByteSize() = 0;

		static Ref<VertexBuffer> Create(const uint32_t size);
		static Ref<VertexBuffer> Create(const void* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(const void* vertices, uint32_t size, BufferDataUsage usage);
		static Ref<VertexBuffer> Create(const void* vertices, uint32_t size, BufferDataUsage usage, const BufferLayout& layout);
		static Ref<VertexBuffer> CreateAsync( std::vector<unsigned char>&& data, uint32_t size, BufferDataUsage usage, const BufferLayout& layout);

		static Ref<VertexBuffer> Default();
		// virtual const RendererAPI::API GetRendererAPI() const = 0;

		virtual const std::vector<unsigned char>& GetBufferData() = 0;
		virtual void FreeBufferData() = 0;

		static AssetType GetStaticType() { return AssetType::VertexBuffer; }
		AssetType GetType() const { return GetStaticType(); }
	};

	class RYNEX_API IndexBuffer : public Asset
	{
	public:
		virtual uint32_t GetCount() const = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		
		virtual void InitAsync() = 0;
		virtual void SetData(const uint32_t* indices, uint32_t count) = 0;
		virtual void SetData(const uint16_t* indices, uint32_t count) = 0;

		virtual const std::vector<uint32_t> GetBufferData() = 0;
		virtual void FreeBufferData() = 0;
		virtual uint32_t GetElementByte() = 0; 
		virtual uint32_t GetRenderID() = 0;
		
		static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::None);
		static Ref<IndexBuffer> Create(const uint16_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::None);
		static Ref<IndexBuffer> CreateAsync(std::vector<uint32_t>&& data, uint32_t size, BufferDataUsage usage = BufferDataUsage::None);
		

		static AssetType GetStaticType() { return AssetType::IndexBuffer; }
		AssetType GetType() const { return GetStaticType(); }

	
		
	};

	class RYNEX_API StorageBuffer : public Asset
	{
	public: 
		enum class Type 
		{
			None = 0,
			Dynamic,
			Read,
			Write,
			Presistent,
			Coherent,
			Client
		};

	public:
		static Ref<StorageBuffer> Create(uint32_t byteSize, Type type);
		static Ref<StorageBuffer> Create(void* data, uint32_t byteSize, Type type);

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void UnBind() const = 0;
		virtual void InitAsync() = 0;
		virtual void SetData(const void* data, uint32_t byteSize) = 0;

		static AssetType GetStaticType() { return AssetType::StorageBuffer; }
		AssetType GetType() const { return GetStaticType(); }
	};

	class RYNEX_API UniformBuffer : public Asset
	{
	public:
		virtual ~UniformBuffer() {}


		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual void Bind(uint32_t slot = 15) = 0;

		virtual void SetData(const void* data, uint32_t byteSize) = 0;
		virtual void SetData(const void* data, uint32_t offset, uint32_t byteSize) = 0;
		virtual void SetLocelData(const BufferElement& ellement, const void* data, uint32_t byteSize) = 0;

		virtual void InitAsync() = 0;

		virtual uint32_t GetByteSize() = 0;
		virtual void SetOnDelete(std::function<void()> onDelete) = 0;

		static Ref<UniformBuffer> Create(uint32_t byteSize, uint32_t binding);
		static Ref<UniformBuffer> Create(const void* data, uint32_t byteSize, uint32_t binding);
		static Ref<UniformBuffer> Create(const void* data, uint32_t byteSize, const BufferLayout& layout, uint32_t binding);
		static Ref<UniformBuffer> Create(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferDataUsage usage, uint32_t binding);
		static Ref<UniformBuffer> CreateAsync(std::vector<unsigned char>&& data, const BufferLayout& layout, BufferDataUsage usage, uint32_t binding);
		// virtual const RendererAPI::API GetRendererAPI() const = 0;

		virtual const std::vector<unsigned char>& GetBufferData() = 0;
		virtual void FreeBufferData() = 0;

		static AssetType GetStaticType() { return AssetType::UniformBuffer; }
		AssetType GetType() const { return GetStaticType(); }
	};
}


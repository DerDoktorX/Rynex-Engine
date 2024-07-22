#pragma once

namespace Rynex {
	
	enum class ShaderDataType
	{
		None = 0, 
		Float, Float2, Float3, Float4, Float3x3, Float4x4, FloatArray,
		Int, Int2, Int3, Int4, Int3x3, Int4x4, IntArray,
		Uint, Uint2, Uint3, Uint4, Uint3x3, Uint4x4, UintArray,
		Bool,
		Struct,
	};
	
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case Rynex::ShaderDataType::Float:		 return sizeof(float);
			case Rynex::ShaderDataType::Float2:		 return sizeof(float) * 2;
			case Rynex::ShaderDataType::Float3:		 return sizeof(float) * 3;
			case Rynex::ShaderDataType::Float4:		 return sizeof(float) * 4;
			case Rynex::ShaderDataType::Float3x3:	 return sizeof(float) * 3 * 3;
			case Rynex::ShaderDataType::Float4x4:	 return sizeof(float) * 4 * 4;
			//case Rynex::ShaderDataType::FloatArray:	 return 4;
			case Rynex::ShaderDataType::Int:		 return sizeof(int);
			case Rynex::ShaderDataType::Int2:		 return sizeof(int) * 2;
			case Rynex::ShaderDataType::Int3:		 return sizeof(int) * 3;
			case Rynex::ShaderDataType::Int4:		 return sizeof(int) * 4;
			case Rynex::ShaderDataType::Int3x3:		 return sizeof(int) * 3 * 3;
			case Rynex::ShaderDataType::Int4x4:		 return sizeof(int) * 4 * 4;
			//case Rynex::ShaderDataType::IntArray:	return 4;
			//case Rynex::ShaderDataType::Uint:		return 4;
			//case Rynex::ShaderDataType::Uint2:	return 4;
			//case Rynex::ShaderDataType::Uint3:	return 4;
			//case Rynex::ShaderDataType::Uint4:	return 4;
			//case Rynex::ShaderDataType::Uint3x3:	return 4;
			//case Rynex::ShaderDataType::Uint4x4:	return 4;
			//case Rynex::ShaderDataType::UintArray	return 4;
			case Rynex::ShaderDataType::Bool:		 return sizeof(bool);
			//case Rynex::ShaderDataType::Struct:	return 4;
		}
		RY_CORE_ASSERT(false, "Uknokn ShaderDataType!")
	};

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normilized;
	
		BufferElement(){}
		
		BufferElement(ShaderDataType type, const std::string& name, bool normilized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normilized(normilized)
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
			case ShaderDataType::Bool:			return 1;
			//case ShaderDataType::Struct:		return 4;
			}
			return 0;
		}

		
	};

	class BufferLayout 
	{
	public:
		BufferLayout() {}
		BufferLayout(const BufferLayout&) {}

		BufferLayout(const std::initializer_list<BufferElement>& element)
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
	private:
		void CaculateOffsetAndStride() 
		{
			uint32_t offset = 0;
			m_Stride = 0;
			m_Length = 0;
			m_BufferCount = 0;
			for (auto& elements : m_Elements)
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


	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout & layout) = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual uint32_t GetCount() const = 0;
		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;
		virtual void SetData(const uint32_t* indices, uint32_t count) = 0;

		static Ref<IndexBuffer> Create(uint32_t count);
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}


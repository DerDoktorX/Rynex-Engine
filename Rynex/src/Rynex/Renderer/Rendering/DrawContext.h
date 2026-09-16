#pragma once
#include <Rynex/Memory/DynamicDataStruct.h>
#include <Rynex/Memory/StringScopeStack.h>

#include <Rynex/Renderer/Rendering/RenderTarget.h>

// #define RY_SCOPE_STREING_INLINE
// #define RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
namespace Rynex {
	enum class RenderBuffer
	{
		None = 0,
		FixedSize,
		AppendOnly,
		DeduplicatingAppend,
		RingBuffer,
		PersistentMapped,
		MultiBuffered
	};
#ifdef RY_USE_SINGLE_BUFFER_GROUP 
	template<typename T>
	struct RenderBuffer
	{
		Memory::DynamicDataStruct dataBuffer;
		T buffer;
	};
#endif

	struct BufferKey
	{
		std::string m_Scope;
		std::string m_Name;


		BufferKey()
			: m_Scope(), m_Name()
		{
			RY_CORE_WARN("Default BufferKey is empty by default!");
		}

		BufferKey(const BufferKey&) = default;
		// BufferKey(BufferKey&&) noexcept = default;


		BufferKey(const std::string& scope)
			: m_Scope(scope)
			, m_Name()
		{
		}

		BufferKey(const std::string& scope, const std::string& name)
			: m_Scope(scope)
			, m_Name(name)
		{
		}

		explicit BufferKey(std::string&& scope)
			: m_Scope(scope)
			, m_Name()
		{
		}

		explicit BufferKey(std::string&& scope, std::string&& name)
			: m_Scope(scope)
			, m_Name(name)
		{
		}

		explicit BufferKey(std::string&& scope, const std::string& name)
			: m_Scope(scope)
			, m_Name(name)
		{
		}

		bool operator==(const BufferKey& right) const
		{
			const bool isScope = m_Scope == right.m_Scope;
			const bool isName = m_Name == right.m_Name;

			return isScope && isName;
		};

		size_t GetHash() const
		{
			size_t hash;
			if(!m_Scope.empty() && !m_Name.empty())
			{
				std::string hashName = m_Scope + '.' + m_Name;
				hash = robin_hood::hash<std::string>{}(hashName);
			}
			else if(m_Scope.empty())
			{
				hash = robin_hood::hash<std::string>{}(m_Scope);
			}
			else
			{
				hash = robin_hood::hash<std::string>{}(m_Name);
			}
			return hash;
		}
	};
}
namespace robin_hood {


	template<>
	struct hash<Rynex::BufferKey>
	{
		std::size_t operator()(const Rynex::BufferKey& bufferKey) const
		{
			std::size_t hash = bufferKey.GetHash();
			return hash;
		}
	};
}
namespace Rynex {


	class DrawContext 
	{
	public:
		using BufferGPU = std::variant<Ref<VertexBuffer>, Ref<UniformBuffer>, Ref<Texture>, Ref<LinkedTextureArray>, Ref<StorageBuffer>, Ref<BindlesTextureArray>, Ref<IndexBuffer>>;

		struct RenderBuffer
		{
			Memory::DynamicDataStruct m_DataBuffer;
			BufferGPU m_Buffer = Ref<IndexBuffer>(nullptr);


			operator bool() const
			{
				if (std::holds_alternative<Ref<IndexBuffer>>(m_Buffer))
				{
					RY_CORE_ERROR("This RenderBuffer has a Ref<IndexBuffer> as buffer what is set by default! but not vaild!");
					return false;
				}
				return std::visit([](const auto& ref) 
					{
						if constexpr (std::is_same_v<std::decay_t<decltype(ref)>, Ref<IndexBuffer>>)
						{
							RY_CORE_ERROR("This RenderBuffer has a Ref<IndexBuffer> as buffer what is set by default! but not vaild!");
						}
						bool isCreatedRef = ref != nullptr;
						RY_CORE_ASSERT(isCreatedRef,"This RenderBuffer has a Ref<IndexBuffer> as buffer what is set by default! but not vaild!");
						return isCreatedRef;
					}, m_Buffer);
			}
		};

		using RenderBufferGPU = Ref<RenderBuffer>;
		using BufferPtrGPUVec = std::vector<RenderBufferGPU>;

#ifndef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
		struct RenderBufferScope
		{
			std::vector<RenderBufferGPU> m_ScopeBufferVec;
			HashMapFlat<std::string, BufferPtrGPUVec> m_ElementNameScopeHashMap;
		};
#endif
		
	public:
		template<typename T>
		using HashMapName = HashMapFlat<BufferKey, T>;
		

		


		
		using HashMapRenderBuffer = HashMapName<RenderBufferGPU>;
		using ItHashMapRenderBuffer = typename HashMapRenderBuffer::iterator;
		using ConstItHashMapRenderBuffer = typename HashMapRenderBuffer::const_iterator;

#ifndef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY

		using HashMapScopeRenderBuffer = HashMapFlat<std::string, RenderBufferScope>;
		using ItHashMapScopeRenderBuffer = typename HashMapScopeRenderBuffer::iterator;
		using ConstItHashMapScopeRenderBuffer = typename HashMapScopeRenderBuffer::const_iterator;
#else
		using HashMapElementRenderBuffer = HashMapName<BufferPtrGPUVec>;
		using ItHashMapElementRenderBuffer = typename HashMapElementRenderBuffer::iterator;
		using ConstItHashMapElementRenderBuffer = typename HashMapElementRenderBuffer::const_iterator;
#endif


	public:
		DrawContext()
		{

		}

		~DrawContext()
		{

		}
		void Clear();
		bool HasBuffer(const std::string& name);
		bool HasElement(const std::string& name);

		const BufferGPU& GetBuffer(const std::string& name) const;
        BufferGPU GetBufferFromScopeName(const std::string& scope, const std::string& name) const;
        BufferGPU GetBufferFromScopeName(const std::string& scopeName) const;

		template<typename T>
		Ref<T> GetBufferAs(const std::string& name) const
		{
			const Ref<T>* buffer = nullptr;

			const BufferGPU& bufferVarient = GetBuffer(name);
			
			buffer = std::get_if<Ref<T>>(&bufferVarient);
			if (nullptr == buffer)
				return Ref<T>(nullptr);
			return *buffer;
		}

		template<typename T>
		Ref<T> GetBufferFromScopeNameAs(const std::string& scope, const std::string& name) const
		{
			const Ref<T>* buffer = nullptr;

			const BufferGPU& bufferVarient = GetBufferFromScopeName(scope, name);

			buffer = std::get_if<Ref<T>>(&bufferVarient);
			if (nullptr == buffer)
				return Ref<T>(nullptr);
			return *buffer;
		}

        RenderBufferGPU GetRenderBuffer(const std::string& name) const;


#ifdef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
		template<typename T>
		void SetElement(const std::string& name, const T& value, uint32_t index = 0)
		{
#ifdef RY_SCOPE_STREING_INLINE
			BufferKey bufferKey{ m_ScopeName, name };
#else
			BufferKey bufferKey{ m_ScopeName.GetScope(), name };
#endif

			ItHashMapElementRenderBuffer it = m_HashMapElementRenderBuffer.find(bufferKey);
			if (it == m_HashMapElementRenderBuffer.end())
			{
				RY_CORE_WARN("Not found element in context {}", name);
				return;
			}

			BufferPtrGPUVec& bufferPtrGPUVec = it->second;
			for (RenderBufferGPU renderBufferGPU : bufferPtrGPUVec)
			{
				renderBufferGPU->dataBuffer.Set(name, index, value);
			}
		}


		template<typename T>
		void CreateBuffer(const std::string& name, const Ref<T>& buffer, BufferLayout layout, uint32_t arrayCount = 1u, bool geanrteNeElementMap = true)
		{
			RY_CORE_ASSERT(nullptr != buffer, "no vaild buffer!");
#ifdef RY_SCOPE_STREING_INLINE
			BufferKey bufferKey{ m_ScopeName, name };
#else
			BufferKey bufferKey( m_ScopeName.GetScopePath(), name );
#endif
			RenderBufferGPU& renderBuffer = m_HashMapRenderBuffer[bufferKey];
#if 1
			if (nullptr == renderBuffer)
				renderBuffer = CreateRef<RenderBuffer>();
#endif

			renderBuffer->buffer= buffer;
			renderBuffer->dataBuffer = Memory::DynamicDataStruct(layout, arrayCount);
			
			for(const BufferElement& e : layout)
			{
#ifdef RY_SCOPE_STREING_INLINE
				BufferKey bufferEllementKey{ m_ScopeName, e.name };
#else
				BufferKey bufferEllementKey{ m_ScopeName.GetScopePath(), e.name};
#endif
				m_HashMapElementRenderBuffer[bufferEllementKey];
			}
			if(geanrteNeElementMap)
				GenarteNewElementMap();
		}

#else

		template<typename T>
		void SetElement(const std::string& name, const T& value, uint32_t index = 0)
		{

			HashMapFlat<std::string, BufferPtrGPUVec>& inhartendEllmentNameScopeHashMap = m_InhartendScopeBuffer.m_ElementNameScopeHashMap;
			HashMapFlat<std::string, BufferPtrGPUVec>::iterator it = inhartendEllmentNameScopeHashMap.begin();

			if (it == inhartendEllmentNameScopeHashMap.end())
			{
				RY_CORE_WARN("Not found element in context {}", name);
				return;
			}

			BufferPtrGPUVec& bufferPtrGPUVec = it->second;

			for (RenderBufferGPU renderBufferGPU : bufferPtrGPUVec)
			{
				renderBufferGPU->m_DataBuffer.Set(name, index, value);
			}
		}

		template<typename T>
		void CreateBuffer(const std::string& name, const Ref<T>& buffer, BufferLayout layout, uint32_t arrayCount = 1u, bool geanrteNeElementMap = true)
		{
			RY_CORE_ASSERT(nullptr != buffer, "no vaild buffer!");
			const std::string& scopeName = m_ScopeName.GetScope();
			BufferKey bufferKey(scopeName, name);
			ItHashMapRenderBuffer pos = m_HashMapRenderBuffer.find(bufferKey);
			if(pos == m_HashMapRenderBuffer.end())
			{			
				RenderBufferGPU& renderBuffer = m_HashMapRenderBuffer[bufferKey];

				renderBuffer = CreateRef<RenderBuffer>();

				renderBuffer->m_Buffer = buffer;
				renderBuffer->m_DataBuffer = Memory::DynamicDataStruct(layout, arrayCount);

				InsertRenderBufferGPU(renderBuffer, scopeName);
			}
			else
			{				
				RenderBufferGPU& renderBuffer = pos->second;

				renderBuffer->m_Buffer = buffer;
				renderBuffer->m_DataBuffer = Memory::DynamicDataStruct(layout, arrayCount);

				OverrideRenderBufferGPU(renderBuffer, scopeName);
			}
		}
#endif

		void PushScope(AssetHandle handle);
		void PushScope(uint64_t value);
		void PushScope(uint32_t value);

		void PushScope(const std::string& name);
		void PushScope(const std::string& name, AssetHandle handle);
		void PushScope(const std::string& name, uint64_t value);
		void PushScope(const std::string& name, uint32_t value);

		void PrintScope() const;
		bool IsInScope(const std::string& name) const;
        std::string GetScopeToName(const std::string& name) const;

        std::string GetScopeName();
		void PopScope();

	private:
#ifdef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
		void GenarteNewElementMap();
		BufferPtrGPUVec GenarteBufferPtrGPUVec(const std::string& name);
#else
		void AddScopeBufferToInhertedScopeBuffer();
		void RemoveScopeBufferToInhertedScopeBuffer();


		void InsertRenderBufferGPU(const RenderBufferGPU& rendbufferGPU, const std::string& scopeName);
		void InsertRenderBufferGPUToScopeBufferVec(RenderBufferScope& renderBufferScope, const RenderBufferGPU& rendbufferGPU);
		void InsertRenderBufferGPUToScopeElementBufferVec(const RenderBufferGPU& rendbufferGPU, HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap);

		void OverrideRenderBufferGPU(const RenderBufferGPU& rendbufferGPU, const std::string& scopeName);
		void OverrideRenderBufferGPUScopeBufferVec(RenderBufferScope& renderBufferScope, const RenderBufferGPU& rendbufferGPU);
		void OverrideRenderBufferGPUScopeElementBufferVec(const RenderBufferGPU& rendbufferGPU, HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap);


		void RemoveRenderBufferGPU(const RenderBufferGPU& rendbufferGPU, const std::string& scopeName);
		void RemoveRenderBufferGPUScopeBufferVec(RenderBufferScope& renderBufferScope, const RenderBufferGPU& rendbufferGPU);
		void RemoveRenderBufferGPUScopeElementBufferVec(const RenderBufferGPU& rendbufferGPU, HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap);
		static bool RemoveRenderBufferGPUBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec);
		static void RemoveAllRenderBufferGPUBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec);
		static void RemoveRenderBufferVecRenderBufferVec(const std::vector<RenderBufferGPU>& srcRendbufferVec, std::vector<RenderBufferGPU>& distRendbufferVec);

		static void AddRenderBufferGPURenderBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec);
		static void AddRenderBufferVecRenderRenderBufferVec(const std::vector<RenderBufferGPU>& srcRendbufferVec, std::vector<RenderBufferGPU>& distRendbufferVec);

		static void OverrideRenderBufferGPUBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec);
		static void OverrideRenderBufferVecRenderRenderBufferVec(const std::vector<RenderBufferGPU>& srcRendbufferVec, std::vector<RenderBufferGPU>& distRendbufferVec);

#endif
		 
	private:


#ifdef RY_SCOPE_STREING_INLINE
		std::string m_ScopeName;
		std::vector<uint32_t> m_ScopeCountVec;
#else
		Memory::StringScopeStack m_ScopeName;
#endif

#ifdef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
		HashMapRenderBuffer m_HashMapRenderBuffer;
		HashMapElementRenderBuffer m_HashMapElementRenderBuffer;
#else
		HashMapRenderBuffer m_HashMapRenderBuffer;
		HashMapScopeRenderBuffer m_HashMapScopeRenderBuffer;
		RenderBufferScope m_InhartendScopeBuffer;

#endif

	};

}

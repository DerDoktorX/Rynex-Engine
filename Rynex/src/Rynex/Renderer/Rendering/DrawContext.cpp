#include "rypch.h"
#include "DrawContext.h"

namespace Rynex {

	void DrawContext::Clear()
	{
#ifdef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
		m_HashMapElementRenderBuffer.clear();
#else
		m_InhartendScopeBuffer.scopeBufferVec.clear();
		m_HashMapScopeRenderBuffer.clear();
#endif
		m_HashMapRenderBuffer.clear();
#ifdef RY_SCOPE_STREING_INLINE

		m_ScopeCountVec.clear();
		m_ScopeCountVec.shrink_to_fit();

		m_ScopeName.clear();
		m_ScopeName.shrink_to_fit();
#else
		m_ScopeName.Clear();

#endif

	}

	bool DrawContext::HasBuffer(const std::string& name)
	{
#ifdef RY_SCOPE_STREING_INLINE
		BufferKey bufferKey{ m_ScopeName, name };
#else
		BufferKey bufferKey{ m_ScopeName.GetScopePath(), name};
#endif

		ConstItHashMapRenderBuffer it = m_HashMapRenderBuffer.find(bufferKey);
		return it == m_HashMapRenderBuffer.end();
	}

	bool DrawContext::HasElement(const std::string& name)
	{
#ifdef RY_SCOPE_STREING_INLINE
		BufferKey bufferKey{ m_ScopeName, name };
#else
#endif

#ifdef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
		BufferKey bufferKey{ m_ScopeName.GetScopePath(), name};
		ConstItHashMapElementRenderBuffer it = m_HashMapElementRenderBuffer.find(bufferKey);
		return it == m_HashMapElementRenderBuffer.end();	
#else

		for (const RenderBufferGPU& buffer : m_InhartendScopeBuffer.scopeBufferVec)
		{
			const Memory::DynamicDataStruct& dataBuffer = buffer->dataBuffer;
			const BufferLayout& layout = dataBuffer.GetLayout();
			for (const BufferElement& e : layout)
			{
				if(e.name == name)
				{ 
					return true;
				}
			}
		}
		return false;

#endif
	}

	const DrawContext::BufferGPU& DrawContext::GetBuffer(const std::string& name) const
	{
#ifdef RY_SCOPE_STREING_INLINE
		return GetBufferFromScopeName(m_ScopeName, name);
#else
		return GetBufferFromScopeName(m_ScopeName.GetScope(), name);
#endif

	}

	const DrawContext::BufferGPU& DrawContext::GetBufferFromScopeName(const std::string& scope, const std::string& name) const
	{
		BufferKey bufferKey = BufferKey( scope, name );

		ConstItHashMapRenderBuffer it = m_HashMapRenderBuffer.find(bufferKey);
		if (it == m_HashMapRenderBuffer.end())
			return Ref<IndexBuffer>(nullptr);

		const RenderBufferGPU& renderBufferGPU = it->second;
		return renderBufferGPU->buffer;
	}

	const DrawContext::BufferGPU& DrawContext::GetBufferFromScopeName(const std::string& scopeName) const
	{
		BufferKey bufferKey = BufferKey( scopeName );

		ConstItHashMapRenderBuffer it = m_HashMapRenderBuffer.find(bufferKey);
		if (it == m_HashMapRenderBuffer.end())
			return Ref<IndexBuffer>(nullptr);

		const RenderBufferGPU& renderBufferGPU = it->second;
		return renderBufferGPU->buffer;
	}

	const DrawContext::RenderBufferGPU& DrawContext::GetRenderBuffer(const std::string& name) const
	{
#ifdef RY_SCOPE_STREING_INLINE
		BufferKey bufferKey{ m_ScopeName, name};
#else
		BufferKey bufferKey = BufferKey(m_ScopeName.GetScope(), name);
#endif

		ConstItHashMapRenderBuffer it = m_HashMapRenderBuffer.find(bufferKey);
		if (it == m_HashMapRenderBuffer.end())
		{
#ifdef RY_SCOPE_STREING_INLINE
			RY_CORE_ERROR("no RenderBuffer found {}.{}!", m_ScopeName, name);
#else
			RY_CORE_ERROR("no RenderBuffer found {}.{}!", m_ScopeName.GetScope(), name);
#endif

			return RenderBufferGPU();
		}

		return it->second;
	}

	bool DrawContext::IsInScope(const std::string& name) const
	{
#ifdef RY_SCOPE_STREING_INLINE
		if (m_ScopeName.empty())
			return false;
		size_t pos = m_ScopeName.rfind(name);

		return pos != m_ScopeName.max_size();
#else
		RY_CORE_NOT_IMPL();
		return false;
#endif

	}

	std::string&& DrawContext::GetScopeToName(const std::string& name) const
	{
#ifdef RY_SCOPE_STREING_INLINE
		if (m_ScopeName.empty())
		{
			RY_CORE_WARN("Scope string is empty!");
			return std::string();
		}

		size_t pos = m_ScopeName.rfind(name);
		const char* posFirst = m_ScopeName.c_str();
		const char* posEnd = posFirst + pos;
		return std::string(posFirst, posEnd);
#else
		return m_ScopeName.GetScopeToName(name);
#endif

	}

	void DrawContext::PushScope(AssetHandle handle)
	{
		uint64_t value = handle;

		PushScope(value);

	}

	void DrawContext::PushScope(uint64_t value)
	{
#ifdef RY_SCOPE_STREING_INLINE
		uint32_t scopeCharEnd = m_ScopeName.size();

		if (0u != scopeCharEnd)
			m_ScopeName += ".";

		m_ScopeName += std::to_string(value);
		m_ScopeCountVec.emplace_back(scopeCharEnd);
#else
		m_ScopeName.PushScope(std::to_string(value));
		AddScopeBufferToInhertedScopeBuffer();
#endif

	}

	void DrawContext::PushScope(uint32_t value)
	{
#ifdef RY_SCOPE_STREING_INLINE
		uint32_t scopeCharEnd = m_ScopeName.size();

		if (0u != scopeCharEnd)
			m_ScopeName += ".";

		m_ScopeName += std::to_string(value);
		m_ScopeCountVec.emplace_back(scopeCharEnd);
#else
		m_ScopeName.PushScope(std::to_string(value));
		AddScopeBufferToInhertedScopeBuffer();
#endif


	}

	void DrawContext::PushScope(const std::string& name)
	{
#ifdef RY_SCOPE_STREING_INLINE
		uint32_t scopeCharEnd = m_ScopeName.size();
		if (0u != scopeCharEnd)
			m_ScopeName += ".";
		m_ScopeName += name;
		
		m_ScopeCountVec.emplace_back(scopeCharEnd);
#else
		m_ScopeName.PushScope(name);
		AddScopeBufferToInhertedScopeBuffer();
#endif

	}

	void DrawContext::PushScope(const std::string& name, AssetHandle handle)
	{
		uint64_t value = handle;
		PushScope(name, value);
	}

	void DrawContext::PushScope(const std::string& name, uint64_t value)
	{
#ifdef RY_SCOPE_STREING_INLINE
		uint32_t scopeCharEnd = m_ScopeName.size();

		if (0u != scopeCharEnd)
			m_ScopeName += ".";
		m_ScopeName += name;
		m_ScopeName += "[";
		m_ScopeName += std::to_string(value);
		m_ScopeName += "]";

		m_ScopeCountVec.emplace_back(scopeCharEnd);
#else
		m_ScopeName.PushScope(name, value);
		AddScopeBufferToInhertedScopeBuffer();
#endif


	}

	void DrawContext::PushScope(const std::string& name, uint32_t value)
	{
#ifdef RY_SCOPE_STREING_INLINE
		uint32_t scopeCharEnd = m_ScopeName.size();
		if(0u != scopeCharEnd)
			m_ScopeName += ".";
		m_ScopeName += name;
		m_ScopeName += "[";
		m_ScopeName += std::to_string(value);
		m_ScopeName += "]";

		m_ScopeCountVec.emplace_back(scopeCharEnd);
#else


		m_ScopeName.PushScope(name, value);
		AddScopeBufferToInhertedScopeBuffer();
#ifndef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
		

#endif

#endif
	}

	void DrawContext::PrintScope() const
	{
#ifdef RY_SCOPE_STREING_INLINE
		RY_CORE_TRACE("DrawContext Scope: {}", m_ScopeName);
#else
		m_ScopeName.PrintScope();
#endif

	}

	std::string&& DrawContext::GetScopeName()
	{
#ifdef RY_SCOPE_STREING_INLINE
		return std::string(m_ScopeName);
#else
		return m_ScopeName.GetScopePath();
#endif

	}

	void DrawContext::PopScope()
	{
#ifdef RY_SCOPE_STREING_INLINE
		uint32_t scopeCharEnd= m_ScopeCountVec.back();
		m_ScopeCountVec.pop_back();
		std::string::const_iterator itPos = m_ScopeName.begin() + scopeCharEnd;
		std::string::const_iterator itEnd = m_ScopeName.end();

		m_ScopeName.erase(itPos, itEnd);
#else
		RemoveScopeBufferToInhertedScopeBuffer();
		m_ScopeName.PopScope();
#endif

	}

#ifdef RY_HASH_MAP_FOR_BUFFER_AND_ELEMENT_ONLY
	void DrawContext::GenarteNewElementMap()
	{
		for (auto& [elementName, renderBufferGPU] : m_HashMapElementRenderBuffer)
		{
			renderBufferGPU = GenarteBufferPtrGPUVec(elementName.name);
		}
	}

	DrawContext::BufferPtrGPUVec DrawContext::GenarteBufferPtrGPUVec(const std::string& name)
	{
		BufferPtrGPUVec vec;
		SDT type = SDT::None;
		for (auto& [bufferName, renderBufferGPU] : m_HashMapRenderBuffer)
		{
			const Memory::DynamicDataStruct& dynamicDataStruct = renderBufferGPU->dataBuffer;
			const BufferLayout& layout = dynamicDataStruct.GetLayout();
			for (const BufferElement& e : layout)
			{
				if (name == e.name)
				{
					RY_CORE_ASSERT(type == SDT::None || type == e.type, "Not expeted Type!");
					type = e.type;
					RenderBufferGPU dataPtr = renderBufferGPU;
					vec.emplace_back(dataPtr);
					break;
				}
			}
		}
		return vec;
	
	}
#else
	void DrawContext::AddScopeBufferToInhertedScopeBuffer()
	{
		const std::string& scopeName = m_ScopeName.GetScope();
		ConstItHashMapScopeRenderBuffer pos = m_HashMapScopeRenderBuffer.find(scopeName);
		if (m_HashMapScopeRenderBuffer.end() == pos)
			return;


		const std::vector<RenderBufferGPU>& renderBufferScopeVec = pos->second.scopeBufferVec;
		std::vector<RenderBufferGPU>& inhartendRenderBufferScopeVec = m_InhartendScopeBuffer.scopeBufferVec;
		AddRenderBufferVecRenderRenderBufferVec(renderBufferScopeVec, inhartendRenderBufferScopeVec);


		const HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap = pos->second.ellmentNameScopeHashMap;
		HashMapFlat<std::string, BufferPtrGPUVec>& inhartendEllmentNameScopeHashMap = m_InhartendScopeBuffer.ellmentNameScopeHashMap;

		for (auto&[elementName, renderBufferVec] : ellmentNameScopeHashMap)
		{
			std::vector<RenderBufferGPU>& inhartendRenderBufferElementVec = inhartendEllmentNameScopeHashMap[elementName];
			AddRenderBufferVecRenderRenderBufferVec(renderBufferVec, inhartendRenderBufferElementVec);
		}

	}

	void DrawContext::RemoveScopeBufferToInhertedScopeBuffer()
	{
		const std::string& scopeName = m_ScopeName.GetScope();
		ConstItHashMapScopeRenderBuffer pos = m_HashMapScopeRenderBuffer.find(scopeName);
		if (m_HashMapScopeRenderBuffer.end() == pos)
			return;


		const std::vector<RenderBufferGPU>& renderBufferScopeVec = pos->second.scopeBufferVec;
		std::vector<RenderBufferGPU>& inhartendRenderBufferScopeVec = m_InhartendScopeBuffer.scopeBufferVec;
		RemoveRenderBufferVecRenderBufferVec(renderBufferScopeVec, inhartendRenderBufferScopeVec);


		const HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap = pos->second.ellmentNameScopeHashMap;
		HashMapFlat<std::string, BufferPtrGPUVec>& inhartendEllmentNameScopeHashMap = m_InhartendScopeBuffer.ellmentNameScopeHashMap;

		for (auto& [elementName, renderBufferVec] : ellmentNameScopeHashMap)
		{
			std::vector<RenderBufferGPU>& inhartendRenderBufferElementVec = inhartendEllmentNameScopeHashMap[elementName];
			RemoveRenderBufferVecRenderBufferVec(renderBufferVec, inhartendRenderBufferElementVec);
		}
	}

	void DrawContext::InsertRenderBufferGPU(const RenderBufferGPU& rendbufferGPU, const std::string& scopeName)
	{
		RenderBufferScope& renderBufferScope = m_HashMapScopeRenderBuffer[scopeName];
		InsertRenderBufferGPUToScopeBufferVec(renderBufferScope, rendbufferGPU);

		RenderBufferScope& inhartendRenderBufferScope = m_InhartendScopeBuffer;
		InsertRenderBufferGPUToScopeBufferVec(inhartendRenderBufferScope, rendbufferGPU);
	}

	void DrawContext::InsertRenderBufferGPUToScopeBufferVec(RenderBufferScope& renderBufferScope, const RenderBufferGPU& rendbufferGPU)
	{
		auto& [scopeBufferVec, ellmentNameScopeHashMap] = renderBufferScope;
		AddRenderBufferGPURenderBufferVec(rendbufferGPU, scopeBufferVec);
		InsertRenderBufferGPUToScopeElementBufferVec(rendbufferGPU, ellmentNameScopeHashMap);
	}

	void DrawContext::InsertRenderBufferGPUToScopeElementBufferVec(const RenderBufferGPU& rendbufferGPU, HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap)
	{
		const Memory::DynamicDataStruct& dataBuffer = rendbufferGPU->dataBuffer;
		const BufferLayout& layout = dataBuffer.GetLayout();
		for (const BufferElement& e : layout)
		{
			BufferPtrGPUVec& bufferPtrVec = ellmentNameScopeHashMap[e.name];
			AddRenderBufferGPURenderBufferVec(rendbufferGPU, bufferPtrVec);
		}

	}

	void DrawContext::OverrideRenderBufferGPU(const RenderBufferGPU& rendbufferGPU, const std::string& scopeName)
	{
		RenderBufferScope& renderBufferScope = m_HashMapScopeRenderBuffer[scopeName];
		OverrideRenderBufferGPUScopeBufferVec(renderBufferScope, rendbufferGPU);

		RenderBufferScope& inhartendRenderBufferScope = m_InhartendScopeBuffer;
		OverrideRenderBufferGPUScopeBufferVec(inhartendRenderBufferScope, rendbufferGPU);
	}

	void DrawContext::OverrideRenderBufferGPUScopeBufferVec(RenderBufferScope& renderBufferScope, const RenderBufferGPU& rendbufferGPU)
	{
		auto& [scopeBufferVec, ellmentNameScopeHashMap] = renderBufferScope;

		OverrideRenderBufferGPUBufferVec(rendbufferGPU, scopeBufferVec);
		OverrideRenderBufferGPUScopeElementBufferVec(rendbufferGPU, ellmentNameScopeHashMap);
		
	}

	void DrawContext::OverrideRenderBufferGPUScopeElementBufferVec(const RenderBufferGPU& rendbufferGPU,HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap)
	{
		const Memory::DynamicDataStruct& dataBuffer = rendbufferGPU->dataBuffer;
		const BufferLayout& layout = dataBuffer.GetLayout();
		
		for (const BufferElement& e : layout)
		{
			BufferPtrGPUVec& bufferPtrVec = ellmentNameScopeHashMap[e.name];
			OverrideRenderBufferGPUBufferVec(rendbufferGPU, bufferPtrVec);
		}
	}

	void DrawContext::RemoveRenderBufferGPU(const RenderBufferGPU& rendbufferGPU, const std::string& scopeName)
	{
		RenderBufferScope& renderBufferScope = m_HashMapScopeRenderBuffer[scopeName];
		RemoveRenderBufferGPUScopeBufferVec(renderBufferScope, rendbufferGPU);

		RenderBufferScope& inhartendRenderBufferScope = m_InhartendScopeBuffer;
		RemoveRenderBufferGPUScopeBufferVec(inhartendRenderBufferScope, rendbufferGPU);
	}

	void DrawContext::RemoveRenderBufferGPUScopeBufferVec(RenderBufferScope& renderBufferScope, const RenderBufferGPU& rendbufferGPU)
	{
		auto& [scopeBufferVec, ellmentNameScopeHashMap] = renderBufferScope;

		RemoveAllRenderBufferGPUBufferVec(rendbufferGPU, scopeBufferVec);
		RemoveRenderBufferGPUScopeElementBufferVec(rendbufferGPU, ellmentNameScopeHashMap);
	}

	void DrawContext::RemoveRenderBufferGPUScopeElementBufferVec(const RenderBufferGPU& rendbufferGPU, HashMapFlat<std::string, BufferPtrGPUVec>& ellmentNameScopeHashMap)
	{
		const Memory::DynamicDataStruct& dataBuffer = rendbufferGPU->dataBuffer;
		const BufferLayout& layout = dataBuffer.GetLayout();

		for (const BufferElement& e : layout)
		{
			BufferPtrGPUVec& bufferPtrVec = ellmentNameScopeHashMap[e.name];
			RemoveAllRenderBufferGPUBufferVec(rendbufferGPU, bufferPtrVec);
		}

	}

	bool DrawContext::RemoveRenderBufferGPUBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec)
	{
		uint32_t index = MAXUINT32;
		uint32_t i = 0;
		for (const RenderBufferGPU& renderBuffer : rendbufferGPUVec)
		{
			if (renderBuffer == rendbufferGPU)
			{
				index = i;
				break;
			}
		}

		if (MAXUINT32 != index)
		{
			BufferPtrGPUVec::iterator it = rendbufferGPUVec.begin() + index;
			rendbufferGPUVec.erase(it);
			return true;
		}
		return false;
	}

	void DrawContext::RemoveAllRenderBufferGPUBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec)
	{
		uint32_t count = 0;
		
		while (RemoveRenderBufferGPUBufferVec(rendbufferGPU, rendbufferGPUVec))
		{ 
			count++;
		}

		if(1 < count)
		{
			RY_CORE_TRACE("More then one time in vec {}!", count);
		}
		else if(count == 0)
		{
			RY_CORE_INFO("No RenderBufferGPU found!");
		}
	}

	void DrawContext::RemoveRenderBufferVecRenderBufferVec(const std::vector<RenderBufferGPU>& srcRendbufferVec, std::vector<RenderBufferGPU>& distRendbufferVec)
	{
		for (const RenderBufferGPU& srcRenderBuffer : srcRendbufferVec)
		{
			RemoveRenderBufferGPUBufferVec(srcRenderBuffer, distRendbufferVec);
		}
	}

	

	void DrawContext::AddRenderBufferGPURenderBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec)
	{
#ifndef RY_DIST
		for (const RenderBufferGPU& renderBuffer : rendbufferGPUVec)
		{
			RY_CORE_ASSERT(renderBuffer != rendbufferGPU, "not expexted to find dublicats!")
		}
#endif
		rendbufferGPUVec.emplace_back(rendbufferGPU);
	}

	void DrawContext::AddRenderBufferVecRenderRenderBufferVec(const std::vector<RenderBufferGPU>& srcRendbufferVec, std::vector<RenderBufferGPU>& distRendbufferVec)
	{
		for (const RenderBufferGPU& srcRenderBuffer : srcRendbufferVec)
		{
			AddRenderBufferGPURenderBufferVec(srcRenderBuffer, distRendbufferVec);
		}
	}

	

	void DrawContext::OverrideRenderBufferGPUBufferVec(const RenderBufferGPU& rendbufferGPU, std::vector<RenderBufferGPU>& rendbufferGPUVec)
	{
		for (const RenderBufferGPU& renderBuffer : rendbufferGPUVec)
		{
			if (renderBuffer == rendbufferGPU)
				return;
		}
		rendbufferGPUVec.emplace_back(rendbufferGPU);
	}

	void DrawContext::OverrideRenderBufferVecRenderRenderBufferVec(const std::vector<RenderBufferGPU>& srcRendbufferVec, std::vector<RenderBufferGPU>& distRendbufferVec)
	{
		for (const RenderBufferGPU& srcRenderBuffer : srcRendbufferVec)
		{
			OverrideRenderBufferGPUBufferVec(srcRenderBuffer, distRendbufferVec);
		}
	}

	

#endif



}


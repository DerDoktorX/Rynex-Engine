#pragma once
#include <rypch.h>


#define RY_RENDER_RESOURCE_DATA_ARRAY 0

namespace Rynex {
	
	template<typename T, typename N>
	class RenderResourceList
	{
	public:

		using TypeT = T;
		using _N = N;
		using ResourceT = Ref<TypeT>;
		
		struct InfoData
		{
			uint32_t Count;
#if RY_RENDER_RESOURCE_DATA_ARRAY
			_N* Data;
#else
			_N Data;
#endif

			Ref<TypeT> Resource;
		};

		using _InfoDataNT = InfoData;
	public:
		RenderResourceList()
		{

		};

		~RenderResourceList()
		{

		};

		_InfoDataNT& Add(const ResourceT& resource, const _N& data)
		{
			const std::unordered_map<UUID, uint32_t>::const_iterator& it = m_ResourceFinder.find(resource->Handle);
			if (it == m_ResourceFinder.end())
			{
				uint32_t index = m_Size;
#if RY_RENDER_RESOURCE_DATA_ARRAY
				_N& dataRef = m_Data.emplace_back<_N>(_N{ data });
#endif

				_InfoDataNT& info = m_Infos.template emplace_back<_InfoDataNT>(
					_InfoDataNT{
						1u,
#if RY_RENDER_RESOURCE_DATA_ARRAY
						&dataRef,
#else
						data,
#endif
						resource
					});
				
				
				m_ResourceFinder[resource->Handle] = index;
				m_Size++;

#if RY_RENDER_RESOURCE_DATA_ARRAY
				RY_CORE_ASSERT((m_Data.data() + index) == ((m_Infos.data() + index)->Data));
#endif
				RY_CORE_ASSERT(resource.get() == info.Resource.get());
				return info;
			}
			else
			{
				uint32_t index = it->second;
				_InfoDataNT& info = m_Infos.at(index);
				
#if RY_RENDER_RESOURCE_DATA_ARRAY
				RY_CORE_ASSERT((m_Data.data() + index) == &((m_Infos.data() + index)->Data));
#endif
				info.Count++;
				RY_CORE_ASSERT(resource.get() == info.Resource.get());
				return info;
			}
		}

		_InfoDataNT& Add(const ResourceT& resource)
		{
			return Add(resource, _N());
		}

		_InfoDataNT GetInfo(const ResourceT& resource)
		{
			using ItConst = std::unordered_map<UUID, uint32_t>::iterator;
			ItConst it = m_ResourceFinder.find(resource->Handle);
			if (it != m_ResourceFinder.end())
			{
				uint32_t index = it->second;
				_InfoDataNT& info = m_Infos.at(index);
#if RY_RENDER_RESOURCE_DATA_ARRAY
				RY_CORE_ASSERT((m_Data.data() + index) == (m_Infos.data() + index)->Data);
#endif
				return info;
			}
			RY_CORE_ASSERT(false);
			_InfoDataNT infoDataNT{
				0u, _N(), nullptr
			};

			return infoDataNT;
		}

		

		_N GetData(const ResourceT& resource)
		{
			_InfoDataNT resourceData = GetInfo(resource);
			return resourceData.Data;
		}

		ResourceT& GetResource(const ResourceT& resource)
		{
			_InfoDataNT& resourceData = GetInfo(resource);
			return resourceData.Resource;
		}

		uint32_t GetCount(const ResourceT& resource)
		{
			_InfoDataNT& resourceData = GetInfo(resource);
			return resourceData.Count;
		}

		uint32_t GetIndex(const ResourceT& resource)
		{
			const std::unordered_map<UUID, uint32_t>::const_iterator& it = m_ResourceFinder.find(resource->Handle);
			if (it == m_ResourceFinder.end())
				return it->second;
			RY_CORE_ASSERT(false);
			return MAXUINT32;
		}

		bool Has(const ResourceT& resource)
		{
			using ItConst = std::unordered_map<UUID, uint32_t>::iterator;
			ItConst it = m_ResourceFinder.find(resource->Handle);
			return it != m_ResourceFinder.end();
		}

		void Remove(const ResourceT& resource)
		{
			const std::unordered_map<UUID, uint32_t>::const_iterator& it = m_ResourceFinder.find(resource->Handle);
			if (it == m_ResourceFinder.end())
			{
				RY_CORE_ASSERT(false);
				return;
			}
			uint32_t index = it->second;
			_InfoDataNT& info = m_Infos.at(index);
			info.Count--;
			if(info.Count != 0)
				return;

			m_ResourceFinder.clear();

			using It = typename std::vector<_InfoDataNT>::iterator;
			It itInfoBegin = m_Infos.begin();
			m_Infos.erase(itInfoBegin + index);
			std::vector<_InfoDataNT> resoureData = m_Infos;
			m_Infos.clear();

#if RY_RENDER_RESOURCE_DATA_ARRAY
			std::vector<_N>::iterator& itDataBegin = m_Data.begin();
			m_Data.erase(itDataBegin + index);
			std::vector<_N> data = m_Data;
			m_Data.clear();
#endif
			uint32_t i = 0;
			for (_InfoDataNT& res : resoureData)
			{
#if RY_RENDER_RESOURCE_DATA_ARRAY
				_N& dataRef = m_Data.emplace_back<_N>(_N{ data.at(i)});
				m_Infos.emplace_back<InfoData>(InfoData{ res.Count,  ,res.Resource});
#endif
				m_Infos.template emplace_back<_InfoDataNT>(_InfoDataNT{ res });
				m_ResourceFinder[res.Resource->Handle] = i;
				i++;
			}
			
		}

		uint32_t Size()
		{
#if RY_RENDER_RESOURCE_DATA_ARRAY
			RY_CORE_ASSERT(m_Infos.size() == m_Data.size());
#endif
			RY_CORE_ASSERT(m_ResourceFinder.size() == m_Infos.size());
			return m_Infos.size();
		}

#if RY_RENDER_RESOURCE_DATA_ARRAY
		_N* DataPtr()
		{
			return m_Data.data();
		}
#endif

		_InfoDataNT* InfoPtr()
		{
			return m_Infos.data();
		}

		std::vector<_InfoDataNT>& GetInfoRef()
		{
			return m_Infos;
		}

		const std::vector<_InfoDataNT>& GetInfo() const
		{
			return m_Infos;
		}

		_InfoDataNT& GetInfoIndex(uint32_t index)
		{
			return m_Infos.at(index);
		}

#if RY_RENDER_RESOURCE_DATA_ARRAY
		std::vector<_N>& GetDataRef()
		{
			return m_Data;
		}
		
		const std::vector<_N>& GetDataEnd() const
		{
			return m_Data;
		}
#endif

		typename std::vector<_InfoDataNT>::iterator begin()
		{
			return m_Infos.begin();
		}
		
		typename std::vector<_InfoDataNT>::iterator end()
		{
			return m_Infos.end();
		}

		typename std::vector<_InfoDataNT>::const_iterator begin() const
		{
			return m_Infos.begin();
		}
		
		typename std::vector<_InfoDataNT>::const_iterator end() const
		{
			return m_Infos.end();
		}

		void Clear()
		{
			m_ResourceFinder.clear();
			m_Infos.clear();
			m_Size = 0;
		}


	private:
		std::unordered_map<UUID, uint32_t> m_ResourceFinder;
		std::vector<_InfoDataNT> m_Infos;
#if RY_RENDER_RESOURCE_DATA_ARRAY
		std::vector<_N> m_Data;
#endif
		uint32_t m_Size = 0u;
	};
}



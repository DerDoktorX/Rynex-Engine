#pragma once
#include <rypch.h>
#include <Rynex/Renderer/API/ProtypeAPI.h>

namespace Rynex {
	template<typename T>
	class ResourceMap
	{
	private:
		// using T = Texture;
		using EntityVec = std::vector<int>;
		using PairResource = std::pair<Ref<T>, EntityVec>;
		using ConstItMap = typename HashMapNode<Ref<T>, uint64_t>::const_iterator;
	public:

		uint64_t GetOrCreate(const Ref<T>& object, int enitity)
		{
			uint64_t idIndex = GetIndexIdOrCreate(object);
			PairResource& resurce = m_List.at(idIndex);
			EntityVec& vec = resurce.second;

			if(!IsEntityInVec(vec, enitity));
			{
				vec.emplace_back(enitity);
			}
			return idIndex;
		}

		void Remove(uint64_t id, int enitity)
		{
			RY_CORE_ASSERT(id < m_List.size());
			PairResource& resurce = m_List.at(id);
			EntityVec& vec = resurce.second;

			constexpr size_t max = std::numeric_limits<size_t>::max();
			size_t index = GetEntityIndexVec(vec, enitity);
			if (max == index)
			{
				RY_CORE_WARN("entity was not in the list!");
				return;
			}

			auto it = vec.begin() + index;
			vec.erase(it);

		}

		Ref<T>& Get(uint64_t id)
		{
			RY_CORE_ASSERT(id < m_List.size());
			return m_List.at(id).first;
		}

		void Clear()
		{
			m_List.clear();
			m_ResurceIdMap.clear();
		}

		bool IsEntityInResurce(uint64_t id, int enitity)const
		{
			RY_CORE_ASSERT(id < m_List.size());
			const PairResource& resurce = m_List.at(id);
			const EntityVec& vec = resurce.second;

			return IsEntityInVec(vec, enitity);
		}

		size_t CountFromResource(uint64_t id) const
		{
			RY_CORE_ASSERT(id < m_List.size());
			const PairResource& resurce = m_List.at(id);
			const EntityVec& vec = resurce.second;
			return vec.size();
		}
	private:
		

		bool IsEntityInVec(const EntityVec& vec, int enitity)const
		{
			constexpr size_t max = std::numeric_limits<size_t>::max();
			size_t index = GetEntityIndexVec(vec, enitity);
			return max != index;
		}

		size_t GetEntityIndexVec(const EntityVec& vec, int enitity)const
		{
			size_t index = 0;
			for (const int& e : vec)
			{
				if (e == enitity)
					return index;
				index++;
			}
			constexpr size_t max = std::numeric_limits<size_t>::max();
			return max;
		}

		uint64_t GetIndexId(const Ref<T>& object) const
		{
			constexpr size_t max = std::numeric_limits<size_t>::max();
			ConstItMap it = m_ResurceIdMap.find(object);

			if (it == m_ResurceIdMap.end())
				return max;

			uint64_t idIndex = it->second;
			return idIndex;
		}

		uint64_t GetIndexIdOrCreate(const Ref<T>& object)
		{
			ConstItMap it = m_ResurceIdMap.find(object);
			if(it != m_ResurceIdMap.end())
			{
				uint64_t idIndex = it->second;
				return idIndex;
			}
			uint64_t idIndexCreate = m_List.size();
			m_List.emplace_back(PairResource{ object, EntityVec{} });
			m_ResurceIdMap[object] = idIndexCreate;
			return idIndexCreate;
		}
	private:
		std::vector<PairResource> m_List;

		HashMapNode<Ref<T>, uint64_t> m_ResurceIdMap;
	};
}

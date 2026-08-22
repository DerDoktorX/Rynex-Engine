#include "rypch.h"
#include "RenderProxyConectionEntity.h"

#include <Rynex/Memory/ElementVectorFunc.h>

namespace Rynex {

	RenderProxyConectionEntity::RenderProxyConectionEntity()
	{
	}

	const RenderProxyConectionEntity::IndexSubMesh& RenderProxyConectionEntity::GetIndexSubMeshFromMap(int entity, uint32_t subMesh) const
	{

		using ItMap = robin_hood::unordered_flat_map<int, std::vector<IndexSubMesh>>::const_iterator;
		ItMap it = m_EnitiyToVecIndicesMap.find(entity);
		RY_CORE_ASSERT(it != m_EnitiyToVecIndicesMap.end(), "The given Enity is not be found!");
		const std::vector<IndexSubMesh>& indicesSubMeshVec = it->second;
		RY_CORE_ASSERT(!indicesSubMeshVec.empty(), "The list is empty so sub mesh count not me in it! And the key shoud not any longer exist!");

		uint32_t indicesSubMeshIndex = GetSubMeshVecIndexByBinarySearch(indicesSubMeshVec, subMesh);
		RY_CORE_ASSERT(UINT32_MAX != indicesSubMeshIndex, "Not found proxy Index from sub mesh!");

		return indicesSubMeshVec.at(indicesSubMeshIndex);
	}

	uint32_t RenderProxyConectionEntity::GetProxyIndexFormMap(int entity, uint32_t subMesh)
	{
		using ItMap = robin_hood::unordered_flat_map<int, std::vector<IndexSubMesh>>::const_iterator;
		ItMap it = m_EnitiyToVecIndicesMap.find(entity);
		RY_CORE_ASSERT(it != m_EnitiyToVecIndicesMap.end());
		const std::vector<IndexSubMesh>& indexSubMeshVec = it->second;
		RY_CORE_ASSERT(!indexSubMeshVec.empty(), "The list is empty so sub mesh count not me in it!");

		uint32_t subMeshProxyIndex = GetSubMeshProxyIndexByBinarySearch(indexSubMeshVec, subMesh);
		RY_CORE_ASSERT(UINT32_MAX != subMeshProxyIndex, "Not found proxy Index from sub mesh!");

		return subMeshProxyIndex;
	}

	const std::vector<RenderProxyConectionEntity::IndexSubMesh>& RenderProxyConectionEntity::GetIndicesSubMeshVecFromMap(int entity) const
	{
		using ItMap = robin_hood::unordered_flat_map<int, std::vector<IndexSubMesh>>::const_iterator;
		ItMap it = m_EnitiyToVecIndicesMap.find(entity);
		RY_CORE_ASSERT(it != m_EnitiyToVecIndicesMap.end(), "The given Enity is not be found!");
		const std::vector<IndexSubMesh>& indicesSubMeshVec = it->second;
		RY_CORE_ASSERT(!indicesSubMeshVec.empty(), "The list is empty so sub mesh count not me in it! And the key shoud not any longer exist!");
		return indicesSubMeshVec;
	}

	std::pair<uint32_t, RenderProxyKey> RenderProxyConectionEntity::RemoveProxyIndexFromMap(int entity, uint32_t subMesh)
	{
		using ItMap = robin_hood::unordered_flat_map<int, std::vector<IndexSubMesh>>::iterator;
		using ItErase = std::vector<IndexSubMesh>::const_iterator;

		ItMap it = m_EnitiyToVecIndicesMap.find(entity);
		RY_CORE_ASSERT(it != m_EnitiyToVecIndicesMap.end(), "The given Enity is not be found!");
		std::vector<IndexSubMesh>& indicesSubMeshVec = it->second;
		RY_CORE_ASSERT(!indicesSubMeshVec.empty(), "The list is empty so sub mesh count not me in it! And the key shoud not any longer exist!");

		uint32_t indicesSubMeshIndex = GetSubMeshVecIndexByBinarySearch(indicesSubMeshVec, subMesh);
		RY_CORE_ASSERT(UINT32_MAX != indicesSubMeshIndex, "Not found proxy Index from sub mesh!");
		ItErase begin = indicesSubMeshVec.begin();
		ItErase erasePos = begin + indicesSubMeshIndex;
		uint32_t proxyIndex = indicesSubMeshVec.at(indicesSubMeshIndex).proxyIndex;
		RenderProxyKey renderProxyKey = indicesSubMeshVec.at(indicesSubMeshIndex).renderProxyKey;
		indicesSubMeshVec.erase(erasePos);

		if (indicesSubMeshVec.empty())
		{
			m_EnitiyToVecIndicesMap.erase(entity);
		}
		RY_CORE_INFO("Remove Enity {} Submesh {} Index[{}]", entity, subMesh, proxyIndex);

		return {proxyIndex, renderProxyKey};
	}
	

	std::vector<RenderProxyConectionEntity::IndexSubMesh> RenderProxyConectionEntity::RemoveProxyIndexVecFromMap(int entity)
	{
		using ItMap = HashMapFlat<int, std::vector<IndexSubMesh>>::iterator;
		using ItErase = std::vector<IndexSubMesh>::const_iterator;

		ItMap it = m_EnitiyToVecIndicesMap.find(entity);
		if (it == m_EnitiyToVecIndicesMap.end())
		{
			RY_CORE_WARN("Entity {} do not exist hear!");
			return std::vector<RenderProxyConectionEntity::IndexSubMesh>();
		}
		const std::vector<RenderProxyConectionEntity::IndexSubMesh>& indicesSubMeshVec = it->second;

		RY_CORE_ASSERT(!indicesSubMeshVec.empty(), "The list is empty so sub mesh count not me in it! And the key shoud not any longer exist!");
		std::vector<RenderProxyConectionEntity::IndexSubMesh> indicesSubMeshVecCopy = indicesSubMeshVec;

		m_EnitiyToVecIndicesMap.erase(entity);
		return indicesSubMeshVecCopy;
	}

	void RenderProxyConectionEntity::AddProxyIndexToMap(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, RenderProxySortedProxyVec& renderProxySortedProxyVec)
	{
		using ItInsert = std::vector<IndexSubMesh>::iterator;
		std::vector<IndexSubMesh>& subMeshIniciesVec = m_EnitiyToVecIndicesMap[entity];

		ItInsert itPos = std::lower_bound(subMeshIniciesVec.begin(), subMeshIniciesVec.end(), subMesh,
			[](const IndexSubMesh& aIndices, uint32_t bSubMesh)
			{
				return aIndices.subMesh < bSubMesh;
			});

		{

			bool isVaildInsideRange = nullptr != itPos._Ptr && itPos < subMeshIniciesVec.end();
			bool hasSameSubMesh = isVaildInsideRange ? itPos._Ptr->subMesh == subMesh : false;
			bool hasNotSameIndexOrOutSideVaildRange = isVaildInsideRange ? itPos._Ptr->proxyIndex != proxyIndex : true;
			if (hasSameSubMesh && hasNotSameIndexOrOutSideVaildRange)
			{
				RY_CORE_WARN(" Submesh {} From Entity {} tried to add again! Abourt Add and removed the new Proxy Again, and instand Trede it like a Update off all proxy Data", subMesh, entity);
				uint32_t oldProxyIndex = itPos._Ptr->proxyIndex;
				uint32_t oldRenderProxyKey = itPos._Ptr->renderProxyKey;
#ifdef RY_SORT_KEY
				renderProxySortedProxyVec.Remove(oldProxyIndex, oldRenderProxyKey);
#else
				renderProxySortedProxyVec.Remove(oldProxyIndex, oldRenderProxyKey);
#endif

				itPos._Ptr->proxyIndex = proxyIndex;
				return;
			}
			else if (hasNotSameIndexOrOutSideVaildRange)
			{
				subMeshIniciesVec.insert(itPos, IndexSubMesh{ proxyIndex, subMesh, renderProxyKey });

				RY_CORE_INFO("Add Enity {} Submesh {} Index[{}]", entity, subMesh, proxyIndex);
			}
			else
			{
				RY_CORE_WARN("Add Enity {} Submesh {} Index[{}] again skip", entity, subMesh, proxyIndex);
			}
		}

	}

	void RenderProxyConectionEntity::AddProxyIndexToMap(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, RenderProxySortedIndicesVec& renderProxySortedProxyVec)
	{
		using ItInsert = std::vector<IndexSubMesh>::iterator;
		std::vector<IndexSubMesh>& subMeshIniciesVec = m_EnitiyToVecIndicesMap[entity];


		std::function<bool(const IndexSubMesh&, const IndexSubMesh&)> sortFunc = [](const IndexSubMesh& aIndices, const IndexSubMesh& element)
			{
				return aIndices.subMesh < element.subMesh;
			};
		std::function<bool(const IndexSubMesh&, const IndexSubMesh&)> equalFunc = [](const IndexSubMesh& aIndices, const IndexSubMesh& element)
			{
				return aIndices.subMesh == element.subMesh && aIndices.proxyIndex == element.proxyIndex;
			};

		IndexSubMesh element = {
			proxyIndex, subMesh, renderProxyKey
		};

		if (Memory::AddSortedElementVector(subMeshIniciesVec, element, sortFunc, equalFunc))
		{
			RY_CORE_INFO("Add Enity {} Submesh {} Index[{}]", entity, element.subMesh, element.proxyIndex);
		}
		else
		{
			RY_CORE_WARN(" Submesh {} From Entity {} tried to add again! Abourt Add and removed the new Proxy Again, and instand Trede it like a Update off all proxy Data", subMesh, entity);
			ItInsert it = Memory::GetSortedElementIteratorVector(subMeshIniciesVec, element, sortFunc);

			uint32_t oldProxyIndex = it->proxyIndex;
			uint32_t oldRenderProxyKey = it._Ptr->renderProxyKey;
			renderProxySortedProxyVec.Remove(oldProxyIndex, oldRenderProxyKey);
			it->proxyIndex = element.proxyIndex;
		}


	}

	void RenderProxyConectionEntity::AddProxyIndexToMap(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, RenderProxyMapProxyVec& renderProxyMapProxyVec)
	{
		using ItInsert = std::vector<IndexSubMesh>::iterator;
		std::vector<IndexSubMesh>& subMeshIniciesVec = m_EnitiyToVecIndicesMap[entity];


		std::function<bool(const IndexSubMesh&, const IndexSubMesh&)> sortFunc = [](const IndexSubMesh& aIndices, const IndexSubMesh& element)
			{
				return aIndices.subMesh < element.subMesh;
			};
		std::function<bool(const IndexSubMesh&, const IndexSubMesh&)> equalFunc = [](const IndexSubMesh& aIndices, const IndexSubMesh& element)
			{
				return aIndices.subMesh == element.subMesh && aIndices.proxyIndex == element.proxyIndex;
			};

		IndexSubMesh element = {
			proxyIndex, subMesh, renderProxyKey
		};

		if (Memory::AddSortedElementVector(subMeshIniciesVec, element, sortFunc, equalFunc))
		{
			RY_CORE_INFO("Add Enity {} Submesh {} Index[{}]", entity, element.subMesh, element.proxyIndex);
		}
		else
		{
			RY_CORE_WARN(" Submesh {} From Entity {} tried to add again! Abourt Add and removed the new Proxy Again, and instand Trede it like a Update off all proxy Data", subMesh, entity);
			ItInsert it = Memory::GetSortedElementIteratorVector(subMeshIniciesVec, element, sortFunc);

			uint32_t oldProxyIndex = it->proxyIndex;
			uint32_t oldRenderProxyKey = it._Ptr->renderProxyKey;
			renderProxyMapProxyVec.Remove(oldProxyIndex, oldRenderProxyKey);
			it->proxyIndex = element.proxyIndex;
		}


	}

	void RenderProxyConectionEntity::CheckConectionToProxyData(RenderProxySortedProxyVec& renderProxySortedProxyVec)
	{
		for (auto& [entity, subMeshIniciesVec] : m_EnitiyToVecIndicesMap)
		{
			for (const IndexSubMesh& subMeshInicies : subMeshIniciesVec)
			{
				RenderProxy& proxy = renderProxySortedProxyVec.AtProxy(subMeshInicies.renderProxyKey, subMeshInicies.proxyIndex);
				RY_CORE_ASSERT(proxy.entity == entity, "We update the whrong Enity in the Proxy!");
				RY_CORE_ASSERT(proxy.subMesh == subMeshInicies.subMesh, "We update the whrong SubMesh in the Proxy!");
			}
		}
	}

	void RenderProxyConectionEntity::CheckConectionToProxyData(RenderProxySortedIndicesVec& renderProxySortedProxyVec)
	{
		for (auto& [entity, subMeshIniciesVec] : m_EnitiyToVecIndicesMap)
		{
			for (const IndexSubMesh& subMeshInicies : subMeshIniciesVec)
			{
				RenderProxy& proxy = renderProxySortedProxyVec.AtProxy(subMeshInicies.proxyIndex);
				RY_CORE_ASSERT(proxy.entity == entity, "We update the whrong Enity in the Proxy!");
				RY_CORE_ASSERT(proxy.subMesh == subMeshInicies.subMesh, "We update the whrong SubMesh in the Proxy!");
			}
		}
	}

	void RenderProxyConectionEntity::CheckConectionToProxyData(RenderProxyMapProxyVec& renderProxyMapProxyVec)
	{
		for (auto& [entity, subMeshIniciesVec] : m_EnitiyToVecIndicesMap)
		{
			for (const IndexSubMesh& subMeshInicies : subMeshIniciesVec)
			{
				RenderProxy& proxy = renderProxyMapProxyVec.AtProxy(subMeshInicies.renderProxyKey, subMeshInicies.proxyIndex);
				RY_CORE_ASSERT(proxy.entity == entity, "We update the whrong Enity in the Proxy!");
				RY_CORE_ASSERT(proxy.subMesh == subMeshInicies.subMesh, "We update the whrong SubMesh in the Proxy!");
			}
		}
	}

	void RenderProxyConectionEntity::Clear()
	{
		m_EnitiyToVecIndicesMap.clear();
	}

	bool RenderProxyConectionEntity::Empty() const
	{
		return m_EnitiyToVecIndicesMap.empty();
	}

	bool RenderProxyConectionEntity::HasEntity(int entity) const
	{
		using ItMap = robin_hood::unordered_flat_map<int, std::vector<IndexSubMesh>>::const_iterator;

		ItMap it = m_EnitiyToVecIndicesMap.find(entity);
		bool result = it != m_EnitiyToVecIndicesMap.end();
		return result;
	}

	uint32_t RenderProxyConectionEntity::GetSubMeshProxyIndexFirst(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh)
	{
		uint32_t count = subMeshIniciesVec.size();
		constexpr uint32_t searchIndex = 0u;

		RY_CORE_ASSERT(searchIndex < count, "Buffer Overflow!");
		const IndexSubMesh& firstSubMesh = subMeshIniciesVec.at(searchIndex);

		if (firstSubMesh.subMesh == subMesh)
			return searchIndex;
		return UINT32_MAX;
	}

	uint32_t RenderProxyConectionEntity::GetSubMeshProxyIndexLast(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh)
	{
		uint32_t count = subMeshIniciesVec.size();
		uint32_t searchIndex = count - 1u;

		RY_CORE_ASSERT(UINT32_MAX != searchIndex, "Buffer Overflow!");
		const IndexSubMesh& lastSubMesh = subMeshIniciesVec.at(searchIndex);

		if (lastSubMesh.subMesh == subMesh)
			return searchIndex;
		return UINT32_MAX;
	}

	uint32_t RenderProxyConectionEntity::GetSubMeshProxyIndexSelfIndex(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh)
	{
		uint32_t count = subMeshIniciesVec.size();

		if (subMesh < count)
		{
			const IndexSubMesh& firstSubMesh = subMeshIniciesVec.at(subMesh);

			if (firstSubMesh.subMesh == subMesh)
				return subMesh;
		}
		return UINT32_MAX;
	}
	
	uint32_t RenderProxyConectionEntity::GetSubMeshProxyIndexOptimalPostion(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh)
	{
		static int64_t hitCount[5] = {
			0ll, 0ll, 0ll, 0ll, 0ll,
		};
		(hitCount[0])++;

		if (subMeshIniciesVec.empty())
			return UINT32_MAX;

		(hitCount[1])++;

		uint32_t foundIndex = GetSubMeshProxyIndexSelfIndex(subMeshIniciesVec, subMesh);
		if (UINT32_MAX != foundIndex)
			return foundIndex;


		(hitCount[2])++;
		foundIndex = GetSubMeshProxyIndexFirst(subMeshIniciesVec, subMesh);
		if (UINT32_MAX != foundIndex)
			return foundIndex;

		(hitCount[3])++;

		foundIndex = GetSubMeshProxyIndexLast(subMeshIniciesVec, subMesh);


		hitCount[4] = UINT32_MAX != foundIndex ? hitCount[4] + 1 : hitCount[4];


		return foundIndex;
	}

	uint32_t RenderProxyConectionEntity::GetSubMeshProxyIndexByBinarySearch(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh)
	{


		uint32_t lowerBound = 0u;
		uint32_t uperBound = subMeshIniciesVec.size();
		uint32_t lowerUperBound = uperBound - lowerBound;
		// Test first the first Ellement 

		while (0u < lowerUperBound)
		{
			lowerUperBound = uperBound - lowerBound;
			uint32_t m = lowerBound + (lowerUperBound / 2u);

			const IndexSubMesh& indexSubMesh = subMeshIniciesVec.at(m);
			uint32_t subMeshValue = indexSubMesh.subMesh;

			if (subMesh == subMeshValue)
			{
				uint32_t proxyIndex = indexSubMesh.proxyIndex;
				return proxyIndex;
			}

			if (subMesh < subMeshValue)
				uperBound = m - 1u;
			else // if(subMesh > subMeshValue)
				lowerBound = m + 1u;

		}
		RY_CORE_ERROR("Out side off bounderies (sub mesh Proxy Index) Not Found!");

		return UINT32_MAX;
	}


	uint32_t RenderProxyConectionEntity::GetSubMeshVecIndexByBinarySearch(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh)
	{
		if (subMeshIniciesVec.empty())
			return UINT32_MAX;


		uint32_t subMeshOptimalIndex = GetSubMeshProxyIndexOptimalPostion(subMeshIniciesVec, subMesh);
		if (UINT32_MAX != subMeshOptimalIndex)
			return subMeshOptimalIndex;

		uint32_t lowerBound = 0u;
		uint32_t uperBound = subMeshIniciesVec.size();
		uint32_t lowerUperBound = uperBound - lowerBound;
		while (0u < lowerUperBound)
			// while (lowerBound < uperBound)
		{
			lowerUperBound = uperBound - lowerBound;
			uint32_t m = lowerBound + (lowerUperBound / 2u);

			const IndexSubMesh& indexSubMesh = subMeshIniciesVec.at(m);
			uint32_t subMeshValue = indexSubMesh.subMesh;

			if (subMesh == subMeshValue)
			{
				return m;
			}

			if (subMesh < subMeshValue)
				uperBound = m - 1u;
			else // if(subMesh > subMeshValue)
				lowerBound = m + 1u;

		}
		RY_CORE_ERROR("Out side off bounderies (IndicesSubMesh Index) Not Found!");
		return UINT32_MAX;
	}

	uint32_t RenderProxyConectionEntity::GetSubMeshVecInsertIndexByBinarySearch(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh)
	{
		uint32_t lowerBound = 0u;
		uint32_t uperBound = subMeshIniciesVec.size() - 1u;
		uint32_t lowerUperBound = uperBound - lowerBound;

		while (0u < lowerUperBound)
		{
			lowerUperBound = uperBound - lowerBound;
			uint32_t m = lowerBound + (lowerUperBound / 2u);

			const IndexSubMesh& indexSubMesh = subMeshIniciesVec.at(m);
			uint32_t subMeshValue = indexSubMesh.subMesh;

			if (subMesh == subMeshValue)
			{
				RY_CORE_FATAL("We found an equal to subMesh:= {} Stores Index:= on index:= {}!", subMesh, indexSubMesh.proxyIndex, m);
				return UINT32_MAX;
			}

			if (subMesh < subMeshValue)
				uperBound = m - 1;
			else // if(subMesh > subMeshValue)
				lowerBound = m + 1;

		}
		uint32_t insertIndex;
		if (subMeshIniciesVec.empty())
		{
			insertIndex = 0u;
		}
		else
		{
			const IndexSubMesh& indexSubMeshUper = subMeshIniciesVec.at(uperBound);
			const IndexSubMesh& indexSubMeshLower = subMeshIniciesVec.at(lowerBound);
			bool uperResult = indexSubMeshUper.subMesh < subMesh;
			bool lowerResult = indexSubMeshLower.subMesh > subMesh;

			insertIndex = lowerResult && uperResult ? uperBound : lowerBound;
		}

		RY_CORE_TRACE("We found an insert Index on index:= {}", lowerBound);
		return UINT32_MAX;
	}

}
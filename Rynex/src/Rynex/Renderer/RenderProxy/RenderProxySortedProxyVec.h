#pragma once
#include <Rynex/Renderer/RenderProxy/Proxy.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchDataStructs.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>
#include <Rynex/Renderer/RenderProxy/RenderProxyMapProxyVec.h>

#define RY_TEST_CHECK_FOR_IDENTY

namespace Rynex {

	class RenderProxySortedProxyVec
	{
	public:
		using IndexCountPair = std::pair<uint32_t, uint32_t>;
	// --- public member funktions --------------------------------------------------------------------------------------------
		RenderProxySortedProxyVec();
		RenderProxySortedProxyVec(const RenderProxySortedProxyVec& r);
		~RenderProxySortedProxyVec();

		uint32_t SubmiteProxy(const RenderProxy& proxy, const glm::mat4& modelMatrix);
		void Remove(uint32_t proxyIndex, RenderProxyKey renderProxyKey);
		void Clear();
		bool Empty() const;
		uint32_t Size() const;

		RenderProxy& AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex);
		const RenderProxy& AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex) const;

		RenderProxy& AtSortedProxy(uint32_t sortedProxyIndex);
		const RenderProxy& AtSortedProxy( uint32_t sortedProxyIndex) const;

		ProxyGroupView GetGroupView(RenderProxyKey renderProxyKey)
		{
			auto& [firstIndex, count] = m_GroupViewsMap.at(renderProxyKey);
			return ProxyGroupView(m_SortedProxyVec.data() + firstIndex, count);
		}

		std::pair<RenderProxyKey, uint32_t> FindeProxy(int entity, uint32_t subMesh);

		template<typename Func>
		void ForeachGroupView(Func func)
		{
			for (auto&[key, groupRange] : m_GroupViewsMap)
			{
				ProxyGroupView groupView(
					m_SortedProxyVec.data() + groupRange.first,
					groupRange.second
				);
				func(key, groupView);
			}
		}

		template<typename Func>
		void ForeachGroupView(Func func) const
		{
			for (const auto& [key, groupRange] : m_GroupViewsMap)
			{
				ProxyGroupViewConst groupView(
					m_SortedProxyVec.data() + groupRange.first,
					groupRange.second
				);
				func(key, groupView);
			}
		}
	private:
		uint32_t InsertProxyIndex(uint32_t proxyAccesIndexInsert);
		uint32_t RemoveProxyIndex(uint32_t proxyAccesIndexRemove);

		uint32_t InsertProxyIntoSortedPorxyVec(const RenderProxy& proxy, const glm::mat4& modelMatrix);
		uint32_t GetProxyIndex(uint32_t proxyAccesIndex) const;
		void RemoveProxyFromSortedPorxyVec(uint32_t proxyIndexRemove);
		void RebuildSortedIndices();

		uint32_t GetIndexFromFreeList();
		void AddFreeList(uint32_t proxyAccesIndex);

		void InsertProxyGroupView(uint32_t proxyAccesIndexInsert, RenderProxyKey renderProxyKey);
		void RemoveProxyGroupView(uint32_t proxyAccesIndexInsert, RenderProxyKey renderProxyKey);

		void CheckColsionGroupView();
	// --- private member funktions -------------------------------------------------------------------------------------------
		std::vector<RenderProxy> m_SortedProxyVec;
		std::vector<uint32_t> m_ProxyInidicesVec;
		std::vector<uint32_t> m_FreeListVec;
		std::map<RenderProxyKey, IndexCountPair> m_GroupViewsMap;
		
	};


}

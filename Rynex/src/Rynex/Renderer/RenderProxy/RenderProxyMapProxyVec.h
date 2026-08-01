#pragma once

#include <Rynex/Renderer/RenderProxy/Proxy.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchDataStructs.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>

namespace Rynex {

	class RenderProxyGroup
	{
	public:
		RenderProxyGroup();
		RenderProxyGroup(const RenderProxyGroup&) = default;
		RenderProxyGroup(const RenderMeshBatch& renderMeshBatch);
		~RenderProxyGroup();

		uint32_t Add(const RenderProxy& proxy);

		void Remove(uint32_t stableIndex);

		RenderProxy& AtProxy(uint32_t stableIndex);
		const RenderProxy& AtProxy(uint32_t stableIndex) const;

		RenderProxy& AtSortedProxy(uint32_t stableIndex);
		const RenderProxy& AtSortedProxy(uint32_t stableIndex) const;



		bool IsValid(uint32_t stableIndex) const;

		bool     Empty()      const { return m_ActiveCount == 0; }
		uint32_t ActiveSize() const { return m_ActiveCount; }
		uint32_t Capacity()   const { return static_cast<uint32_t>(m_ProxyVec.size()); }

		ProxyGroupView GetView() { return ProxyGroupView{ m_ProxyVec.data(), static_cast<uint32_t>(m_ProxyVec.size()), m_MeshBatch }; }
		ProxyGroupViewConst GetView() const { return ProxyGroupViewConst{ m_ProxyVec.data(), static_cast<uint32_t>(m_ProxyVec.size()), m_MeshBatch }; }
	
		uint32_t GetProxyIndex(uint32_t stableIndex) const;
	private:
		uint32_t InsertProxyIndex(uint32_t proxyAccesIndexInsert);
		uint32_t RemoveProxyIndex(uint32_t proxyAccesIndexRemove);

		uint32_t GetIndexFromFreeList();
		void AddFreeList(uint32_t proxyAccesIndex);
	// --- private member varibles --------------------------------------------------------------------------------------------

		RenderMeshBatch			 m_MeshBatch;
		std::vector<RenderProxy> m_ProxyVec;
		std::vector<uint32_t>    m_ProxyInidicesVec;
		std::vector<uint32_t>    m_FreeListVec;
		uint32_t                 m_ActiveCount;
		
	};


	class RenderProxyMapProxyVec
	{
	public:
		using Map = std::map<RenderProxyKey, RenderProxyGroup>;
		using Iterator = typename Map::iterator;
		using ConstIterator = typename Map::const_iterator;

		RenderProxyMapProxyVec();
		RenderProxyMapProxyVec(const RenderProxyMapProxyVec& r);
		~RenderProxyMapProxyVec();

		uint32_t SubmiteProxy(const RenderProxy& proxy, const glm::mat4& modelMatrix);
		void Remove(uint32_t proxyIndex, RenderProxyKey renderProxyKey);
		void Clear();
		bool Empty() const;
		uint32_t Size() const;

		RenderProxy& AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex);
		const RenderProxy& AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex) const;


		RenderProxy& AtSortedProxy(RenderProxyKey renderProxyKey, uint32_t sortedProxyIndex);
		const RenderProxy& AtSortedProxy(RenderProxyKey renderProxyKey, uint32_t sortedProxyIndex) const;


		std::pair<RenderProxyKey, uint32_t> FindeProxy(int entity, uint32_t subMesh) const;

		RenderProxyGroup* GetGroup(RenderProxyKey key);
		const RenderProxyGroup* GetGroup(RenderProxyKey key) const;

		Iterator begin()  { return m_GroupMap.begin(); }
		ConstIterator begin() const { return m_GroupMap.begin(); }

		Iterator end() { return m_GroupMap.end(); }
		ConstIterator end() const { return m_GroupMap.end(); }

		template<typename Func>
		void ForeachGroupView(Func func) const
		{
			for (const auto&[renderProxyKey, renderProxyGroup]  : m_GroupMap)
			{
				func(renderProxyKey, renderProxyGroup.GetView());
			}
		}

		template<typename Func>
		void ForeachGroupView(Func func)
		{
			for (auto& [renderProxyKey, renderProxyGroup] : m_GroupMap)
			{
				func(renderProxyKey, renderProxyGroup.GetView());
			}
		}
	private:
	
	// --- private member varibles --------------------------------------------------------------------------------------------
		Map m_GroupMap;
	};

}
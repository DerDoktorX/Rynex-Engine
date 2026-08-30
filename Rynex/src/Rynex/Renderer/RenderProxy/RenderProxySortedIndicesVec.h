#pragma once
#include <Rynex/Renderer/RenderProxy/Proxy.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchDataStructs.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>
#define RY_TEST_CHECK_FOR_IDENTY

namespace Rynex {
	
	class RenderProxySortedIndicesVec
	{
	public:
		RenderProxySortedIndicesVec();
		RenderProxySortedIndicesVec(const RenderProxySortedIndicesVec& r);
		~RenderProxySortedIndicesVec();

		uint32_t SubmiteProxy(const RenderProxy& proxy, const glm::mat4& modelMatrix);
		void Remove(uint32_t proxyIndex, RenderProxyKey renderProxyKey);
		void Clear();
		bool Empty() const;
		uint32_t Size() const;

		RenderProxy& AtProxy(uint32_t proxyIndex);
		const RenderProxy& AtProxy(uint32_t proxyIndex) const;


		RenderProxy& AtSortedProxy(uint32_t sortedProxyIndex);
		const RenderProxy& AtSortedProxy(uint32_t sortedProxyIndex) const;



		uint32_t FindeProxy(RenderProxyKey renderProxyKey, int entity, uint32_t subMesh);
	private:



		void InsertIntoSortedIndices(uint32_t proxyIndex);

		void RemoveFromSortedIndices(uint32_t proxyIndexRemove, RenderProxyKey renderProxyKey);
		void RebuildSortedIndices();

		uint32_t GetIndexFromFreeList();
		void AddFreeList(uint32_t proxyIndex);
	private:
		std::vector<RenderProxy> m_ProxyVec;

		std::vector<uint32_t> m_SortedProxyInidicesVec;
		std::vector<uint32_t> m_FreeListVec;
	};

}
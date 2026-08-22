#pragma once
#include <Rynex/Renderer/RenderProxy/RenderProxySortedIndicesVec.h>
#include <Rynex/Renderer/RenderProxy/RenderProxySortedProxyVec.h>
#define ROBINE_HOOD_HASH
#define RY_SORT_KEY

namespace Rynex {
	class RenderProxyConectionEntity
	{
	public:
		struct IndexSubMesh
		{
			uint32_t proxyIndex;
			uint32_t subMesh;
			RenderProxyKey renderProxyKey;

			bool operator==(uint32_t subMesh) const
			{
				return subMesh == this->subMesh;
			}
		};
	public:
		RenderProxyConectionEntity();
		RenderProxyConectionEntity(const RenderProxyConectionEntity&) = default;
		RenderProxyConectionEntity(RenderProxyConectionEntity&&) noexcept = delete;

		const IndexSubMesh& GetIndexSubMeshFromMap(int entity, uint32_t subMesh) const;
		uint32_t GetProxyIndexFormMap(int entity, uint32_t subMesh);
		const std::vector<IndexSubMesh>& GetIndicesSubMeshVecFromMap(int entity) const;

		std::pair<uint32_t, RenderProxyKey> RemoveProxyIndexFromMap(int entity, uint32_t subMesh);

		std::vector<IndexSubMesh> RemoveProxyIndexVecFromMap(int entity);

		void AddProxyIndexToMap(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, RenderProxySortedProxyVec& renderProxySortedProxyVec);
		void AddProxyIndexToMap(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, RenderProxySortedIndicesVec& renderProxySortedProxyVec);
		void AddProxyIndexToMap(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, RenderProxyMapProxyVec& renderProxyMapProxyVec);

		void CheckConectionToProxyData(RenderProxySortedProxyVec& renderProxySortedProxyVec);
		void CheckConectionToProxyData(RenderProxySortedIndicesVec& renderProxySortedProxyVec);
		void CheckConectionToProxyData(RenderProxyMapProxyVec& renderProxyMapProxyVec);

		void Clear();
		bool Empty() const;
		bool HasEntity(int entity)const;

	private:
		// runtime: O( 1 )
		static uint32_t GetSubMeshProxyIndexFirst(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh);
		// runtime: O( 1 )
		static uint32_t GetSubMeshProxyIndexLast(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh);
		// runtime: O( 1 )
		static uint32_t GetSubMeshProxyIndexSelfIndex(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh);

		// runtime: O( 1 ) 
		static uint32_t GetSubMeshProxyIndexOptimalPostion(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh);

		// runtime: O( log(n) )
		static uint32_t GetSubMeshProxyIndexByBinarySearch(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh);
		// runtime: O( log(n) )
		static uint32_t GetSubMeshVecIndexByBinarySearch(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh);
		// runtime: O( log(n) )
		static uint32_t GetSubMeshVecInsertIndexByBinarySearch(const std::vector<IndexSubMesh>& subMeshIniciesVec, uint32_t subMesh);
	private:
		HashMapFlat<int, std::vector<IndexSubMesh>> m_EnitiyToVecIndicesMap;
	};
}

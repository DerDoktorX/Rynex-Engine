#pragma once
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/Mesh/Mesh.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <robin_hood.h>

namespace Rynex {
// #define RY_BATCH_KEY_HASH
// #define	RY_BATCH_KEY_HASH_FUNC
	class RenderPassDrawResoucs;

	class Batch3DKey
	{
	public:
		Batch3DKey();
		Batch3DKey(const Batch3DKey&) = default;

		Batch3DKey(const Ref<Material>& materiel, const Ref<MeshSingle>& mesh, const BufferLayout& outPut, int lodTier);
		~Batch3DKey() = default;

		operator uint64_t() const;
		uint64_t GetHash() const;
	private:
		static uint64_t Mix64(uint64_t value);
		static uint64_t GetMaterielHash(uint64_t hash, uint64_t& bitsUsed, const Ref<Material>& materiel);
		static uint64_t GetMeshHash(uint64_t hash, uint64_t& bitsUsed, const Ref<MeshSingle>& mesh);
		static uint64_t GetShaderHash(uint64_t hash, uint64_t& bitsUsed, const Ref<Shader>& shader);

	private:
		Ref<Material> m_Materiel;
		Ref<MeshSingle> m_Mesh;
		Ref<Shader> m_Shader;
		int m_DrawSpec;
		int m_RenderMode;
		int m_LOD_Level;
	private:
		friend RenderPassDrawResoucs;
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(::Rynex::Batch3DKey, bool, == );
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(::Rynex::Batch3DKey, bool, != );
	};

	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch3DKey, == , &&, m_Materiel, m_Mesh, m_Shader, m_DrawSpec, m_RenderMode, m_LOD_Level);
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch3DKey, != , ||, m_Materiel, m_Mesh, m_Shader, m_DrawSpec, m_RenderMode, m_LOD_Level);

}

namespace robin_hood {
	template<>
	struct hash<Rynex::Batch3DKey>
	{
		std::size_t operator()(const Rynex::Batch3DKey& batch3DKey) const
		{
			std::size_t hash = batch3DKey.GetHash();
			return hash;
		}
	};
}
namespace std {

	template<>
	struct hash<Rynex::Batch3DKey>
	{
		std::size_t operator()(const Rynex::Batch3DKey& batch3DKey) const
		{
			std::size_t hash = batch3DKey.GetHash();
			return hash;
		}
	};

}
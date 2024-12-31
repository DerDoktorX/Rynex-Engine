#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/Objects/Mesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;


namespace Rynex {


	class Model : public Asset
	{
	public:
		Model(const std::string& path);
		Model(std::vector<Ref<Mesh>>& meshes, std::vector<MeshRootData>& meshRootDatas);
		Model() = default;
		~Model() = default;

		virtual AssetType GetType() const
		{
			return AssetType::Mesh;
		}

		const std::vector<Ref<Mesh>>& GetMeshes() { std::lock_guard<std::mutex> lock(m_Accese); return m_Meshes; }
		const Ref<Mesh>& GetMesh(uint32_t index) { std::lock_guard<std::mutex> lock(m_Accese); return m_Meshes[index]; }

		const std::vector<MeshRootData>& GetRootDatas() { std::lock_guard<std::mutex> lock(m_Accese); return m_MeshRootDatas; }
		const MeshRootData& GetRootData(uint32_t index) { std::lock_guard<std::mutex> lock(m_Accese); return m_MeshRootDatas[index]; }

		void SetMeshes(std::vector<Ref<Mesh>>& meshes) { std::lock_guard<std::mutex> lock(m_Accese); m_Meshes.clear(); m_Meshes = meshes; }
		void PushMesh(Ref<Mesh>& mesh) { std::lock_guard<std::mutex> lock(m_Accese); m_Meshes.push_back(mesh); }

		void Draw(const Ref<Shader>& shader);
	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		Ref<Mesh> ProcessMesh(aiMesh* node, const aiScene* scene);
		std::vector<MeshTexture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& typeName);
		Ref<Texture> TextureFromFile(const std::filesystem::path& toFile, bool gamma = false);


	private:
		std::vector<Ref<Mesh>> m_Meshes;
		std::vector<MeshRootData> m_MeshRootDatas;
		std::mutex m_Accese;
	};

}


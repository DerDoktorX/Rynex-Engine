#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/Objects/Mesh.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

#define RY_MODEL_NODE 1
namespace Rynex {

	struct MeshRenderData
	{
		Ref<Mesh> MeshR = nullptr;
		glm::mat4 LocaleMatrix = glm::mat4(1.0);
		glm::mat4 GlobleMatrix = glm::mat4(1.0);
		Ref<Material> Materiel = nullptr;

		MeshRenderData() = default;
		MeshRenderData(const MeshRenderData&) = default;

		MeshRenderData(Ref<Mesh> mesh, const glm::mat4& matrix)
			: MeshR(mesh), LocaleMatrix(matrix), GlobleMatrix(matrix)
		{ }

		void UpdateMatrix(glm::mat4 parentMat4)
		{
			GlobleMatrix = LocaleMatrix * parentMat4;
		}
	};

	struct NodeData
	{
		std::vector<Ref<Mesh>> Meshes;
		glm::mat4 Matrix;
		std::string Name;
		std::vector<int> Children;
		int Parent = -1;


	};

	class Model : public Asset
	{
	public:
		Model(const std::string& path);
#if RY_MODEL_NODE
		Model(std::vector<NodeData>&& nodeData);
#else
		Model(std::vector<Ref<Mesh>>& meshes, std::vector<MeshRootData>& meshRootDatas);
#endif
		
		Model() = default;
		~Model() = default;

		virtual AssetType GetType() const
		{
			return AssetType::Mesh;
		}
#if RY_MODEL_NODE
		const std::vector<NodeData>& GetNodes() { std::lock_guard<std::mutex> lock(m_Accese); return m_RootNode; }
#else
		const std::vector<Ref<Mesh>>& GetMeshes() { std::lock_guard<std::mutex> lock(m_Accese); return m_Meshes; }
		const Ref<Mesh>& GetMesh(uint32_t index) { std::lock_guard<std::mutex> lock(m_Accese); return m_Meshes[index]; }

		const std::vector<MeshRootData>& GetRootDatas() { std::lock_guard<std::mutex> lock(m_Accese); return m_MeshRootDatas; }
		const MeshRootData& GetRootData(uint32_t index) { std::lock_guard<std::mutex> lock(m_Accese); return m_MeshRootDatas[index]; }

		void SetMeshes(std::vector<Ref<Mesh>>& meshes) { std::lock_guard<std::mutex> lock(m_Accese); m_Meshes.clear(); m_Meshes = meshes; }
		void PushMesh(Ref<Mesh>& mesh) { std::lock_guard<std::mutex> lock(m_Accese); m_Meshes.push_back(mesh); }

		std::vector<MeshRenderData>&& GetMeshRenderData();

		void Draw(const Ref<Shader>& shader);
#endif

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		Ref<Mesh> ProcessMesh(aiMesh* node, const aiScene* scene);
		std::vector<MeshTexture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& typeName);
		Ref<Texture> TextureFromFile(const std::filesystem::path& toFile, bool gamma = false);


	private:
#if RY_MODEL_NODE
		std::vector<NodeData> m_RootNode;
#else
		std::vector<Ref<Mesh>> m_Meshes;
		std::vector<MeshRootData> m_MeshRootDatas;
#endif
		std::mutex m_Accese;
	};

}


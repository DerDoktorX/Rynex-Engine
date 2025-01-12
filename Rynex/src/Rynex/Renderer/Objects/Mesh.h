#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Renderer/Materials/Material.h"

namespace Rynex {

	class Mesh;

	enum class MeshMode
	{
		None = 0,
		Statitic,
		Dynamic
	};

	struct MeshVertex
	{
		glm::vec3 Postion;
		glm::vec2 TexCorrds; // UV
		glm::vec3 Normale;


		MeshVertex() = default;
		MeshVertex(const MeshVertex&) = default;
		MeshVertex(MeshVertex&&) = default;

		MeshVertex(const glm::vec3& postion, const glm::vec3& normale, const glm::vec2& texCorrds)
			: Postion(postion), Normale(normale), TexCorrds(texCorrds)
		{ }

		MeshVertex(glm::vec3&& postion, glm::vec3&& normale, glm::vec2&& texCorrds)
			: Postion(std::move(postion)), Normale(std::move(normale)), TexCorrds(std::move(texCorrds))
		{ }

	};

	struct MeshRootData
	{
		glm::mat4 NodeMatrix;
		std::string NodeName = "Default-Name";
		std::vector<Ref<Mesh>> Meshes;

		MeshRootData() = default;
		MeshRootData(const MeshRootData&) = default;
		MeshRootData(MeshRootData&&) = default;

		MeshRootData(const glm::mat4& nodeMatrix)
			: NodeMatrix(nodeMatrix)
		{ }
		MeshRootData(const glm::mat4& nodeMatrix, std::string& nodeName)
			: NodeMatrix(nodeMatrix), NodeName(nodeName)
		{ }

		MeshRootData(const glm::mat4& nodeMatrix, const std::string& nodeName, const std::vector<Ref<Mesh>>& meshes)
			: NodeMatrix(nodeMatrix), NodeName(nodeName), Meshes(meshes)
		{ }

		MeshRootData(const glm::mat4&& nodeMatrix, const std::string& nodeName, std::vector<Ref<Mesh>>&& meshes)
			: NodeMatrix(nodeMatrix), NodeName(nodeName), Meshes(std::move(meshes))
		{ }
	};

	struct MeshTexture
	{
		Ref<Texture> TextureResur;
		std::string Type;
		std::filesystem::path Path;

		MeshTexture() = default;
		MeshTexture(const MeshTexture&) = default;

		MeshTexture(const std::string& type, const std::filesystem::path& path)
			: TextureResur(nullptr), Type(type), Path(path)
		{
		}

		MeshTexture(Ref<Texture> texture, const std::string& type, const std::filesystem::path& path)
			: TextureResur(texture), Type(type), Path(path)
		{  }
	};

	class Mesh : public Asset
	{
	public:
		Mesh() = default;
		Mesh(std::vector<MeshVertex>&& meshVertex, std::vector<unsigned int>&& meshIndex, std::vector<MeshTexture>&& meshTexures, bool async = false);
		Mesh(std::vector<MeshVertex>&& meshVertex, std::vector<unsigned int>&& meshIndex, std::vector<MeshTexture>&& meshTexures, const Ref<Material>& material, bool async = false);
		Mesh(Mesh&&) = default;
		~Mesh();

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		const MeshTexture& GetTexture(uint32_t index) const { return m_Textures[index]; }
		const std::vector<MeshTexture>& GetTextures() const { return m_Textures; }


		void SetMateriel(const Ref<Material>& materiel) { m_Material = materiel; }
		const Ref<Material>& GetMateriel() const { return m_Material; }
		

		void InitAsync();
		virtual AssetType GetType() const
		{
			return AssetType::Mesh;
		}

		void OnDraw(const Ref<Shader>& shader);

		bool IsRady()
		{
			return m_VertexArray && m_VertexBuffer && m_IndexBuffer && m_Material->IsRady();
		}
	private:
		Ref<VertexArray> m_VertexArray; 
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Material> m_Material;
		std::vector<MeshTexture> m_Textures;
	};
}

#if 0
namespace std {

	template<>
	struct hash<Rynex::Ref<Rynex::Mesh>>
	{
		std::size_t operator()(const Rynex::Ref<Rynex::Mesh>& mesh) const
		{
			return (std::size_t)mesh.get();
		}
	};

	template<>
	struct hash<Rynex::Weak<Rynex::Mesh>>
	{
		std::size_t operator()(const Rynex::Weak<Rynex::Mesh>& mesh) const
		{
			if (Rynex::Ref<Rynex::Mesh> meshRef = mesh.lock())
				return (std::size_t)meshRef.get();
			return 0;
		}
	};

}
#endif
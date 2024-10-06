#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/API/Texture.h"

namespace Rynex {

	struct MeshVertex
	{
		glm::vec3 Postion;
		glm::vec2 TexCorrds; // UV
		glm::vec3 Normale;
		

		MeshVertex() = default;
		MeshVertex(const MeshVertex&) = default;

		MeshVertex(glm::vec3 postion, glm::vec3 normale, glm::vec2 texCorrds)
			: Postion(postion), Normale(normale), TexCorrds(texCorrds)
		{ }

	};

	struct MeshRootData
	{
		glm::mat<4, 4, float> NodeMatrix;
		std::string NodeName;
		MeshRootData() = default;
		MeshRootData(const MeshRootData&) = default;

		MeshRootData(glm::mat<4, 4, float> nodeMatrix)
			: NodeMatrix(nodeMatrix)
		{ }
		MeshRootData(glm::mat<4, 4, float> nodeMatrix, std::string& nodeName)
			: NodeMatrix(nodeMatrix), NodeName(nodeName)
		{ }
	};

	struct MeshTexture
	{
		Ref<Texture> TextureResur;
		std::string Type;
		std::string Path;

		MeshTexture() = default;
		MeshTexture(const MeshTexture&) = default;

		MeshTexture(Ref<Texture> texture, const std::string& type, const std::string& path)
			: TextureResur(texture), Type(type), Path(path)
		{  }
	};

	class Mesh : public Asset
	{
	public:
		Mesh() = default;
		Mesh(std::vector<MeshVertex>& meshVertex, std::vector<unsigned int>& meshIndex, std::vector<MeshTexture>& meshTexures);
		Mesh(Mesh&&) = default;
		~Mesh();

		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray;}
		const MeshTexture& GetTexture(uint32_t index) const { return m_Textures[index]; }
		const std::vector<MeshTexture>& GetTextures() const { return m_Textures; }

		virtual AssetType GetType() const
		{
			return AssetType::Mesh;
		}

		void OnDraw(const Ref<Shader>& shader);
	private:
		void Create();

	private:
		Ref<VertexArray> m_VertexArray;
		std::vector<MeshTexture> m_Textures;
	};
}


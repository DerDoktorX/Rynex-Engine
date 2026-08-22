#pragma once
#include "Rynex/Project/Project.h"
#include "Rynex/Renderer/API/Buffer.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace YAML {
	class Emitter;
	class Node;

	template <typename T>
	struct convert;

#pragma region OperatorDeclarion
#pragma region type_opertor
	Emitter& operator<<(Emitter& out, const std::string_view& v);
	Emitter& operator<<(Emitter& out, const std::filesystem::path& v);
	Emitter& operator<<(Emitter& out, const Rynex::UUID& handle);
#pragma endregion



	Emitter& operator<<(Emitter& out, const Rynex::SceneCamera::ProjectionType& projectionType);
	Emitter& operator<<(Emitter& out, const Rynex::TextureWrappingMode& textureWrappingMode);
	Emitter& operator<<(Emitter& out, const Rynex::TextureFilteringMode& textureFilteringMode);
	Emitter& operator<<(Emitter& out, const Rynex::TextureCompareModes& textureCompareModes);
	Emitter& operator<<(Emitter& out, const Rynex::TextureFormat& textureFormat);
	Emitter& operator<<(Emitter& out, const Rynex::TextureTarget& textureTarget);
	Emitter& operator<<(Emitter& out, const Rynex::ShaderDataType& shaderDataType);
	Emitter& operator<<(Emitter& out, const Rynex::ShaderType::ShaderType& shaderType);
	Emitter& operator<<(Emitter& out, const Rynex::RenderMode::RenderMode& renderMode);
	Emitter& operator<<(Emitter& out, const Rynex::BufferType& bufferType);


	Emitter& operator<<(Emitter& out, const Rynex::TextureWrappingSpecification& textureWrapping);
	Emitter& operator<<(Emitter& out, const Rynex::FramebufferTextureSpecification& framebufferTextureSpecification);
	Emitter& operator<<(Emitter& out, const Rynex::SceneCamera& sceneCamera);
	Emitter& operator<<(Emitter& out, const Rynex::FramebufferAttachmentSpecification& framebufferAttachmentSpecification);
	Emitter& operator<<(Emitter& out, const Rynex::FramebufferSpecification& specification);


	Emitter& operator<<(Emitter& out, const Rynex::BufferElement& element);
	Emitter& operator<<(Emitter& out, const Rynex::BufferLayout& layout);

#pragma endregion


#pragma region glm_operator

#pragma region glm_FLOAT_operator

	Emitter& operator<<(Emitter& out, const glm::vec2& v);
	Emitter& operator<<(Emitter& out, const glm::vec3& v);
	Emitter& operator<<(Emitter& out, const glm::vec4& v);
	Emitter& operator<<(Emitter& out, const glm::mat3& v);
	Emitter& operator<<(Emitter& out, const glm::mat4& v);

#pragma endregion

#pragma region glm_DOUBLE_operator

	Emitter& operator<<(Emitter& out, const glm::dvec2& v);
	Emitter& operator<<(Emitter& out, const glm::dvec3& v);
	Emitter& operator<<(Emitter& out, const glm::dvec4& v);
	Emitter& operator<<(Emitter& out, const glm::dmat3& v);
	Emitter& operator<<(Emitter& out, const glm::dmat4& v);

#pragma endregion

#pragma region glm_INT_operator
	Emitter& operator<<(Emitter& out, const glm::ivec2& v);
	Emitter& operator<<(Emitter& out, const glm::ivec3& v);
	Emitter& operator<<(Emitter& out, const glm::ivec4& v);
	Emitter& operator<<(Emitter& out, const glm::imat3x3& v);
	Emitter& operator<<(Emitter& out, const glm::imat4x4& v);
#pragma endregion

#pragma region glm_UINT_operator
	Emitter& operator<<(Emitter& out, const glm::uvec2& v);
	Emitter& operator<<(Emitter& out, const glm::uvec3& v);
	Emitter& operator<<(Emitter& out, const glm::uvec4& v);
	Emitter& operator<<(Emitter& out, const glm::umat3x3& v);
	Emitter& operator<<(Emitter& out, const glm::umat3x3& v);
#pragma endregion

#pragma endregion


#pragma region DoceodeFunc


#pragma region CoverterOperators

	template<>
	struct convert<Rynex::SceneCamera::ProjectionType>
	{
		static bool decode(const Node& node, Rynex::SceneCamera::ProjectionType& projectionType);
	};

	template<>
	struct convert<Rynex::TextureWrappingMode>
	{
		static bool decode(const Node& node, Rynex::TextureWrappingMode& textureWrappingMode);
	};

	template<>
	struct convert<Rynex::TextureFilteringMode>
	{
		static bool decode(const Node& node, Rynex::TextureFilteringMode& textureFilteringMode);
	};

	template<>
	struct convert<Rynex::TextureCompareModes>
	{
		static bool decode(const Node& node, Rynex::TextureCompareModes& textureCompareModes);
	};

	template<>
	struct convert<Rynex::TextureFormat>
	{
		static bool decode(const Node& node, Rynex::TextureFormat& textureFormat);
	};

	template<>
	struct convert<Rynex::TextureTarget>
	{
		static bool decode(const Node& node, Rynex::TextureTarget& textureFormat);
	};

	template<>
	struct convert<Rynex::ShaderDataType>
	{
		static bool decode(const Node& node, Rynex::ShaderDataType& shaderDataType);
	};

#pragma endregion

	template<>
	struct convert<Rynex::TextureWrappingSpecification>
	{
		static bool decode(const Node& node, Rynex::TextureWrappingSpecification& textureWrapping);
	};


	template<>
	struct convert<Rynex::FramebufferTextureSpecification>
	{
		static bool decode(const Node& node, Rynex::FramebufferTextureSpecification& framebufferTextureSpecification);
	};

	template<>
	struct convert<Rynex::SceneCamera>
	{
		static bool decode(const Node& node, Rynex::SceneCamera& sceneCamera);
	};

	template<>
	struct convert<Rynex::FramebufferAttachmentSpecification>
	{
		static bool decode(const Node& node, Rynex::FramebufferAttachmentSpecification& framebufferAttachmentSpecification);
	};

	template<>
	struct convert<Rynex::FramebufferSpecification>
	{
		static bool decode(const Node& node, Rynex::FramebufferSpecification& specification);
	};

	template<>
	struct convert<Rynex::BufferLayout>
	{

		static bool decode(const Node& node, Rynex::BufferLayout& layout);
	};

	template<>
	struct convert<Rynex::BufferElement>
	{
		// static Node encode(const Rynex::BufferElement& element);
		static bool decode(const Node& node, Rynex::BufferElement& element);
	};


#pragma region type_Decode

	template<>
	struct convert<std::filesystem::path>
	{
		// static Node encode(const std::filesystem::path& rhs);
		static bool decode(const Node& node, std::filesystem::path& rhs);
	};

#pragma endregion


#pragma region glm_Decode

#pragma region glm_FLOAT_Decode
	template<>
	struct convert<glm::vec2>
	{
		// static Node encode(const glm::vec2& rhs);
		static bool decode(const Node& node, glm::vec2& rhs);
	};

	template<>
	struct convert<glm::vec3>
	{
		// static Node encode(const glm::vec3& rhs);
		static bool decode(const Node& node, glm::vec3& rhs);
	};

	template<>
	struct convert<glm::vec4>
	{
		// static Node encode(const glm::vec4& rhs);
		static bool decode(const Node& node, glm::vec4& rhs);
	};

	template<>
	struct convert<glm::mat3>
	{
		// static Node encode(const glm::mat3& rhs);
		static bool decode(const Node& node, glm::mat3& rhs);
	};

	template<>
	struct convert<glm::mat4>
	{
		// static Node encode(const glm::mat4& rhs);
		static bool decode(const Node& node, glm::mat4& rhs);
	};
#pragma endregion

#pragma region glm_DOUBLE_Decode

	template<>
	struct convert<glm::dvec2>
	{
		// static Node encode(const glm::dvec2& rhs);
		static bool decode(const Node& node, glm::dvec2& rhs);
	};

	template<>
	struct convert<glm::dvec3>
	{
		// static Node encode(const glm::dvec3& rhs);
		static bool decode(const Node& node, glm::dvec3& rhs);
	};

	template<>
	struct convert<glm::dvec4>
	{
		// static Node encode(const glm::dvec4& rhs);
		static bool decode(const Node& node, glm::dvec4& rhs);
	};

	template<>
	struct convert<glm::dmat3>
	{
		// static Node encode(const glm::dmat3& rhs);
		static bool decode(const Node& node, glm::dmat3& rhs);
	};

	template<>
	struct convert<glm::dmat4>
	{
		// static Node encode(const glm::dmat4& rhs);
		static bool decode(const Node& node, glm::dmat4& rhs);
	};
#pragma endregion

#pragma region glm_INT_Decode

	template<>
	struct convert<glm::ivec2>
	{
		// static Node encode(const glm::ivec2& rhs);
		static bool decode(const Node& node, glm::ivec2& rhs);
	};

	template<>
	struct convert<glm::ivec3>
	{
		// static Node encode(const glm::ivec3& rhs);
		static bool decode(const Node& node, glm::ivec3& rhs);
	};

	template<>
	struct convert<glm::ivec4>
	{
		// static Node encode(const glm::ivec4& rhs);
		static bool decode(const Node& node, glm::ivec4& rhs);
	};

	template<>
	struct convert<glm::imat3x3>
	{
		// static Node encode(const glm::imat3x3& rhs);
		static bool decode(const Node& node, glm::imat3x3& rhs);
	};

	template<>
	struct convert<glm::imat4x4>
	{
		// static Node encode(const glm::imat4x4& rhs);
		static bool decode(const Node& node, glm::imat4x4& rhs);
	};

#pragma endregion

#pragma region glm_UINT_Decode

	template<>
	struct convert<glm::uvec2>
	{
		// static Node encode(const glm::uvec2& rhs);
		static bool decode(const Node& node, glm::uvec2& rhs);
	};

	template<>
	struct convert<glm::uvec3>
	{
		// static Node encode(const glm::uvec3& rhs);
		static bool decode(const Node& node, glm::uvec3& rhs);
	};

	template<>
	struct convert<glm::uvec4>
	{
		// static Node encode(const glm::uvec4& rhs);
		static bool decode(const Node& node, glm::uvec4& rhs);
	};

	template<>
	struct convert<glm::umat3x3>
	{
		// static Node encode(const glm::umat3x3& rhs);
		static bool decode(const Node& node, glm::umat3x3& rhs);
	};

	template<>
	struct convert<glm::umat4x4>
	{
		// static Node encode(const glm::umat4x4& rhs);
		static bool decode(const Node& node, glm::umat4x4& rhs);
	};
#pragma endregion

#pragma endregion

#pragma endregion

}

namespace Serializer {
	void AssetFormate(YAML::Emitter& out, Rynex::AssetHandle handle);
	void AssetFormate(YAML::Emitter& out, const std::string& name, Rynex::AssetHandle handle);
}

namespace Deserialize {


	template<typename T>
	static bool AssetFormate(YAML::Node& nodeE, Rynex::Ref<T>* entityC, bool async);


}
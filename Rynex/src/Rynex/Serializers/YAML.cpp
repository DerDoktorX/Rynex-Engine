#include "rypch.h"
#include "YAML.h"

#include <Rynex/Asset/Base/AssetManager.h>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

#include <magic_enum.hpp>


namespace YAML {


	Emitter& operator<<(Emitter& out, const std::string_view& v)
	{
		out << std::string(v.data(), v.size());
		return out;
	}

	Emitter& operator<<(Emitter& out, const std::filesystem::path& v)
	{
		out << v.string();
		return out;
	}
	Emitter& operator<<(Emitter& out, const Rynex::UUID& handle)
	{
		uint64_t v = handle;
		out << v;
		return out;
	}

#pragma region glm_Operator

#pragma region glm_FLOAT_Operator
	Emitter& operator<<(Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	Emitter& operator<<(Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat3& v)
	{

		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i] << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat4& v)
	{
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			
			out << v[i];
		}
		out << YAML::EndSeq;
		return out;
	}
#pragma endregion


#pragma region glm_DOUBLE_Operator
	Emitter& operator<<(Emitter& out, const glm::dvec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::dvec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	Emitter& operator<<(Emitter& out, const glm::dvec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::dmat3& v)
	{

		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i] << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::dmat4& v)
	{
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			out << Flow;
			out << YAML::BeginSeq << v[i] << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}
#pragma endregion

#pragma region glm_INT_Operator
	Emitter& operator<<(Emitter& out, const glm::ivec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::ivec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::ivec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}


	Emitter& operator<<(Emitter& out, const glm::imat3x3& v)
	{

		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i] << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::imat4x4& v)
	{
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i] << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}
#pragma endregion

#pragma region glm_UINT_Operator

	Emitter& operator<<(Emitter& out, const glm::uvec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::uvec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::uvec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}


	Emitter& operator<<(Emitter& out, const glm::umat3x3& v)
	{

		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i] << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::umat4x4& v)
	{
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i] << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}
#pragma endregion
#pragma endregion



	Emitter& operator<<(Emitter& out, const Rynex::BufferElement& element)
	{
		std::string_view view = magic_enum::enum_name(element.type);


		out << YAML::Flow;
		out << YAML::BeginSeq;

		out << YAML::Value << magic_enum::enum_name(element.type);
		out << YAML::Value << element.name;
		if (element.normilized)
			out << YAML::Value << element.normilized;

		out << YAML::EndSeq;

	

		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::BufferLayout& layout)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "AutoCompress" << YAML::Value << layout.AutoCompress();
		out << YAML::Key << "InstastancIncreas" << YAML::Value << layout.GetInstastancIncreas();

		out << YAML::Key << "Elements";
		out << YAML::BeginSeq;
		for (const Rynex::BufferElement& element : layout)
		{
			out << element;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		return out;
	}





	Emitter& operator<<(Emitter& out, const Rynex::SceneCamera::ProjectionType& projectionType)
	{
		out << YAML::Value << magic_enum::enum_name(projectionType);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::TextureWrappingMode& textureWrappingMode)
	{
		out << YAML::Value << magic_enum::enum_name(textureWrappingMode);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::TextureFilteringMode& textureFilteringMode)
	{
		out << YAML::Value << magic_enum::enum_name(textureFilteringMode);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::TextureCompareModes& textureCompareModes)
	{
		out << YAML::Value << magic_enum::enum_name(textureCompareModes);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::TextureFormat& textureFormat)
	{
		out << YAML::Value << magic_enum::enum_name(textureFormat);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::TextureTarget& textureTarget)
	{
		out << YAML::Value << magic_enum::enum_name(textureTarget);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::ShaderDataType& shaderDataType)
	{
		out << YAML::Value << magic_enum::enum_name(shaderDataType);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::BufferType& bufferType)
	{

		out << YAML::Value << magic_enum::enum_name(bufferType);
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::ShaderType::ShaderType& shaderType)
	{
		constexpr uint8_t count = Rynex::ShaderType::s_Count;
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < count; i++)
		{
			uint16_t enumValue = BIT(i);

			if (BIT_EQUAL(shaderType, enumValue))
			{
				Rynex::ShaderType::ShaderType typeIndex = magic_enum::enum_value<Rynex::ShaderType::ShaderType>(enumValue);
				out << magic_enum::enum_name(typeIndex);
			}
		}
		out << YAML::EndSeq;

		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::RenderMode::RenderMode& renderMode)
	{
		constexpr uint16_t count = Rynex::RenderMode::s_Count;
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint16_t i = 1; i < count; i++)
		{
			uint16_t enumValue = BIT(i);
			if (BIT_EQUAL(renderMode, enumValue))
			{
				Rynex::RenderMode::RenderMode typeIndex = static_cast<Rynex::RenderMode::RenderMode>(enumValue);
				out << magic_enum::enum_name(typeIndex);
			}
		}
		out << YAML::EndSeq;

		return out;
	}
#pragma endregion

	Emitter& operator<<(Emitter& out, const Rynex::TextureWrappingSpecification& textureWrapping)
	{
		out << YAML::Flow;
		out << YAML::BeginMap;
		out << YAML::Key << "S" << textureWrapping.S;
		out << YAML::Key << "T" << textureWrapping.T;
		out << YAML::Key << "R" << textureWrapping.R;
		out << YAML::EndMap;
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::FramebufferTextureSpecification& framebufferTextureSpecification)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "TextureFormat" << framebufferTextureSpecification.TextureFormat;
		out << YAML::Key << "Samples" << framebufferTextureSpecification.Samples;
		out << YAML::Key << "TextureWrapping" << framebufferTextureSpecification.TextureWrapping;
		out << YAML::Key << "TextureFiltering" << framebufferTextureSpecification.TextureFiltering;
		out << YAML::Key << "Compare" << framebufferTextureSpecification.Compare;
		out << YAML::EndMap;
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::SceneCamera& camera)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectionType" << camera.GetProjectionType();
		out << YAML::Key << "PerspectivVerticleFOV" << camera.GetPerspectivVerticleFOV();
		out << YAML::Key << "PerspectivNearClipe" << camera.GetPerspectivNearClipe();
		out << YAML::Key << "PerspectivFarClipe" << camera.GetPerspectivFarClipe();
		out << YAML::Key << "OrthographicSize" << camera.GetOrthographicSize();
		out << YAML::Key << "OrthographicNearClipe" << camera.GetOrthographicNearClipe();
		out << YAML::Key << "OrthographicFarClipe" << camera.GetOrthographicFarClipe();

		out << YAML::EndMap;

		return out;
	}


	Emitter& operator<<(Emitter& out, const Rynex::FramebufferAttachmentSpecification& framebufferAttachmentSpecification)
	{
		out << YAML::BeginSeq;
		for (const Rynex::FramebufferTextureSpecification& framTexSpec : framebufferAttachmentSpecification.Attachments)
		{
			out << framTexSpec;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const Rynex::FramebufferSpecification& specification)
	{
		out << YAML::BeginMap;

		out << YAML::Key << "Width" << YAML::Value << specification.Width;
		out << YAML::Key << "Height" << YAML::Value << specification.Height;
		out << YAML::Key << "Depth" << YAML::Value << specification.Depth;

		out << YAML::Key << "Attachments" << specification.Attachments;
		out << YAML::Key << "Target" << specification.Target;
		out << YAML::Key << "Samples" << specification.Samples;
		out << YAML::Key << "SwapChainTarget" << specification.SwapChainTarget;

		out << YAML::EndSeq;
		return out;
	}



#pragma region DcodeFunc
	


#pragma region Data_Struct
#pragma region CoverterOperators

	
	bool convert<Rynex::SceneCamera::ProjectionType>::decode(const Node& node, Rynex::SceneCamera::ProjectionType& projectionType)
	{
		std::string projectionTypeStr = node.as<std::string>();
		std::optional<Rynex::SceneCamera::ProjectionType> optionle = magic_enum::enum_cast<Rynex::SceneCamera::ProjectionType>(projectionTypeStr);
		projectionType = optionle.value_or(Rynex::SceneCamera::ProjectionType::Perspectiv);
		return true;
	}

	bool convert<Rynex::TextureWrappingMode>::decode(const Node& node, Rynex::TextureWrappingMode& textureWrappingMode)
	{
		std::string textureWrappingModeStr = node.as<std::string>();
		std::optional<Rynex::TextureWrappingMode> optionle = magic_enum::enum_cast<Rynex::TextureWrappingMode>(textureWrappingModeStr);
		textureWrappingMode = optionle.value_or(Rynex::TextureWrappingMode::Default);
		return true;
	}
	

	bool convert<Rynex::TextureFilteringMode>::decode(const Node& node, Rynex::TextureFilteringMode& textureFilteringMode)
	{
		std::string textureFilteringModeStr = node.as<std::string>();
		std::optional<Rynex::TextureFilteringMode> optionle = magic_enum::enum_cast<Rynex::TextureFilteringMode>(textureFilteringModeStr);
		textureFilteringMode = optionle.value_or(Rynex::TextureFilteringMode::Default);
		return true;
	}
	

	bool convert<Rynex::TextureCompareModes>::decode(const Node& node, Rynex::TextureCompareModes& textureCompareModes)
	{
		std::string textureCompareModesStr = node.as<std::string>();
		std::optional<Rynex::TextureCompareModes> optionle = magic_enum::enum_cast<Rynex::TextureCompareModes>(textureCompareModesStr);
		textureCompareModes = optionle.value_or(Rynex::TextureCompareModes::Default);
		return true;
	}
	

	bool convert<Rynex::TextureFormat>::decode(const Node& node, Rynex::TextureFormat& textureFormat)
	{
		std::string textureFormatStr = node.as<std::string>();
		std::optional<Rynex::TextureFormat> optionle = magic_enum::enum_cast<Rynex::TextureFormat>(textureFormatStr);
		textureFormat = optionle.value_or(Rynex::TextureFormat::Default);
		return true;
	}

	bool convert<Rynex::TextureTarget>::decode(const Node& node, Rynex::TextureTarget& textureFormat)
	{
		std::string textureTargetStr = node.as<std::string>();
		std::optional<Rynex::TextureTarget> optionle = magic_enum::enum_cast<Rynex::TextureTarget>(textureTargetStr);
		textureFormat = optionle.value_or(Rynex::TextureTarget::Default);
		return true;
	}

	bool convert<Rynex::ShaderDataType>::decode(const Node& node, Rynex::ShaderDataType& textureFormat)
	{
		std::string textureTargetStr = node.as<std::string>();
		std::optional<Rynex::ShaderDataType> optionle = magic_enum::enum_cast<Rynex::ShaderDataType>(textureTargetStr);
		textureFormat = optionle.value_or(Rynex::ShaderDataType::None);
		return true;
	}

#pragma endregion

	bool convert<Rynex::TextureWrappingSpecification>::decode(const Node& node, Rynex::TextureWrappingSpecification& textureWrapping)
	{
		textureWrapping.S = node["S"].as<Rynex::TextureWrappingMode>();
		textureWrapping.T = node["T"].as<Rynex::TextureWrappingMode>();
		textureWrapping.R = node["R"].as<Rynex::TextureWrappingMode>();

		return true;
	}

	bool convert<Rynex::FramebufferTextureSpecification>::decode(const Node& node, Rynex::FramebufferTextureSpecification& framebufferTextureSpecification)
	{
		framebufferTextureSpecification.TextureFormat = node["TextureFormat"].as<Rynex::TextureFormat>();
		framebufferTextureSpecification.Samples = node["Samples"].as<uint32_t>();
		framebufferTextureSpecification.TextureWrapping = node["TextureWrapping"].as<Rynex::TextureWrappingSpecification>();
		framebufferTextureSpecification.TextureFiltering = node["TextureFiltering"].as<Rynex::TextureFilteringMode>();
		framebufferTextureSpecification.Compare = node["Compare"].as<Rynex::TextureCompareModes>();

		return true;
	}

	bool convert<Rynex::SceneCamera>::decode(const Node& node, Rynex::SceneCamera& sceneCamera)
	{
		sceneCamera.SetProjectionType(node["ProjectionType"].as<Rynex::SceneCamera::ProjectionType>());
		sceneCamera.SetPerspectivVerticleFOV(node["PerspectivVerticleFOV"].as<float>());
		sceneCamera.SetPerspectivNearClipe(node["PerspectivNearClipe"].as<float>());
		sceneCamera.SetPerspectivFarClipe(node["PerspectivFarClipe"].as<float>());
		sceneCamera.SetOrthograficSize(node["OrthographicSize"].as<float>());
		sceneCamera.SetOrthograficNearClipe(node["OrthographicNearClipe"].as<float>());
		sceneCamera.SetOrthograficFarClipe(node["OrthographicFarClipe"].as<float>());

		return true;
	}

	bool convert<Rynex::FramebufferAttachmentSpecification>::decode(const Node& node, Rynex::FramebufferAttachmentSpecification& framebufferAttachmentSpecification)
	{
		framebufferAttachmentSpecification.Attachments.reserve(node.size());
		for (const Node& nodeAttchment : node)
		{
			framebufferAttachmentSpecification.Attachments.emplace_back(nodeAttchment.as<Rynex::FramebufferTextureSpecification>());
		}

		return true;
	}

	bool convert<Rynex::FramebufferSpecification>::decode(const Node& node, Rynex::FramebufferSpecification& specification)
	{
		specification.Width = node["Width"].as<uint32_t>();
		specification.Height = node["Height"].as<uint32_t>();
		specification.Depth = node["Depth"].as<uint32_t>();


		specification.Attachments = node["Attachments"].as<Rynex::FramebufferAttachmentSpecification>();
		specification.Target = node["Target"].as<Rynex::TextureTarget>();
		specification.Samples = node["Samples"].as<uint32_t>();
		specification.SwapChainTarget = node["SwapChainTarget"].as<bool>();

		return true;
	}


	bool convert<Rynex::BufferLayout>::decode(const Node& node, Rynex::BufferLayout& layout)
	{

		std::vector<Rynex::BufferElement> elementsVec;
		uint32_t count = node.size();
		elementsVec.reserve(count);

		const Node& autoCompressNode = node["AutoCompress"];
		bool autoCompress = autoCompressNode ? autoCompressNode.as<bool>() : false;

		const Node& instanIncreaseNode = node["InstastancIncreas"];
		uint32_t instancIncrease = instanIncreaseNode ? instanIncreaseNode.as<uint32_t>() : 0u;



		const Node& nodeElements = node["Elements"];
		for (uint32_t i = 0u; i < count; i++)
		{
			Rynex::BufferElement elements = nodeElements[i].as<Rynex::BufferElement>();
			elementsVec.emplace_back(elements);
		}
		layout = Rynex::BufferLayout(elementsVec, instancIncrease);
		layout.SetAutoCompress(autoCompress);
		return true;

	}

	bool convert<Rynex::BufferElement>::decode(const Node& node, Rynex::BufferElement& element)
	{
		std::string shaderDataTypeStr = node[0].as<std::string>();
		std::optional<Rynex::ShaderDataType> optionel = magic_enum::enum_cast<Rynex::ShaderDataType>(shaderDataTypeStr);


		element.type = optionel.value_or(Rynex::ShaderDataType::None);
		element.name = node[1].as<std::string>();
		element.normilized = node.size() < 3 ? false : node[2].as<bool>();


		element = Rynex::BufferElement(element.type, element.name, true, 0u, element.normilized);
		return true;
	}

#pragma endregion

	bool convert<std::filesystem::path>::decode(const Node& node, std::filesystem::path& rhs)
	{
		if (!node.IsScalar())
			return false;

		rhs = node.Scalar();
		return true;
	}

#pragma region type_Decode


#pragma endregion

#pragma region glm_Decode

#pragma region glm_FLOAT_Decode
	bool convert<glm::vec2>::decode(const Node& node, glm::vec2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		for (uint8_t i = 0; i < 2; i++)
		{
			rhs[i] = node[i].as<float>();
		}
		return true;
	}

	bool convert<glm::vec3>::decode(const Node& node, glm::vec3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		for (uint8_t i = 0; i < 3; i++)
		{
			rhs[i] = node[i].as<float>();
		}

		return true;
	}

	bool convert<glm::vec4>::decode(const Node& node, glm::vec4& rhs)
	{
		uint32_t count = node.size();
		if (!node.IsSequence() || count != 4)
			return false;

		for (uint8_t i = 0; i < 4; i++)
		{
			rhs[i] = node[i].as<float>();
		}

		return true;
	}

	bool convert<glm::mat3>::decode(const Node& node, glm::mat3& rhs)
	{
		constexpr uint8_t dymension = 3;
		constexpr uint8_t count = dymension * dymension;
		if (!node.IsSequence() || node.size() != count)
			return false;


		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t x = i / dymension;
			uint8_t y = i % dymension;

			rhs[x][y] = node[i].as<float>();
		}


		return true;
	}

	bool convert<glm::mat4>::decode(const Node& node, glm::mat4& rhs)
	{
		constexpr uint8_t dymension = 4;
		constexpr uint8_t count = dymension * dymension;
		if (node.IsSequence() && node.size() == count)
		{
			for (uint8_t i = 0; i < count; i++)
			{
				uint8_t x = i / dymension;
				uint8_t y = i % dymension;

				rhs[x][y] = node[i].as<float>();
			}
			return true;
		}
		else if(!node.IsSequence() || node.size() != dymension)
		{
			return false;
		}


		for (uint8_t i = 0; i < dymension; i++)
		{
			rhs[i] = node[i].as<glm::vec4>();
		}


		return true;
	}
#pragma endregion

#pragma region glm_DOUBLE_Decode


	bool convert<glm::dvec2>::decode(const Node& node, glm::dvec2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		for (uint8_t i = 0; i < 2; i++)
		{
			rhs[i] = node[i].as<double>();
		}
		return true;
	}

	bool convert<glm::dvec3>::decode(const Node& node, glm::dvec3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		for (uint8_t i = 0; i < 3; i++)
		{
			rhs[i] = node[i].as<double>();
		}

		return true;
	}

	bool convert<glm::dvec4>::decode(const Node& node, glm::dvec4& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		for (uint8_t i = 0; i < 4; i++)
		{
			rhs[i] = node[i].as<double>();
		}

		return true;
	}

	bool convert<glm::dmat3>::decode(const Node& node, glm::dmat3& rhs)
	{
		constexpr uint8_t dymension = 3;
		constexpr uint8_t count = dymension * dymension;
		if (!node.IsSequence() || node.size() != count)
			return false;


		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t x = i / dymension;
			uint8_t y = i % dymension;

			rhs[x][y] = node[i].as<double>();
		}


		return true;
	}

	bool convert<glm::dmat4>::decode(const Node& node, glm::dmat4& rhs)
	{
		constexpr uint8_t dymension = 4;
		constexpr uint8_t count = dymension * dymension;
		if (!node.IsSequence() || node.size() != count)
			return false;


		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t x = i / dymension;
			uint8_t y = i % dymension;

			rhs[x][y] = node[i].as<double>();
		}


		return true;
	}

#pragma endregion

#pragma region glm_INT_Decode

	bool convert<glm::ivec2>::decode(const Node& node, glm::ivec2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		for (uint8_t i = 0; i < 2; i++)
		{
			rhs[i] = node[i].as<int>();
		}
		return true;
	}

	bool convert<glm::ivec3>::decode(const Node& node, glm::ivec3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		for (uint8_t i = 0; i < 3; i++)
		{
			rhs[i] = node[i].as<int>();
		}

		return true;
	}

	bool convert<glm::ivec4>::decode(const Node& node, glm::ivec4& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		for (uint8_t i = 0; i < 4; i++)
		{
			rhs[i] = node[i].as<int>();
		}

		return true;
	}

	bool convert<glm::imat3x3>::decode(const Node& node, glm::imat3x3& rhs)
	{
		constexpr uint8_t dymension = 3;
		constexpr uint8_t count = dymension * dymension;
		if (!node.IsSequence() || node.size() != count)
			return false;


		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t x = i / dymension;
			uint8_t y = i % dymension;

			rhs[x][y] = node[i].as<int>();
		}


		return true;
	}

	bool convert<glm::imat4x4>::decode(const Node& node, glm::imat4x4& rhs)
	{
		constexpr uint8_t dymension = 4;
		constexpr uint8_t count = dymension * dymension;
		if (!node.IsSequence() || node.size() != count)
			return false;


		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t x = i / dymension;
			uint8_t y = i % dymension;

			rhs[x][y] = node[i].as<int>();
		}


		return true;
	}
#pragma endregion

#pragma region glm_UINT_Decode

	bool convert<glm::uvec2>::decode(const Node& node, glm::uvec2& rhs)
	{
		if (!node.IsSequence() || node.size() != 2)
			return false;

		for (uint8_t i = 0; i < 2; i++)
		{
			rhs[i] = node[i].as<uint32_t>();
		}
		return true;
	}

	bool convert<glm::uvec3>::decode(const Node& node, glm::uvec3& rhs)
	{
		if (!node.IsSequence() || node.size() != 3)
			return false;

		for (uint8_t i = 0; i < 3; i++)
		{
			rhs[i] = node[i].as<uint32_t>();
		}

		return true;
	}

	bool convert<glm::uvec4>::decode(const Node& node, glm::uvec4& rhs)
	{
		if (!node.IsSequence() || node.size() != 4)
			return false;

		for (uint8_t i = 0; i < 4; i++)
		{
			rhs[i] = node[i].as<uint32_t>();
		}

		return true;
	}

	bool convert<glm::umat3x3>::decode(const Node& node, glm::umat3x3& rhs)
	{
		constexpr uint8_t dymension = 3;
		constexpr uint8_t count = dymension * dymension;
		if (!node.IsSequence() || node.size() != count)
			return false;


		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t x = i / dymension;
			uint8_t y = i % dymension;

			rhs[x][y] = node[i].as<uint32_t>();
		}
		return true;
	}

	bool convert<glm::umat4x4>::decode(const Node& node, glm::umat4x4& rhs)
	{
		constexpr uint8_t dymension = 4;
		constexpr uint8_t count = dymension * dymension;
		if (!node.IsSequence() || node.size() != count)
			return false;


		for (uint8_t i = 0; i < count; i++)
		{
			uint8_t x = i / dymension;
			uint8_t y = i % dymension;

			rhs[x][y] = node[i].as<uint32_t>();
		}

		return true;
	}
#pragma endregion


#pragma endregion

#pragma endregion
}


namespace Serializer {
	
	void AssetFormate(YAML::Emitter& out, Rynex::AssetHandle handle)
	{
		Rynex::Ref<Rynex::Project> project = Rynex::Project::GetActive();
		Rynex::Ref<Rynex::EditorAssetManegerThreade> editorAssetManger = project->GetEditorAssetManger();
		Rynex::AssetMetadata metadata = editorAssetManger->GetMetadata(handle);
		const std::filesystem::path& filePath = metadata.FilePath;
		const std::filesystem::path& pathMarked = metadata.PathMarker;

		std::string filePathStr = filePath.string();
		std::string pathMarkedStr = pathMarked.string();

		
		out << YAML::BeginMap;
		out << YAML::Key << "Path" << filePathStr;
		out << YAML::Key << "Path-ProjectMarker" << pathMarkedStr;
		out << YAML::Key << "Handle" << handle;
		out << YAML::EndMap;
	}
	
	void AssetFormate(YAML::Emitter& out, const std::string& name, Rynex::AssetHandle handle)
	{
		out << YAML::Key << name;
		AssetFormate(out, handle);
	}
}
namespace Deserialize {

	template<>
	bool AssetFormate<Rynex::Texture>(YAML::Node& nodeE, Rynex::Ref<Rynex::Texture>* entityC, bool async)
	{
		if (!nodeE)
			return false;

		std::string path = "";
		std::string makredPath = "";
		if (YAML::Node nodeAtribut = nodeE["Path"])
			path = nodeAtribut.as<std::string>();
		if (YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
			makredPath = nodeAtribut.as<std::string>();

		Rynex::AssetHandle handle = nodeE["Handle"].as<uint64_t>();
		Rynex::AssetFindeInfo info = Rynex::AssetFindeInfo(handle, makredPath, path);
		*entityC = Rynex::AssetManager::FindeAsset<Rynex::Texture>(info);

		return true;
	}

	template<>
	bool AssetFormate<Rynex::MeshStatic>(YAML::Node& nodeE, Rynex::Ref<Rynex::MeshStatic>* entityC, bool async)
	{
		if (!nodeE)
			return false;

		std::string path = "";
		std::string makredPath = "";
		if (YAML::Node nodeAtribut = nodeE["Path"])
			path = nodeAtribut.as<std::string>();
		if (YAML::Node nodeAtribut = nodeE["Path-ProjectMarker"])
			makredPath = nodeAtribut.as<std::string>();

		Rynex::AssetHandle handle = nodeE["Handle"].as<uint64_t>();
		Rynex::AssetFindeInfo info = Rynex::AssetFindeInfo(handle, makredPath, path);
		*entityC = Rynex::AssetManager::FindeAsset<Rynex::MeshStatic>(info);

		return true;
	}
}

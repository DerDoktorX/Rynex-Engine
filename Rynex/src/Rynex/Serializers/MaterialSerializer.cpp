#include "rypch.h"
#include "MaterialSerializer.h"
#include "YAML.h"

#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/API/Buffer.h>


#include <magic_enum.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <Rynex/Renderer/PiplineObjects/Piplines/RenderPipline.h>





namespace Rynex {

	MaterialSerializer::MaterialSerializer(Ref<Material>& materiel)
		: m_Materiel(materiel)
	{
	}

	bool MaterialSerializer::Serlize(const std::filesystem::path& path)
	{
		RY_CORE_NOT_IMPL();
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key <<  "Material";

		out << YAML::BeginMap;
		out << YAML::Key << "Prame";
		for (uint8_t i = 0; i < 0; i++)
		{
			out << YAML::BeginSeq;
			SDT type;
			std::string elementName;
			out << type;
			out << elementName;
			out << YAML::EndSeq;
		}
		out << YAML::BeginSeq;
		out << YAML::EndMap;

		
		

		out << YAML::EndMap;

		out << YAML::BeginMap;
		out << YAML::Key << "Pipelines";
		out << YAML::BeginSeq;

		for (uint8_t i = 0; i < 0; i++)
		{
			out << YAML::BeginMap;
			{
				std::string passName;
				out << YAML::Key << "Name" << passName;

				Ref<Shader> shader;
				Ref<RenderPipline> pipline;

				Serializer::AssetFormate(out, "Shader", shader->Handle);
				Serializer::AssetFormate(out, "Pipline", pipline->Handle);
				{
					out << YAML::Key << "MaterielBuffer";
					{
						out << YAML::BeginSeq;
						for (uint8_t i = 0; i < 0; i++)
						{
							out << YAML::BeginSeq;
							SDT type;
							std::string elementName;
							out << type;
							out << elementName;

							out << YAML::EndSeq;
						}
						out << YAML::EndSeq;
					}
				}
				{
					out << YAML::Key << "Textures";
					{
						out << YAML::BeginSeq;
						for (uint8_t i = 0; i < 0; i++)
						{
							out << YAML::BeginMap;
							Ref<Texture> texture;
							std::string textureName;
							out << YAML::Key << "Name" << textureName;
							out << YAML::Key << "Index" << i;

							Serializer::AssetFormate(out, "Texture", texture->Handle);

							out << YAML::EndMap;
						}
						out << YAML::EndSeq;
					}
				}
			}
			out << YAML::EndMap;

		}		
		out << YAML::EndSeq;
		out << YAML::EndMap;

		

		out << YAML::EndMap;

		std::stringstream strStream;
		strStream << out.c_str();


		return false;
	}

	bool MaterialSerializer::Deserlize(const std::filesystem::path& path)
	{
		RY_CORE_NOT_IMPL();

		std::ifstream stream(path);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		YAML::Node data = YAML::Load(strStream);

		if (!data["VertexGeomtryLayout"])
			return false;
		BufferLayout layout = data["VertexGeomtryLayout"].as<BufferLayout>();


		return false;
	}
}
#include "rypch.h"
#include "RenderPiplineSerializer.h"
#include "YAML.h"

#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/API/Buffer.h>


#include <magic_enum/magic_enum.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Rynex {

	RenderPiplineSerializer::RenderPiplineSerializer(const Ref<RenderPipline>& renderPipline)
	{
	}


	bool RenderPiplineSerializer::Serlize(const std::filesystem::path& path)
	{
		RY_CORE_NOT_IMPL();

		YAML::Emitter out;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "Pipline";
			out << YAML::BeginMap;

			{
				out << YAML::Key << "Resource";
				out << YAML::BeginSeq;
				for (uint8_t i = 0; i < 0; i++)
				{
					out << YAML::BeginMap;
					{
						bool isSource;
						std::string nameBuffer;
						BufferType bufferType;
						out << YAML::Key << "Buffer";
						out << YAML::Key << "Name" << nameBuffer;
						out << YAML::Key << "Type" << bufferType;

						if(isSource)
						{
							std::string source;
							out << YAML::Key << "Source" << source;
						}
						else
						{
							uint32_t bindSlot = 0;
							const std::vector<std::pair<BufferElement, std::string>> layoute;

							out << YAML::Key << "Layout";
							out << YAML::BeginSeq;
							for (const auto& [ellement, source] : layoute)
							{
								out << YAML::Flow;
								out << YAML::BeginSeq;

								out << ellement.type;
								out << ellement.name;
								if (!ellement.normilized)
									out << ellement.normilized;
								out << source;

								out << YAML::EndSeq;

							}
							out << YAML::EndSeq;

						}
					}
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
			{
				out << YAML::Key << "Definetion";
				{
					out << YAML::BeginSeq;
					for (uint8_t i = 0; i < 0; i++)
					{
						out << YAML::BeginMap;
						{
							ShaderType::ShaderType shaderType;
							out << YAML::Key << "Shader" << shaderType;
							out << YAML::BeginSeq;
							for (uint8_t i = 0; i < 0; i++)
							{
								out << YAML::Flow;
								out << YAML::BeginMap;
								out << YAML::Key << "Buffer";
								{
									out << YAML::Flow;
									out << YAML::BeginSeq;
									{
										uint32_t bindSlot;
										std::string nameBuffer;
										out << bindSlot;
										out << nameBuffer;
									}
									out << YAML::EndSeq;
								}
								out << YAML::EndMap;
							}
							out << YAML::EndSeq;


						}
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
				}
			}
			out << YAML::EndMap;

		}
		out << YAML::EndMap;


		return false;
	}
}
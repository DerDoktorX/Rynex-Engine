#include "rypch.h"
#include "FrambufferSerializer.h"


#include "Rynex/Renderer/API/Buffer.h"
#include "Rynex/Project/Project.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>



namespace YAML {

}


namespace Rynex {



	bool FrambufferSerializer::Serilze(const std::filesystem::path& filepath, Ref<Framebuffer> frambuffer)
	{

		const FramebufferSpecification& spec = frambuffer->GetFramebufferSpecification();
		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			//out << YAML::Key << "Framebuffer-AssetHandle" << YAML::Value << AssetHandle();
			out << YAML::Key << "Framebuffer" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Width" << YAML::Value << spec.Width;
				out << YAML::Key << "Height" << YAML::Value << spec.Height;
				out << YAML::Key << "Attachments";
				{
					out << YAML::BeginSeq;
					for (const FramebufferTextureSpecification& framTexSpec : spec.Attachments.Attachments)
					{
						// out << YAML::Key << "TextureAssetHandle" << YAML::Value << AssetHandle();
						out << YAML::BeginMap;
						out << YAML::Key << "TextureFormat" << YAML::Value << (int)framTexSpec.TextureFiltering;
						out << YAML::Key << "TextureWrapping" << YAML::Value;
						out << YAML::BeginMap;
						{
							out << YAML::Key << "R" << YAML::Value << (int)framTexSpec.TextureWrapping.R;
							out << YAML::Key << "T" << YAML::Value << (int)framTexSpec.TextureWrapping.T;
							out << YAML::Key << "S" << YAML::Value << (int)framTexSpec.TextureWrapping.S;
						}
						out << YAML::EndMap;
						out << YAML::Key << "TextureFiltering" << YAML::Value << (int)framTexSpec.TextureFiltering;
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
				}
				out << YAML::Key << "Samples" << YAML::Value << spec.Samples;
				out << YAML::Key << "SwapChainTarget" << YAML::Value << spec.SwapChainTarget;
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	Ref<Framebuffer> FrambufferSerializer::Deserilze(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}
		FramebufferSpecification spec = FramebufferSpecification();
		auto frameNode = data["Framebuffer"];
		
		if (frameNode)
		{
			spec.Width = frameNode["Width"].as<uint32_t>();
			spec.Height = frameNode["Height"].as<uint32_t>();
			auto attachmentsNode = frameNode["Attachments"];

			auto& attachments = spec.Attachments.Attachments;
			FramebufferTextureSpecification framSpec;
			for (const auto& attachmentNode : attachmentsNode)
			{
				framSpec.TextureFormat = (TextureFormat)attachmentNode["TextureFormat"].as<int>();

				auto textureWrappingNode = attachmentNode["TextureWrapping"];
				framSpec.TextureWrapping.R = (TextureWrappingMode)textureWrappingNode["R"].as<int>();
				framSpec.TextureWrapping.T = (TextureWrappingMode)textureWrappingNode["T"].as<int>();
				framSpec.TextureWrapping.S = (TextureWrappingMode)textureWrappingNode["S"].as<int>();
				
				framSpec.TextureFiltering = (TextureFilteringMode)attachmentNode["TextureFiltering"].as<int>();
				attachments.push_back(framSpec);
			}
			
			spec.Samples = frameNode["Samples"].as<uint32_t>();
			spec.SwapChainTarget = frameNode["SwapChainTarget"].as<bool>();
		}
		Ref<Framebuffer> frameBuffer = Framebuffer::Create(spec);
		return frameBuffer;
	}

};
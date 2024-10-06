#include "rypch.h"
#include "VertexArraySerialzer.h"


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



	struct GeomtryInput
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 nomale;
	};

	bool VertexArraySerialzer::Serialzer(const std::filesystem::path& path, Ref<VertexArray> vertexArray)
	{
		const auto& vertexbuffers = vertexArray->GetVertexBuffers();
		const auto& indexBuffer = vertexArray->GetIndexBuffers();
		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "VertexArray" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Primitiv" << YAML::Value << (int)vertexArray->GetPrimitv();
				out << YAML::Key << "VertexBuffer" << YAML::Value;
				{
					out << YAML::BeginSeq;
					for (auto& buffer : vertexbuffers)
					{
						std::vector<unsigned char> data = buffer->GetBufferData();

						// out << YAML::Key << "TextureAssetHandle" << YAML::Value << AssetHandle();
						out << YAML::BeginMap;
						out << YAML::Key << "ByteSize" << YAML::Value << buffer->GetByteSize();
						out << YAML::Key << "Laoute" << YAML::Value;
						out << YAML::BeginSeq;
						const BufferLayout& layoute = buffer->GetLayout();
					
						
						
						for (auto& ellement : layoute)
						{
							out << YAML::BeginMap;
							out << YAML::Key << "Name" << YAML::Value << ellement.Name;
							out << YAML::Key << "Normilized" << YAML::Value << ellement.Normilized;
							out << YAML::Key << "Offset" << YAML::Value << ellement.Offset;
							out << YAML::Key << "Type" << YAML::Value << (int)ellement.Type;
							out << YAML::Key << "Size" << YAML::Value << ellement.Size;
							out << YAML::EndMap;
						}
						out << YAML::EndSeq;
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
				}
				
				if (indexBuffer)
				{
#if 0
					allData[index] = indexBuffer->GetBufferData();
#endif
					out << YAML::Key << "IndexBuffer" << YAML::Value;
					out << YAML::BeginMap;
					{
						out << YAML::Key << "Count" << YAML::Value << indexBuffer->GetCount();
						out << YAML::Key << "ElementByte" << YAML::Value << indexBuffer->GetElementByte();
					}
					out << YAML::EndMap;
				}
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(path);
		fout << out.c_str();
		fout << "\n--START DATA BINRY--\n";
		//RY_CORE_ASSERT(false, "VertexArraySerialzer::Serialzer! is not rady");
#if 1
		for(auto& buffer : vertexbuffers)
		{
			std::vector<unsigned char> data = buffer->GetBufferData();
			fout.write(reinterpret_cast<const char*>(data.data()), data.size());
			buffer->FreeBufferData();
		}
		if (indexBuffer)
		{
			std::vector<unsigned char> data = indexBuffer->GetBufferData();
			fout.write(reinterpret_cast<const char*>(data.data()), data.size());
			indexBuffer->FreeBufferData();
		}
#endif
		return true;
	}

	Ref<VertexArray> VertexArraySerialzer::Deserlize(const std::filesystem::path& path)
	{
		YAML::Node data;
		std::ifstream file(path, std::ios::binary);
		std::string line, ymalContnet;
		try
		{
			while (std::getline(file, line))
			{
				if (line == "--START DATA BINRY--\r")
					break;
				ymalContnet += line + "\n";
			}
			//data = YAML::LoadFile(filepath.string());
			data = YAML::Load(ymalContnet);
		}
		catch (YAML::ParserException e)
		{
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", path.string(), e.what());
			return false;
		}
		auto arrayeNode = data["VertexArray"];
		if (!arrayeNode)
			return nullptr;
		Ref<VertexArray> vertexArray = VertexArray::Create();
		vertexArray->SetPrimitv((VertexArray::Primitv)arrayeNode["Primitiv"].as<int>());
		auto vertexNodeBuffers = arrayeNode["VertexBuffer"];

		
		if (vertexNodeBuffers)
		{
			
			for (auto vertexNodeBuffer : vertexNodeBuffers)
			{
			
			
				uint32_t byteSize = vertexNodeBuffer["ByteSize"].as<uint32_t>();
				auto usageNode = vertexNodeBuffer["Usage"];
				if (usageNode)
				{
				}

				auto layout = vertexNodeBuffer["Laoute"];
				std::vector<BufferElement> bufferElements;
				if (layout)
				{
					
					for (auto ellements : layout)
					{
						BufferElement bufferElement;
						
						bufferElement.Name = ellements["Name"].as<std::string>();
						bufferElement.Type = (ShaderDataType)ellements["Type"].as<int>();
						bufferElement.Normilized = ellements["Normilized"].as<bool>();
						bufferElement.Offset = ellements["Offset"].as<uint32_t>();
						bufferElement.Size = ellements["Size"].as<uint32_t>();
						bufferElements.push_back(bufferElement);
					}

					
				}					
				BufferLayout layoute(bufferElements);
				std::vector<unsigned char> dataByte;

				dataByte.resize(byteSize);
				file.read(reinterpret_cast<char*>(dataByte.data()), byteSize);
				
				//void* input = new void*[byteSize];
				//input = static_cast<void*>(dataByte.data());
				//GeomtryInput* geomtryInput = static_cast<GeomtryInput*>(input);
				Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(byteSize);
				vertexBuffer->SetData(dataByte.data(),byteSize);
				vertexBuffer->SetLayout(bufferElements);
				
				
			
				vertexArray->AddVertexBuffer(vertexBuffer);
				//delete[] input;
				dataByte.clear();
			}
		}
		auto indexNode = arrayeNode["IndexBuffer"];
		Ref<IndexBuffer> indexBuffer = nullptr;
		if (indexNode)
		{
			
			uint32_t count = indexNode["Count"].as<uint32_t>();
			uint32_t elementByte = indexNode["ElementByte"].as<uint32_t>();
			std::vector<unsigned char> dataByte;
			{
				
				dataByte.clear();
				dataByte.resize(
					 count * elementByte 
				);
				file.read(reinterpret_cast<char*>(dataByte.data()), dataByte.size());
				if (elementByte == sizeof(uint32_t))
				{
					uint32_t* data_uint = (uint32_t*)(dataByte.data());
					indexBuffer = IndexBuffer::Create(data_uint, count);
					vertexArray->SetIndexBuffer(indexBuffer);
				}
				else if (elementByte == sizeof(uint16_t))
				{
					uint16_t* data_uint = (uint16_t*)(dataByte.data());
					indexBuffer = IndexBuffer::Create(data_uint, count);
					vertexArray->SetIndexBuffer(indexBuffer);
				}
				
			}
			
			

			
		}

		return vertexArray;
	}

}
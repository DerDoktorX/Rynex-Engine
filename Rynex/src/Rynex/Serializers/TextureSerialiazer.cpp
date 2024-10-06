#include "rypch.h"
#include "TextureSerialiazer.h"

#include "Rynex/Renderer/API/Buffer.h"
#include "Rynex/Project/Project.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

#define YAML_AKTIV 0


namespace YAML {
#if YAML_AKTIV
	Emitter& operator<<(Emitter& out, const std::string_view& v)
	{
		out << std::string(v.data(), v.size());
		return out;
	}


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
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat4& v)
	{

		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			out << Flow;
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << v[i].w << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}


#if 0

	Emitter& operator<<(Emitter& out, const glm::vec<2, float>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec<3, float>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec<4, float>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}


	Emitter& operator<<(Emitter& out, const glm::mat<3, 3, float>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat<4, 4, float>& v)
	{
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << v[i].w << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

#endif



	Emitter& operator<<(Emitter& out, const glm::vec<2, int>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec<3, int>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec<4, int>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}


	Emitter& operator<<(Emitter& out, const glm::mat<3, 3, int>& v)
	{

		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat<4, 4, int>& v)
	{
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << v[i].w << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}



	Emitter& operator<<(Emitter& out, const glm::vec<2, uint32_t>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec<3, uint32_t>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::vec<4, uint32_t>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}


	Emitter& operator<<(Emitter& out, const glm::mat<3, 3, uint32_t>& v)
	{

		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat<4, 4, uint32_t>& v)
	{
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v[i].x << v[i].y << v[i].z << v[i].w << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}


	template<>
	struct convert<glm::vec<2, float>>
	{
		static Node encode(const glm::vec<2, float>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec<2, float>& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec<3, float>>
	{
		static Node encode(const glm::vec<3, float>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec<3, float>& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec<4, float>>
	{
		static Node encode(const glm::vec<4, float>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec<4, float>& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};


	template<>
	struct convert<glm::mat<3, 3, float>>
	{
		static Node encode(const glm::mat<3, 3, float>& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 3; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);
				node.SetStyle(EmitterStyle::Flow);
			}
			return node;
		}

		static bool decode(const Node& node, glm::mat<3, 3, float>& rhs)
		{
			if (!node.IsSequence() || node.size() != 9)
				return false;

			for (uint8_t i = 0; i < 3; i++)
			{
				rhs[i].x = node[0 + i].as<float>();
				rhs[i].y = node[0 + i].as<float>();
				rhs[i].z = node[0 + i].as<float>();
			}
			return true;
		}
	};

	template<>
	struct convert<glm::mat<4, 4, float>>
	{
		static Node encode(const glm::mat<4, 4, float>& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 4; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);
				node.push_back(rhs[i].w);
				node.SetStyle(EmitterStyle::Flow);
			}
			return node;
		}

		static bool decode(const Node& node, glm::mat<4, 4, float>& rhs)
		{
			if (!node.IsSequence() || node.size() != 16)
				return false;

			for (uint8_t i = 0; i < 4; i++)
			{
				rhs[i].x = node[0 + i].as<float>();
				rhs[i].y = node[0 + i].as<float>();
				rhs[i].z = node[0 + i].as<float>();
				rhs[i].w = node[0 + i].as<float>();
			}
			return true;
		}
	};


#if 1
#endif

	template<>
	struct convert<glm::vec<2, int>>
	{
		static Node encode(const glm::vec< 2, int>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec< 2, int>& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec<3, int>>
	{
		static Node encode(const glm::vec< 3, int>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec< 3, int>& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			rhs.z = node[2].as<int>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec<4, int>>
	{
		static Node encode(const glm::vec<4, int>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec < 4, int>& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<int>();
			rhs.y = node[1].as<int>();
			rhs.z = node[2].as<int>();
			rhs.w = node[3].as<int>();
			return true;
		}
	};


	template<>
	struct convert<glm::mat<3, 3, int>>
	{
		static Node encode(const glm::mat<3, 3, int>& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 3; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);
				node.SetStyle(EmitterStyle::Flow);
			}
			return node;
		}

		static bool decode(const Node& node, glm::mat<3, 3, int>& rhs)
		{
			if (!node.IsSequence() || node.size() != 9)
				return false;
			for (uint8_t i = 0; i < 3; i++)
			{
				rhs[i].x = node[0 + i].as<int>();
				rhs[i].y = node[1 + i].as<int>();
				rhs[i].z = node[1 + i].as<int>();
			}
			return true;
		}
	};

	template<>
	struct convert<glm::mat<4, 4, int>>
	{
		static Node encode(const glm::mat<4, 4, int>& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 4; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);
				node.push_back(rhs[i].w);
				node.SetStyle(EmitterStyle::Flow);
			}
			return node;
		}

		static bool decode(const Node& node, glm::mat<4, 4, int>& rhs)
		{
			if (!node.IsSequence() || node.size() != 16)
				return false;

			for (uint8_t i = 0; i < 4; i++)
			{
				rhs[i].x = node[0 + i].as<int>();
				rhs[i].y = node[0 + i].as<int>();
				rhs[i].z = node[0 + i].as<int>();
				rhs[i].w = node[0 + i].as<int>();
			}
			return true;
		}
	};



	template<>
	struct convert<glm::vec<2, uint32_t>>
	{
		static Node encode(const glm::vec< 2, uint32_t>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec< 2, uint32_t>& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec<3, uint32_t>>
	{
		static Node encode(const glm::vec<3, uint32_t>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec<3, uint32_t>& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<uint32_t>();
			rhs.y = node[1].as<uint32_t>();
			rhs.z = node[2].as<uint32_t>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec<4, uint32_t>>
	{
		static Node encode(const glm::vec<4, uint32_t>& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec<4, uint32_t>& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<uint32_t>();
			rhs.y = node[1].as<uint32_t>();
			rhs.z = node[2].as<uint32_t>();
			rhs.w = node[3].as<uint32_t>();
			return true;
		}
	};


	template<>
	struct convert<glm::mat<3, 3, uint32_t>>
	{
		static Node encode(const glm::mat<3, 3, uint32_t>& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 3; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);
				node.SetStyle(EmitterStyle::Flow);
			}

			return node;
		}

		static bool decode(const Node& node, glm::mat<3, 3, uint32_t>& rhs)
		{
			if (!node.IsSequence() || node.size() != 9)
				return false;

			for (uint8_t i = 0; i < 3; i++)
			{
				rhs[i].x = node[0 + i].as<uint32_t>();
				rhs[i].y = node[0 + i].as<uint32_t>();
				rhs[i].z = node[0 + i].as<uint32_t>();
			}
			return true;
		}
	};

	template<>
	struct convert<glm::mat<4, 4, uint32_t>>
	{
		static Node encode(const glm::mat<4, 4, uint32_t>& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 4; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);
				node.push_back(rhs[i].w);
				node.SetStyle(EmitterStyle::Flow);
			}

			return node;
		}

		static bool decode(const Node& node, glm::mat<4, 4, uint32_t>& rhs)
		{
			if (!node.IsSequence() || node.size() != 16)
				return false;

			for (uint8_t i = 0; i < 4; i++)
			{
				rhs[i].x = node[0 + i].as<uint32_t>();
				rhs[i].y = node[0 + i].as<uint32_t>();
				rhs[i].z = node[0 + i].as<uint32_t>();
				rhs[i].w = node[0 + i].as<uint32_t>();
			}
			return true;
		}
	};
#endif
}

namespace Rynex {

#if YAML_AKTIV
	static void SerializerDynamicData(YAML::Emitter& out, const std::string& name, ShaderDataType type, const std::vector<unsigned char>& data)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<float>(*data.data());
			break;
		case ShaderDataType::Float2:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::vec<2, float>>(*data.data());
			break;
		case ShaderDataType::Float3:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::vec<3, float>>(*data.data());
			break;
		case ShaderDataType::Float4:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::vec<4, float>>(*data.data());
			break;
		case ShaderDataType::Float3x3:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::mat<3, 3, float>>(*data.data());
			break;
		case ShaderDataType::Float4x4:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::mat<4, 4, float>>(*data.data());
			break;
		case ShaderDataType::Int:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<int>(*data.data());
			break;
		case ShaderDataType::Int2:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::vec<2, int>>(*data.data());
			break;
		case ShaderDataType::Int3:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::vec<2, int>>(*data.data());
			break;
		case ShaderDataType::Int4:
			out << YAML::Key << name.c_str() << YAML::Value << static_cast<glm::vec<4, int>>(*data.data());
			break;
		}
	}

	static void SerializerDynamicDataLayout(YAML::Emitter& out, const std::string& name, const BufferLayout& layout, const std::vector<unsigned char>& data)
	{
		out << YAML::Key << name.c_str();
		out << YAML::BeginSeq;

		for (uint64_t offset = 0, size = data.size(); offset < size; offset += layout.GetStride())
		{
			out << YAML::Flow;
			for (auto& ellement : layout)
			{

				switch (ellement.Type)
				{
				case ShaderDataType::Float:
				{
					out << static_cast<float>(data.data()[offset]);
					break;
				}
				case ShaderDataType::Float2:
				{
					glm::vec<2, float> v2 = static_cast<glm::vec<2, float>>(data.data()[offset]);
					out << v2.x << v2.y;
					break;
				}
				case ShaderDataType::Float3:
				{
					glm::vec<3, float> v3 = static_cast<glm::vec<3, float>>(data.data()[offset]);
					out << v3.x << v3.y << v3.z;
					break;
				}
				case ShaderDataType::Float4:
				{
					glm::vec<4, float> v4 = static_cast<glm::vec<4, float>>(data.data()[offset]);
					out << v4.x << v4.y << v4.z << v4.w;
					break;
				}
				case ShaderDataType::Int:
				{
					int v = static_cast<int>(data.data()[offset]);
					out << v;
					break;
				}
				case ShaderDataType::Int2:
				{
					glm::vec<2, int> v = static_cast<glm::vec<2, int>>(data.data()[offset]);
					out << v.x << v.y;
					break;
				}
				case ShaderDataType::Int3:
				{
					glm::vec<3, float> v = static_cast<glm::vec<3, float>>(data.data()[offset]);
					out << v.x << v.y << v.z;
					break;
				}
				case ShaderDataType::Int4:
				{
					glm::vec<4, float> v = static_cast<glm::vec<4, float>>(data.data()[offset]);
					out << v.x << v.y << v.z << v.w;
					break;
				}
				}
			}
		}
		out << YAML::EndSeq;
	}

	static void SerializerAssetFormate(YAML::Emitter& out, const std::string& name, AssetHandle handle)
	{
		out << YAML::Key << name.c_str();
		out << YAML::BeginMap;
		out << YAML::Key << "Path" << Project::GetActive()->GetEditorAssetManger()->GetMetadata(handle).FilePath.string().c_str();
		out << YAML::Key << "AssetHandle" << handle;
		out << YAML::EndMap;
	}


	static void DeserializeDynamicData(YAML::Node& node, ShaderDataType type, std::vector<unsigned char>& data)
	{
		data.resize(ShaderDataTypeSize(type));
		switch (type)
		{
		case ShaderDataType::Float:
		{
			float* dataV = (float*)data.data();
			*dataV = node.as<float>();
			break;
		}
		case ShaderDataType::Float2:
		{
			float* dataV = (float*)data.data();
			*dataV = node.as<float>();
			break;
		}
		case ShaderDataType::Float3:
		{
			glm::vec<3, float>* dataV = (glm::vec<3, float>*)data.data();
			*dataV = node.as<glm::vec<3, float>>();
			break;
		}
		case ShaderDataType::Float4:
		{
			glm::vec<4, float>* dataV = (glm::vec<4, float>*)data.data();
			*dataV = node.as<glm::vec<4, float>>();
			break;
		}
		case ShaderDataType::Float3x3:
		{
			glm::mat<3, 3, float>* dataV = (glm::mat<3, 3, float>*)data.data();
			*dataV = node.as<glm::mat<3, 3, float>>();
			break;
		}
		case ShaderDataType::Float4x4:
		{
			glm::mat<4, 4, float>* dataV = (glm::mat<4, 4, float>*)data.data();
			*dataV = node.as<glm::mat<4, 4, float>>();
			break;
		}
		case ShaderDataType::Int:
		{
			int* dataV = (int*)data.data();
			*dataV = node.as<int>();
			break;
		}
		case ShaderDataType::Int2:
		{
			glm::vec<2, int>* dataV = (glm::vec<2, int>*)data.data();
			*dataV = node.as<glm::vec<2, int>>();
			break;
		}
		case ShaderDataType::Int3:
		{
			glm::vec<3, int>* dataV = (glm::vec<3, int>*)data.data();
			*dataV = node.as<glm::vec<3, int>>();
			break;
		}
		case ShaderDataType::Int4:
		{
			glm::vec<4, int>* dataV = (glm::vec<4, int>*)data.data();
			*dataV = node.as<glm::vec<4, int>>();
			break;
		}
		}
	}

	static void DeserializeDynamicDataLayout(YAML::Node& node, const BufferLayout& layout, std::vector<unsigned char>& data)
	{
		uint32_t size = layout.GetStride();
		data.reserve(size);
		uint32_t offset = 0;
		for (auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			{
				float* dataV = (float*)data.data()[offset];
				*dataV = node[element.Name.c_str()].as<float>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Float2:
			{
				glm::vec<2, float>* dataV = (glm::vec<2, float>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::vec<2, float>>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Float3:
			{
				glm::vec<3, float>* dataV = (glm::vec<3, float>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::vec<3, float>>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Float4:
			{
				glm::vec<4, float>* dataV = (glm::vec<4, float>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::vec<4, float>>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Float3x3:
			{
				glm::mat<3, 3, float>* dataV = (glm::mat<3, 3, float>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::mat<3, 3, float>>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Float4x4:
			{
				glm::mat<4, 4, float>* dataV = (glm::mat<4, 4, float>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::mat<4, 4, float>>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Int:
			{
				int* dataV = (int*)data.data();
				*dataV = node[element.Name.c_str()].as<int>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Int2:
			{
				glm::vec<2, int>* dataV = (glm::vec<2, int>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::vec<2, int>>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Int3:
			{
				glm::vec<3, int>* dataV = (glm::vec<3, int>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::vec<3, int>>();
				offset += element.Size;
				break;
			}
			case ShaderDataType::Int4:
			{
				glm::vec<4, int>* dataV = (glm::vec<4, int>*)data.data();
				*dataV = node[element.Name.c_str()].as<glm::vec<4, int>>();
				offset += element.Size;
				break;
			}
			}
		}
	}

	template<typename T>
	static void DeserializeAssetFormate(YAML::Node& nodeE, Ref<T>* entityC)
	{
		if (nodeE)
		{
			std::string path = nodeE["Path"].as<std::string>();
			AssetHandle handle = nodeE["AssetHandle"].as<uint64_t>();
			AssetHandle handleP = Project::GetActive()->GetEditorAssetManger()->GetAssetHandle(path);
			RY_CORE_ASSERT(handleP == handle, "Not Simulare Asset Handle!");
			*entityC = AssetManager::GetAsset<T>(handleP);
		}
	}
#endif

	bool Texture2DSerialiazer::Serlize(const std::filesystem::path& filepath, Ref<Texture> texture)
	{
		std::string fromtasChar[] = {
			"None",
			"R8",
			"RGB8",
			"RGBA8",
			"RGB32F",
			"RED_INTEGER",
			"DEPTH24STENCIL8"
		};
		std::string wrappingChar[] = {
			"None",
			"Repeate",
			"MirrorRepeate",
			"ClampEdge",
			"ClampBorder",
			"MirrorClampEdge"
		};

		std::string filteringChar[] = {
			"None",
			"Linear",
			"Nearest"
		};

		std::string targetChar[] = {
			"None",
			"Texture1D", "Texture2D", "Texture3D",
			"TextureRectAngle",
			"TextureBuffer",
			"TextureCubeMap",

			"ImageTexture",
			"FrameBufferTexture"
		};

		const auto& config = texture->GetSpecification();
		std::vector<unsigned char> dataVec = texture->GetCurrentRenderData();
		const uint32_t dataSize = texture->GetCurrentRenderDataSize();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Texture" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Width" << YAML::Value << config.Width;
				out << YAML::Key << "Height" << YAML::Value << config.Height;
				out << YAML::Key << "Format" << YAML::Value << fromtasChar[(int)config.Format];
				out << YAML::Key << "Samples" << YAML::Value << config.Samples;
				out << YAML::Key << "WrappingSpec" << YAML::Value;
				{
					out << YAML::BeginMap;
					out << YAML::Key << "S" << YAML::Value << wrappingChar[(int)config.WrappingSpec.S];
					out << YAML::Key << "T" << YAML::Value << wrappingChar[(int)config.WrappingSpec.T];
					out << YAML::Key << "R" << YAML::Value << wrappingChar[(int)config.WrappingSpec.R];
					out << YAML::EndMap;
				}
				out << YAML::Key << "FilteringMode" << YAML::Value << filteringChar[(int)config.FilteringMode];
				out << YAML::Key << "Target" << YAML::Value << targetChar[(int)config.Target];
				out << YAML::Key << "DataSize" << YAML::Value << dataVec.size();

			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout << "\n--START DATA BINRY--\n";
		fout.write(reinterpret_cast<const char*>(dataVec.data()), dataVec.size());
		return true;
	}

	Ref<Texture> Texture2DSerialiazer::Deserlize(const std::filesystem::path& filepath)
	{
	
		std::map<std::string, ImageFormat> fromatMap = {
			{ "None",				ImageFormat::None},
			{ "R8",					ImageFormat::R8 },
			{ "RGB8",				ImageFormat::RGB8 },
			{ "RGBA8",				ImageFormat::RGBA8 },
			{ "RGB32F",				ImageFormat::RGB32F },
			{ "RED_INTEGER",		ImageFormat::RED_INTEGER },
			{ "DEPTH24STENCIL8",	ImageFormat::DEPTH24STENCIL8 }
		};

		std::map<std::string, TextureWrappingMode> wrappingMap = {
			{ "None",			 TextureWrappingMode::None},
			{ "Repeate",		 TextureWrappingMode::Repeate},
			{ "MirrorRepeate",	 TextureWrappingMode::MirrorRepeate},
			{ "ClampEdge",		 TextureWrappingMode::ClampEdge},
			{ "ClampBorder",	 TextureWrappingMode::ClampBorder},
			{ "MirrorClampEdge", TextureWrappingMode::MirrorClampEdge}
		};

		std::map<std::string, TextureFilteringMode> filteringMap = {
			{ "None",	 TextureFilteringMode::None },
			{ "Linear",  TextureFilteringMode::Linear },
			{ "Nearest", TextureFilteringMode::Nearest }
		};

		std::map<std::string, TextureTarget> targetMap = {
			{ "None",				(TextureTarget)0 },
			{ "Texture1D",			(TextureTarget)1 },
			{ "Texture2D", 			(TextureTarget)2 },
			{ "Texture3D",			(TextureTarget)3 },
			{ "TextureRectAngle",	(TextureTarget)4 },
			{ "TextureBuffer",		(TextureTarget)5 },
			{ "TextureCubeMap",		(TextureTarget)6 },
			{ "ImageTexture",		(TextureTarget)7 },
			{ "FrameBufferTexture",	(TextureTarget)8 }
		};

		TextureSpecification spec;
		YAML::Node data;
		std::ifstream file(filepath, std::ios::binary);
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
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		auto TextureNode = data["Texture"];
		if (!TextureNode)
			return nullptr;

		spec.Width = TextureNode["Width"].as<uint32_t>();
		spec.Height = TextureNode["Height"].as<uint32_t>();
		spec.Format = fromatMap[TextureNode["Format"].as<std::string>()];
		spec.Samples = TextureNode["Samples"].as<uint32_t>();
		auto wrappingSpecNode = TextureNode["WrappingSpec"];
		if (wrappingSpecNode)
		{
			auto T_Node = wrappingSpecNode["T"];
			auto S_Node = wrappingSpecNode["S"];
			auto R_Node = wrappingSpecNode["R"];
			if (T_Node)
				spec.WrappingSpec.T = wrappingMap[T_Node.as<std::string>()];
			if (S_Node)
				spec.WrappingSpec.S = wrappingMap[S_Node.as<std::string>()];
			if (R_Node)
				spec.WrappingSpec.R = wrappingMap[R_Node.as<std::string>()];
		}
		auto filteringModeNode = TextureNode["FilteringMode"];
		if (filteringModeNode)
			spec.FilteringMode = filteringMap[filteringModeNode.as<std::string>()];
		auto targetNode = TextureNode["Target"];
		if (targetNode)
			spec.Target = targetMap[targetNode.as<std::string>()];
		auto dataSizeNode = TextureNode["DataSize"];
		uint32_t size = 0;
		std::vector<unsigned char> dataByte;
		if (dataSizeNode)
		{
			size = dataSizeNode.as<size_t>();
			// dataByte = new unsigned char(size);
			dataByte.resize(size);
			file.read(reinterpret_cast<char*>(dataByte.data()), dataByte.size());
				
		}
		
		Ref<Texture> texture;
		if ((dataByte.size() != 0) && (size != 0))
		{
			texture = Texture::Create(spec, dataByte.data(), size);
		}
		else
		{
			texture = Texture::Create(spec);
		}
		file.close();
		
		return texture;
	}
}
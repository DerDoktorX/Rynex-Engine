#include "rypch.h"
#include "SceneSerializer.h"

#include "Rynex/Scene/Entity.h"
#include "Rynex/Scene/Components.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Renderer/API/Buffer.h"


#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

#include "YAML.h"


#define YAML_AKTIV 1


namespace YAML {

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
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{

			out << v[i].x << v[i].y << v[i].z;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat4& v)
	{
		out << Flow;
		out << BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{

			out << v[i].x << v[i].y << v[i].z << v[i].w;
		}
		out << EndSeq;
		return out;
	}



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
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{

			out << v[i].x << v[i].y << v[i].z;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat<4, 4, int>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{
			out << v[i].x << v[i].y << v[i].z << v[i].w;
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
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 3; i++)
		{

			out << v[i].x << v[i].y << v[i].z;
		}
		out << YAML::EndSeq;
		return out;
	}

	Emitter& operator<<(Emitter& out, const glm::mat<4, 4, uint32_t>& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint8_t i = 0; i < 4; i++)
		{

			out << v[i].x << v[i].y << v[i].z << v[i].w;
		}
		out << YAML::EndSeq;
		return out;
	}
#if YAML_AKTIV
#if 0
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

#else

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
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
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
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
	struct convert<glm::mat3>
	{
		static Node encode(const glm::mat3& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 3; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);

			}
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::mat3& rhs)
		{
			if (!node.IsSequence() || node.size() != 9)
				return false;

			for (uint8_t i = 0; i < 3; i++)
			{
				rhs[i].x = node[0 + i * 3].as<float>();
				rhs[i].y = node[0 + i * 3].as<float>();
				rhs[i].z = node[0 + i * 3].as<float>();
			}
			return true;
		}
	};

	template<>
	struct convert<glm::mat4>
	{
		static Node encode(const glm::mat4& rhs)
		{
			Node node;
			for (uint8_t i = 0; i < 4; i++)
			{
				node.push_back(rhs[i].x);
				node.push_back(rhs[i].y);
				node.push_back(rhs[i].z);
				node.push_back(rhs[i].w);
			}
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::mat4& rhs)
		{
			if (!node.IsSequence() || node.size() != 16)
				return false;

			for (uint8_t i = 0; i < 4; i++)
			{
				rhs[i].x = node[0 + i * 4].as<float>();
				rhs[i].y = node[1 + i * 4].as<float>();
				rhs[i].z = node[2 + i * 4].as<float>();
				rhs[i].w = node[3 + i * 4].as<float>();
			}
			return true;
		}
	};
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
				rhs[i].x = node[0 + i * 3].as<int>();
				rhs[i].y = node[1 + i * 3].as<int>();
				rhs[i].z = node[1 + i * 3].as<int>();
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
				rhs[i].x = node[0 + i * 4].as<int>();
				rhs[i].y = node[0 + i * 4].as<int>();
				rhs[i].z = node[0 + i * 4].as<int>();
				rhs[i].w = node[0 + i * 4].as<int>();
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

			}
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::mat<3, 3, uint32_t>& rhs)
		{
			if (!node.IsSequence() || node.size() != 9)
				return false;

			for (uint8_t i = 0; i < 3; i++)
			{
				rhs[i].x = node[0 + i * 3].as<uint32_t>();
				rhs[i].y = node[1 + i * 3].as<uint32_t>();
				rhs[i].z = node[2 + i * 3].as<uint32_t>();
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

			}
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::mat<4, 4, uint32_t>& rhs)
		{
			if (!node.IsSequence() || node.size() != 16)
				return false;

			for (uint8_t i = 0; i < 4; i++)
			{
				rhs[i].x = node[0 + i * 4].as<uint32_t>();
				rhs[i].y = node[1 + i * 4].as<uint32_t>();
				rhs[i].z = node[2 + i * 4].as<uint32_t>();
				rhs[i].w = node[3 + i * 4].as<uint32_t>();
			}
			return true;
		}
	};
#endif
}

namespace Rynex {

	struct EntitySerialData
	{
		uint64_t UUID;
		uint32_t Component = 0;
		TagComponent TagComponent;
		TransformComponent TransformComponent;
		CameraComponent CameraComponent;
		ScriptComponent ScriptComponent;
		SpriteRendererComponent SpriteRendererComponent;
		GeomtryComponent GeomtryComponent;
		MaterialComponent MaterialComponent;
		RealtionShipComponent RealtionShipComponent;
		ModelMatrixComponent ModelMatrixComponent;
		StaticMeshComponent StaticMeshComponent;
		DynamicMeshComponent DynamicMeshComponent;
	};

	struct SceneSerialData
	{
		uint32_t EntitiySize;
		std::vector<EntitySerialData> Enitity;
	};
namespace Utils
{
	static void SerializerDynamicData(YAML::Emitter& out, const std::string& name, ShaderDataType type, const std::vector<unsigned char>& data)
	{
		switch (type)
		{
		case ShaderDataType::Float:
		{
			float* value = (float*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float2:
		{
			glm::vec<2, float>* value = (glm::vec<2, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float3:
		{
			glm::vec<3, float>* value = (glm::vec<3, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float4:
		{
			glm::vec<4, float>* value = (glm::vec<4, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float3x3:
		{
			glm::mat<3, 3, float>* value = (glm::mat<3, 3, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Float4x4:
		{
			glm::mat<4, 4, float>* value = (glm::mat<4, 4, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int:
		{
			int* value = (int*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int2:
		{
			glm::vec<2, float>* value = (glm::vec<2, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int3:
		{
			glm::vec<3, float>* value = (glm::vec<3, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		case ShaderDataType::Int4:
		{
			glm::vec<4, float>* value = (glm::vec<4, float>*)data.data();
			out << YAML::Key << name.c_str() << YAML::Value << *value;
			break;
		}
		}
	}

	static void SerializerDynamicDataLayout(YAML::Emitter& out, const std::string& name, const BufferLayout& layout, const std::vector<unsigned char>& data)
	{
		out << YAML::Key << name.c_str() << YAML::BeginSeq;


		for (uint64_t offset = 0, size = data.size(); offset < size; offset += layout.GetStride())
		{
			for (const BufferElement& ellement : layout)
			{

				switch (ellement.Type)
				{
				case ShaderDataType::Float:
				{
					float* value = (float*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float2:
				{
					glm::vec<2, float>* value = (glm::vec<2, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float3:
				{
					glm::vec<3, float>* value = (glm::vec<3, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float4:
				{
					glm::vec<4, float>* value = (glm::vec<4, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float3x3:
				{
					glm::mat<3, 3, float>* value = (glm::mat<3, 3, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Float4x4:
				{
					glm::mat<4, 4, float>* value = (glm::mat<4, 4, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int:
				{
					int* value = (int*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int2:
				{
					glm::vec<2, float>* value = (glm::vec<2, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int3:
				{
					glm::vec<3, float>* value = (glm::vec<3, float>*)(data.data() + offset);
					out << YAML::Value << *value;
					break;
				}
				case ShaderDataType::Int4:
				{
					glm::vec<4, float>* value = (glm::vec<4, float>*)(data.data() + offset);
					out << YAML::Value << *value;
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
		out << YAML::Key << "Handle" << handle;
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
			glm::vec<2, float>* dataV = (glm::vec<2, float>*)data.data();
			*dataV = node.as<glm::vec<2, float>>();
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
		data.resize(size * node.size());
		uint32_t offset = 0;
		uint32_t index = 0;
		const std::vector<BufferElement>& elements = layout.GetElements();
		uint32_t elementsSize = elements.size();

		for (YAML::detail::iterator_value& element : node)
		{

			switch (elements[(index % elementsSize)].Type)
			{
			case ShaderDataType::Float:
			{
				float* dataV = (float*)(data.data() + offset);
				*dataV = element.as<float>();
				break;
			}
			case ShaderDataType::Float2:
			{
				glm::vec<2, float>* dataV = (glm::vec<2, float>*)(data.data() + offset);
				*dataV = element.as<glm::vec<2, float>>();
				break;
			}
			case ShaderDataType::Float3:
			{
				glm::vec<3, float>* dataV = (glm::vec<3, float>*)(data.data() + offset);
				*dataV = element.as<glm::vec<3, float>>();
				break;
			}
			case ShaderDataType::Float4:
			{
				glm::vec<4, float>* dataV = (glm::vec<4, float>*)(data.data() + offset);
				*dataV = element.as<glm::vec<4, float>>();
				break;
			}
			case ShaderDataType::Float3x3:
			{
				glm::mat<3, 3, float>* dataV = (glm::mat<3, 3, float>*)(data.data() + offset);
				*dataV = element.as<glm::mat<3, 3, float>>();
				break;
			}
			case ShaderDataType::Float4x4:
			{
				glm::mat<4, 4, float>* dataV = (glm::mat<4, 4, float>*)(data.data() + offset);
				*dataV = element.as<glm::mat<4, 4, float>>();
				break;
			}
			case ShaderDataType::Int:
			{
				int* dataV = (int*)(data.data() + offset);
				*dataV = element.as<int>();
				break;
			}
			case ShaderDataType::Int2:
			{
				glm::vec<2, int>* dataV = (glm::vec<2, int>*)(data.data() + offset);
				*dataV = element.as<glm::vec<2, int>>();
				break;
			}
			case ShaderDataType::Int3:
			{
				glm::vec<3, int>* dataV = (glm::vec<3, int>*)(data.data() + offset);
				*dataV = element.as<glm::vec<3, int>>();
				break;
			}
			case ShaderDataType::Int4:
			{
				glm::vec<4, int>* dataV = (glm::vec<4, int>*)(data.data() + offset);
				*dataV = element.as<glm::vec<4, int>>();
				break;
			}
			}
			index++;
		}
	}
	
	template< typename T>
	static bool DeserializeAssetFormate(YAML::Node& nodeE, Ref<T>* entityC, bool async)
	{
		if (nodeE)
		{
			std::string path = nodeE["Path"].as<std::string>();
			AssetHandle handle = nodeE["Handle"].as<uint64_t>();
			AssetHandle handleP = Project::GetActive()->GetEditorAssetManger()->GetAssetHandle(path);
			RY_CORE_ASSERT(handleP == handle, "Not Simulare Asset Handle!");
			if(async)
			{
				if (handle != 0)
					AssetManager::GetAssetAsync<T>(handle, entityC);
				else
					AssetManager::GetAssetAsync<T>(handleP, entityC);
			}
			else
			{
				if (handle != 0)
					*entityC = AssetManager::GetAsset<T>( handle );
				else
					*entityC = AssetManager::GetAsset<T>(handleP);
			}
			return handleP != handle;
		}
		return false;
	}
}

	


	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}


	static void SerializerEntity(YAML::Emitter& out, Entity entity, const Ref<Scene>& scene)
	{
#if RY_EDITOR_ASSETMANGER_THREADE
		Ref<EditorAssetManegerThreade> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#else
		Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#endif
		

		RY_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Error: Entity has not IDComponent");
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			std::string& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			TransformComponent& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Transaltion" << YAML::Value << tc.Transaltion;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			CameraComponent& cc = entity.GetComponent<CameraComponent>();
			SceneCamera& camera = cc.Camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectivVerticleFOV" << YAML::Value << camera.GetPerspectivVerticleFOV();
			out << YAML::Key << "PerspectivNearClipe" << YAML::Value << camera.GetPerspectivNearClipe();
			out << YAML::Key << "PerspectivFarClipe" << YAML::Value << camera.GetPerspectivFarClipe();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNearClipe" << YAML::Value << camera.GetOrthographicNearClipe();
			out << YAML::Key << "OrthographicFarClipe" << YAML::Value << camera.GetOrthographicFarClipe();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRotaion" << YAML::Value << cc.FixedAspectRotaion;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			ScriptComponent& sc = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << sc.Name;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			SpriteRendererComponent& sc = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << sc.Color;
#if RY_DISABLE_WEAK_PTR
			if (sc.Texture)
			{
				SerializerAssetFormate(out, "Texture", sc.Texture->Handle);
			}
#else
			if (Ref<Texture> tex = sc.Texture.lock())
			{
				Utils::SerializerAssetFormate(out, "Texture", tex->Handle);
			}
#endif
			out << YAML::EndMap;
		}

		if (entity.HasComponent<GeomtryComponent>())
		{
			GeomtryComponent& gc = entity.GetComponent<GeomtryComponent>();
			out << YAML::Key << "GeomtryComponent";
			out << YAML::BeginMap;
			if (gc.Geometry)
			{
				out << YAML::Key << "Primtiv" << YAML::Value << (int)gc.Geometry->GetPrimitv();
				out << YAML::Key << "VertexBuffers" << YAML::Value;
				out << YAML::BeginSeq;
				if (gc.Geometry->GetVertexBuffers().size())
				{

					for (const Ref<VertexBuffer>& buffer : gc.Geometry->GetVertexBuffers())
					{
						const BufferLayout& layout = buffer->GetLayout();
						out << YAML::BeginMap;
						out << YAML::Key << "Laoute" << YAML::Value;
						out << YAML::BeginSeq;
						for (const BufferElement& element : layout)
						{
							out << YAML::BeginMap;
							out << YAML::Key << "Name" << element.Name;
							out << YAML::Key << "Type" << (int)element.Type;
							out << YAML::Key << "Size" << element.Size;
							out << YAML::Key << "Offset" << element.Offset;
							out << YAML::Key << "Normilized" << element.Normilized;
							out << YAML::EndMap;
						}
						out << YAML::EndSeq;
						const std::vector<unsigned char>& buferData = buffer->GetBufferData();
						Utils::SerializerDynamicDataLayout(out, "BuferData", layout, buferData);
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
				}
				out << YAML::Key << "IndexBuffer" << YAML::Value;
				out << YAML::BeginMap;
				if (gc.Geometry->GetIndexBuffers())
				{

					{
						Ref<IndexBuffer> indexBuffer = gc.Geometry->GetIndexBuffers();
						std::vector<uint32_t> dataIndex = indexBuffer->GetBufferData();

						out << YAML::Key << "Size-Byte" << YAML::Value << dataIndex.size();
						out << YAML::Key << "Byts" << YAML::Value << 4ull;
						out << YAML::Key << "Indicies" << YAML::Flow << YAML::BeginSeq;
						for (uint64_t i = 0; i < dataIndex.size(); i += 4ull)
						{
							uint32_t* value = (uint32_t*)(dataIndex.data() + i);
							out << YAML::Value << *value;
						}
						out << YAML::EndSeq;
					}
				}
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

#if 0
		if (entity.HasComponent<MaterialComponent>())
		{
			out << YAML::Key << "MaterialComponent" << YAML::Value;
			out << YAML::BeginMap;
			MaterialComponent& mc = entity.GetComponent<MaterialComponent>();
			out << YAML::Key << "Color" << YAML::Value << mc.Color;
			out << YAML::Key << "AlgorithmFlags" << YAML::Value << mc.AlgorithmFlags;
			if (mc.Texture)
			{
				SerializerAssetFormate(out, "Texture", mc.Texture->Handle);
			}
			if (mc.Shader)
			{
				SerializerAssetFormate(out, "Shader", mc.Shader->Handle);
			}
			if (mc.UniformLayoute.size())
			{
				out << YAML::Key << "UniformLayoute";
				out << YAML::BeginSeq;
				for (UniformElement& ellement : mc.UniformLayoute)
				{
					out << YAML::BeginMap;
					out << YAML::Key << "Name" << YAML::Value << ellement.Name.c_str();
					out << YAML::Key << "Type" << YAML::Value << (int)ellement.Type;
					out << YAML::Key << "ShaderResourceType" << YAML::Value << (int)ellement.ShResourceType;
					out << YAML::Key << "GloblelResurce" << YAML::Value << ellement.GloblelResurce;
					if (!ellement.GloblelResurce)
						SerializerDynamicData(out, "LocalResurce", ellement.Type, ellement.LocalResurce);
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap;

		}
#endif

		if (entity.HasComponent<RealtionShipComponent>())
		{
			RealtionShipComponent& rSc = entity.GetComponent<RealtionShipComponent>();

			UUID parent = rSc.ParentID;
			UUID previus = rSc.PreviusID;
			UUID first = rSc.FirstID;
			UUID next = rSc.NextID;
			out << YAML::Key << "RealtionShipComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Previus" << YAML::Value << previus;
			out << YAML::Key << "First" << YAML::Value << first;
			out << YAML::Key << "Next" << YAML::Value << next;
			out << YAML::Key << "Parent" << YAML::Value << parent;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ModelMatrixComponent>())
		{
			out << YAML::Key << "ModelMatrixComponent";
			out << YAML::BeginMap;
			ModelMatrixComponent& modelMatC = entity.GetComponent<ModelMatrixComponent>();
			out << YAML::Key << "Locale" << YAML::Value << modelMatC.Locale;
			out << YAML::Key << "Globle" << YAML::Value << modelMatC.Globle;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ViewMatrixComponent>())
		{
			out << YAML::Key << "ViewMatrixComponent";
			out << YAML::BeginMap;
			ViewMatrixComponent& viewMatC = entity.GetComponent<ViewMatrixComponent>();
			out << YAML::Key << "Locale" << YAML::Value << viewMatC.Locale;
			out << YAML::Key << "Globle" << YAML::Value << viewMatC.Globle;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;
			MeshComponent& meshC = entity.GetComponent<MeshComponent>();
			if (meshC.ModelR)
			{
				Utils::SerializerAssetFormate(out, "Model", meshC.ModelR->Handle);
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<StaticMeshComponent>())
		{
			out << YAML::Key << "StaticMeshComponent";
			out << YAML::BeginMap;
			StaticMeshComponent& meshS_C = entity.GetComponent<StaticMeshComponent>();
			if (meshS_C.ModelR)
			{
				Utils::SerializerAssetFormate(out, "Model-S", meshS_C.ModelR->Handle);
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<DynamicMeshComponent>())
		{
			out << YAML::Key << "DynamicMeshComponent";
			out << YAML::BeginMap;
			DynamicMeshComponent& meshD_C = entity.GetComponent<DynamicMeshComponent>();
#if RY_MODEL_NODE
			if (meshD_C.MeshD.size()>0)
			{
				RealtionShipComponent& rSc = entity.GetComponent<RealtionShipComponent>();
				Entity parent = scene->GetEntitiyByUUID(rSc.ParentID);
				MeshComponent& meshDP_C = parent.GetComponent<MeshComponent>();
				uint32_t index = 0;
				for (const auto& nodeP : meshDP_C.ModelR->GetNodes())
				{
					if ( nodeP.Meshes[0] == meshD_C.MeshD[0].MeshR)
						break;
					index++;
				}
				out << YAML::Key << "Model-D";
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << meshDP_C.ModelR->Handle;
				out << YAML::Key << "Path" << YAML::Value << editorAssetManger->GetMetadata(meshDP_C.ModelR->Handle).FilePath.string();
				out << YAML::Key << "Index" << YAML::Value << index;
				out << YAML::EndMap;
			}
#else
			if (meshD_C.MeshR)
			{
				RealtionShipComponent& rSc = entity.GetComponent<RealtionShipComponent>();
				Entity parent = scene->GetEntitiyByUUID(rSc.ParentID);
				MeshComponent& meshDP_C = parent.GetComponent<MeshComponent>();
				uint32_t index = 0;
				for (const Ref<Mesh>& meshP : meshDP_C.ModelR->GetMeshes())
				{
					if (meshP == meshD_C.MeshR)
						break;
					index++;
				}
				out << YAML::Key << "Model-D";
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << meshDP_C.ModelR->Handle;
				out << YAML::Key << "Path" << YAML::Value << editorAssetManger->GetMetadata(meshDP_C.ModelR->Handle).FilePath.string();
				out << YAML::Key << "Index" << YAML::Value << index;
				out << YAML::EndMap;
			}
#endif
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TextComponent>())
		{
			out << YAML::Key << "TextComponent";
			out << YAML::BeginMap;
			TextComponent& textC = entity.GetComponent<TextComponent>();
			
				
			out << YAML::Key << "TextString" << YAML::Value << textC.TextString.c_str();
			out << YAML::Key << "Color" << YAML::Value << textC.Color;
			out << YAML::Key << "Kerning" << YAML::Value << textC.Kerning;
			out << YAML::Key << "LineSpacing" << YAML::Value << textC.LineSpacing;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<AmbientLigthComponent>())
		{
			out << YAML::Key << "AmbientLigthComponent";
			out << YAML::BeginMap;
			AmbientLigthComponent& ambientC = entity.GetComponent<AmbientLigthComponent>();


			out << YAML::Key << "Color" << YAML::Value << ambientC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << ambientC.Intensitie;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<DrirektionleLigthComponent>())
		{
			out << YAML::Key << "DrirektionleLigthComponent";
			out << YAML::BeginMap;
			DrirektionleLigthComponent& drirektionleC = entity.GetComponent<DrirektionleLigthComponent>();

			out << YAML::Key << "Color" << YAML::Value << drirektionleC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << drirektionleC.Intensitie;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<PointLigthComponent>())
		{
			out << YAML::Key << "PointLigthComponent";
			out << YAML::BeginMap;
			PointLigthComponent& pointC = entity.GetComponent<PointLigthComponent>();

			out << YAML::Key << "Color" << YAML::Value << pointC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << pointC.Intensitie;
			out << YAML::Key << "Distence" << YAML::Value << pointC.Distence;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpotLigthComponent>())
		{
			out << YAML::Key << "SpotLigthComponent";
			out << YAML::BeginMap;
			SpotLigthComponent& spotC = entity.GetComponent<SpotLigthComponent>();

			out << YAML::Key << "Color" << YAML::Value << spotC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << spotC.Intensitie;
			out << YAML::Key << "Distence" << YAML::Value << spotC.Distence;
			out << YAML::Key << "Inner" << YAML::Value << spotC.Inner;
			out << YAML::Key << "Outer" << YAML::Value << spotC.Outer;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<FrameBufferComponent>())
		{
			out << YAML::Key << "FrameBufferComponent";
			{
				out << YAML::BeginMap;
				FrameBufferComponent& frameC = entity.GetComponent<FrameBufferComponent>();

				out << YAML::Key << "Framebuffer" << YAML::Value;
				{
					out << YAML::BeginMap;
					const FramebufferSpecification& spec = frameC.FrameBuffer->GetFramebufferSpecification();
					out << YAML::Key << "Width" << YAML::Value << spec.Width;
					out << YAML::Key << "Height" << YAML::Value << spec.Height;

					{
						out << YAML::Key << "Attachments";
						out << YAML::BeginSeq;
						for (const FramebufferTextureSpecification& framTexSpec : spec.Attachments.Attachments)
						{
							// out << YAML::Key << "TextureAssetHandle" << YAML::Value << AssetHandle();
							out << YAML::BeginMap;
							out << YAML::Key << "TextureFormat" << YAML::Value << (int)framTexSpec.TextureFormat;
							out << YAML::Key << "TextureWrapping" << YAML::Value;
							out << YAML::BeginMap;
							{
								out << YAML::Key << "R" << YAML::Value << (int)framTexSpec.TextureWrapping.R;
								out << YAML::Key << "T" << YAML::Value << (int)framTexSpec.TextureWrapping.T;
								out << YAML::Key << "S" << YAML::Value << (int)framTexSpec.TextureWrapping.S;
							}
							out << YAML::EndMap;
							out << YAML::Key << "TextureFiltering" << YAML::Value << (int)framTexSpec.TextureFiltering;
							out << YAML::Key << "Samples" << YAML::Value << framTexSpec.Samples;
							out << YAML::EndMap;
						}
						out << YAML::EndSeq;
					}
					out << YAML::Key << "Samples" << YAML::Value << spec.Samples;
					out << YAML::Key << "SwapChainTarget" << YAML::Value << spec.SwapChainTarget;
					out << YAML::EndMap;
				}

				out << YAML::Key << "ClearColor" << YAML::Value << frameC.ClearColor;
				out << YAML::Key << "FramebufferSize" << YAML::Value << (int)frameC.FramebufferSize;
				

				out << YAML::EndMap;
			}
		}

		out << YAML::EndMap;
	}

	static void SerializerEntityAsync(YAML::Emitter& out, Entity entity, const Ref<Scene>& scene)
	{
#if RY_EDITOR_ASSETMANGER_THREADE
		Ref<EditorAssetManegerThreade> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#else
		Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#endif


		RY_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Error: Entity has not IDComponent");
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			std::string& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			TransformComponent& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Transaltion" << YAML::Value << tc.Transaltion;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			CameraComponent& cc = entity.GetComponent<CameraComponent>();
			SceneCamera& camera = cc.Camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectivVerticleFOV" << YAML::Value << camera.GetPerspectivVerticleFOV();
			out << YAML::Key << "PerspectivNearClipe" << YAML::Value << camera.GetPerspectivNearClipe();
			out << YAML::Key << "PerspectivFarClipe" << YAML::Value << camera.GetPerspectivFarClipe();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNearClipe" << YAML::Value << camera.GetOrthographicNearClipe();
			out << YAML::Key << "OrthographicFarClipe" << YAML::Value << camera.GetOrthographicFarClipe();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRotaion" << YAML::Value << cc.FixedAspectRotaion;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			ScriptComponent& sc = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << sc.Name;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			SpriteRendererComponent& sc = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << sc.Color;
#if RY_DISABLE_WEAK_PTR
			if (sc.Texture)
			{
				SerializerAssetFormate(out, "Texture", sc.Texture->Handle);
			}
#else
			if (Ref<Texture> tex = sc.Texture.lock())
			{
				Utils::SerializerAssetFormate(out, "Texture", tex->Handle);
			}
#endif
			out << YAML::EndMap;
		}

		if (entity.HasComponent<GeomtryComponent>())
		{
			GeomtryComponent& gc = entity.GetComponent<GeomtryComponent>();
			out << YAML::Key << "GeomtryComponent";
			out << YAML::BeginMap;
			if (gc.Geometry)
			{
				out << YAML::Key << "Primtiv" << YAML::Value << (int)gc.Geometry->GetPrimitv();
				out << YAML::Key << "VertexBuffers" << YAML::Value;
				out << YAML::BeginSeq;
				if (gc.Geometry->GetVertexBuffers().size())
				{

					for (const Ref<VertexBuffer>& buffer : gc.Geometry->GetVertexBuffers())
					{
						const BufferLayout& layout = buffer->GetLayout();
						out << YAML::BeginMap;
						out << YAML::Key << "Laoute" << YAML::Value;
						out << YAML::BeginSeq;
						for (const BufferElement& element : layout)
						{
							out << YAML::BeginMap;
							out << YAML::Key << "Name" << element.Name;
							out << YAML::Key << "Type" << (int)element.Type;
							out << YAML::Key << "Size" << element.Size;
							out << YAML::Key << "Offset" << element.Offset;
							out << YAML::Key << "Normilized" << element.Normilized;
							out << YAML::EndMap;
						}
						out << YAML::EndSeq;
						const std::vector<unsigned char>& buferData = buffer->GetBufferData();
						Utils::SerializerDynamicDataLayout(out, "BuferData", layout, buferData);
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
				}
				out << YAML::Key << "IndexBuffer" << YAML::Value;
				out << YAML::BeginMap;
				if (gc.Geometry->GetIndexBuffers())
				{

					{
						Ref<IndexBuffer> indexBuffer = gc.Geometry->GetIndexBuffers();
						std::vector<uint32_t> dataIndex = indexBuffer->GetBufferData();

						out << YAML::Key << "Size-Byte" << YAML::Value << dataIndex.size();
						out << YAML::Key << "Byts" << YAML::Value << 4ull;
						out << YAML::Key << "Indicies" << YAML::Flow << YAML::BeginSeq;
						for (uint64_t i = 0; i < dataIndex.size(); i += 4ull)
						{
							uint32_t* value = (uint32_t*)(dataIndex.data() + i);
							out << YAML::Value << *value;
						}
						out << YAML::EndSeq;
					}
				}
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

#if 0
		if (entity.HasComponent<MaterialComponent>())
		{
			out << YAML::Key << "MaterialComponent" << YAML::Value;
			out << YAML::BeginMap;
			MaterialComponent& mc = entity.GetComponent<MaterialComponent>();
			out << YAML::Key << "Color" << YAML::Value << mc.Color;
			out << YAML::Key << "AlgorithmFlags" << YAML::Value << mc.AlgorithmFlags;

			if (mc.Texture)
			{
				SerializerAssetFormate(out, "Texture", mc.Texture->Handle);
			}
			if (mc.Shader)
			{
				SerializerAssetFormate(out, "Shader", mc.Shader->Handle);
			}
			if (mc.UniformLayoute.size())
			{
				out << YAML::Key << "UniformLayoute";
				out << YAML::BeginSeq;
				for (UniformElement& ellement : mc.UniformLayoute)
				{
					out << YAML::BeginMap;
					out << YAML::Key << "Name" << YAML::Value << ellement.Name.c_str();
					out << YAML::Key << "Type" << YAML::Value << (int)ellement.Type;
					out << YAML::Key << "ShaderResourceType" << YAML::Value << (int)ellement.ShResourceType;
					out << YAML::Key << "GloblelResurce" << YAML::Value << ellement.GloblelResurce;
					if (!ellement.GloblelResurce)
						SerializerDynamicData(out, "LocalResurce", ellement.Type, ellement.LocalResurce);
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap;

		}
#endif

		if (entity.HasComponent<RealtionShipComponent>())
		{
			RealtionShipComponent& rSc = entity.GetComponent<RealtionShipComponent>();

			UUID parent = rSc.ParentID;
			UUID previus = rSc.PreviusID;
			UUID first = rSc.FirstID;
			UUID next = rSc.NextID;
			out << YAML::Key << "RealtionShipComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Previus" << YAML::Value << previus;
			out << YAML::Key << "First" << YAML::Value << first;
			out << YAML::Key << "Next" << YAML::Value << next;
			out << YAML::Key << "Parent" << YAML::Value << parent;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ModelMatrixComponent>())
		{
			out << YAML::Key << "ModelMatrixComponent";
			out << YAML::BeginMap;
			ModelMatrixComponent& modelMatC = entity.GetComponent<ModelMatrixComponent>();
			out << YAML::Key << "Locale" << YAML::Value << modelMatC.Locale;
			out << YAML::Key << "Globle" << YAML::Value << modelMatC.Globle;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ViewMatrixComponent>())
		{
			out << YAML::Key << "ViewMatrixComponent";
			out << YAML::BeginMap;
			ViewMatrixComponent& viewMatC = entity.GetComponent<ViewMatrixComponent>();
			out << YAML::Key << "Locale" << YAML::Value << viewMatC.Locale;
			out << YAML::Key << "Globle" << YAML::Value << viewMatC.Globle;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;
			MeshComponent& meshC = entity.GetComponent<MeshComponent>();
			if (meshC.ModelR)
			{
				Utils::SerializerAssetFormate(out, "Model", meshC.ModelR->Handle);
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<StaticMeshComponent>())
		{
			out << YAML::Key << "StaticMeshComponent";
			out << YAML::BeginMap;
			StaticMeshComponent& meshS_C = entity.GetComponent<StaticMeshComponent>();
			if (meshS_C.ModelR)
			{
				Utils::SerializerAssetFormate(out, "Model-S", meshS_C.ModelR->Handle);
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<DynamicMeshComponent>())
		{
			out << YAML::Key << "DynamicMeshComponent";
			out << YAML::BeginMap;
			DynamicMeshComponent& meshD_C = entity.GetComponent<DynamicMeshComponent>();
#if RY_MODEL_NODE
			if (meshD_C.MeshD.size() > 0 && meshD_C.MeshD[0].MeshR)
			{
				RealtionShipComponent& rSc = entity.GetComponent<RealtionShipComponent>();
				Entity parent = scene->GetEntitiyByUUID(rSc.ParentID);
				MeshComponent& meshDP_C = parent.GetComponent<MeshComponent>();
				uint32_t index = 0;
				for (const auto& nodeP : meshDP_C.ModelR->GetNodes())
				{
					if (nodeP.Meshes[0] == meshD_C.MeshD[0].MeshR)
						break;
					index++;
				}
				out << YAML::Key << "Model-D";
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << meshDP_C.ModelR->Handle;
				out << YAML::Key << "Path" << YAML::Value << editorAssetManger->GetMetadata(meshDP_C.ModelR->Handle).FilePath.string();
				out << YAML::Key << "Index" << YAML::Value << index;
				out << YAML::EndMap;
			}
#else
			if (meshD_C.MeshR)
			{
				RealtionShipComponent& rSc = entity.GetComponent<RealtionShipComponent>();
				Entity parent = scene->GetEntitiyByUUID(rSc.ParentID);
				MeshComponent& meshDP_C = parent.GetComponent<MeshComponent>();
				uint32_t index = 0;
				for (const Ref<Mesh>& meshP : meshDP_C.ModelR->GetMeshes())
				{
					if (meshP == meshD_C.MeshR)
						break;
					index++;
				}
				out << YAML::Key << "Model-D";
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << meshDP_C.ModelR->Handle;
				out << YAML::Key << "Path" << YAML::Value << editorAssetManger->GetMetadata(meshDP_C.ModelR->Handle).FilePath.string();
				out << YAML::Key << "Index" << YAML::Value << index;
				out << YAML::EndMap;
			}
#endif
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TextComponent>())
		{
			out << YAML::Key << "TextComponent";
			out << YAML::BeginMap;
			TextComponent& textC = entity.GetComponent<TextComponent>();


			out << YAML::Key << "TextString" << YAML::Value << textC.TextString.c_str();
			out << YAML::Key << "Color" << YAML::Value << textC.Color;
			out << YAML::Key << "Kerning" << YAML::Value << textC.Kerning;
			out << YAML::Key << "LineSpacing" << YAML::Value << textC.LineSpacing;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<AmbientLigthComponent>())
		{
			out << YAML::Key << "AmbientLigthComponent";
			out << YAML::BeginMap;
			AmbientLigthComponent& ambientC = entity.GetComponent<AmbientLigthComponent>();


			out << YAML::Key << "Color" << YAML::Value << ambientC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << ambientC.Intensitie;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<DrirektionleLigthComponent>())
		{
			out << YAML::Key << "DrirektionleLigthComponent";
			out << YAML::BeginMap;
			DrirektionleLigthComponent& drirektionleC = entity.GetComponent<DrirektionleLigthComponent>();


			out << YAML::Key << "Color" << YAML::Value << drirektionleC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << drirektionleC.Intensitie;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<PointLigthComponent>())
		{
			out << YAML::Key << "PointLigthComponent";
			out << YAML::BeginMap;
			PointLigthComponent& pointC = entity.GetComponent<PointLigthComponent>();

			out << YAML::Key << "Color" << YAML::Value << pointC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << pointC.Intensitie;
			out << YAML::Key << "Distence" << YAML::Value << pointC.Distence;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpotLigthComponent>())
		{
			out << YAML::Key << "SpotLigthComponent";
			out << YAML::BeginMap;
			SpotLigthComponent& spotC = entity.GetComponent<SpotLigthComponent>();

			out << YAML::Key << "Color" << YAML::Value << spotC.Color;
			out << YAML::Key << "Intensitie" << YAML::Value << spotC.Intensitie;
			out << YAML::Key << "Distence" << YAML::Value << spotC.Distence;
			out << YAML::Key << "Inner" << YAML::Value << spotC.Inner;
			out << YAML::Key << "Outer" << YAML::Value << spotC.Outer;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}


	void SceneSerializer::Serialize(const std::string& filepath)
	{
		RY_CORE_WARN("Begin Serialize a Scene from '{}'", filepath.c_str());
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untiteld";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registery.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;

				SerializerEntity(out, entity, m_Scene);
			});


		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		m_Scene->m_Registery.each([&](auto entityID)
		{
			Entity entity{ entityID, m_Scene.get() };
			entity.UpdateMatrix();
		});
		RY_CORE_INFO("Ende Scene Serializetation");
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{

		RY_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime not Implementet!");
	}


	bool SceneSerializer::Deserialize(const std::string& filepath, bool async)
	{
		RY_CORE_WARN("Begin Serialize a Scene from {} '{}'", async, filepath.c_str());

		m_Scene->ClearAll();
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
#if RY_EDITOR_ASSETMANGER_THREADE
		Ref<EditorAssetManegerThreade> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#else
		Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();
#endif
		Ref<Scene> scene = CreateRef<Scene>();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		RY_CORE_ASSERT("Deserialize scene '{0}'", sceneName);

		YAML::Node entities = data["Entities"];
		if (entities)
		{

			std::vector<std::thread> threadProceses;
			bool assetHandleError = false;
			for (YAML::detail::iterator_value entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				YAML::Node tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				RY_CORE_ASSERT("Deserializing entity withe ID = {0}, name = {1}", sceneName);

				Entity deserializedEntity = m_Scene->CreateEntityWitheUUID(uuid, name);


				if (YAML::Node transformComponent = entity["TransformComponent"])
				{
					// Entities always have transforms
					TransformComponent& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Transaltion = transformComponent["Transaltion"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				if (YAML::Node realtionShipComponent = entity["RealtionShipComponent"])
				{
					RealtionShipComponent& rSc = deserializedEntity.GetComponent<RealtionShipComponent>();
					rSc.FirstID = realtionShipComponent["First"].as<uint64_t>();
					rSc.PreviusID = realtionShipComponent["Previus"].as<uint64_t>();
					rSc.NextID = realtionShipComponent["Next"].as<uint64_t>();
					rSc.ParentID = realtionShipComponent["Parent"].as<uint64_t>();
				}

				if (YAML::Node cameraComponent = entity["CameraComponent"])
				{
					CameraComponent& cc = deserializedEntity.AddComponent<CameraComponent>();

					YAML::Node& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectivVerticleFOV(cameraProps["PerspectivVerticleFOV"].as<float>());
					cc.Camera.SetPerspectivNearClipe(cameraProps["PerspectivNearClipe"].as<float>());
					cc.Camera.SetPerspectivFarClipe(cameraProps["PerspectivFarClipe"].as<float>());

					cc.Camera.SetOrthograficSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthograficNearClipe(cameraProps["OrthographicNearClipe"].as<float>());
					cc.Camera.SetOrthograficFarClipe(cameraProps["OrthographicFarClipe"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRotaion = cameraComponent["FixedAspectRotaion"].as<bool>();
				}

				if (YAML::Node scriptComponent = entity["ScriptComponent"])
				{
					ScriptComponent& tc = deserializedEntity.AddComponent<ScriptComponent>();
					tc.Name = scriptComponent["ClassName"].as<std::string>();
				}

				if (YAML::Node spriteRendererComponent = entity["SpriteRendererComponent"])
				{
					SpriteRendererComponent& sc = deserializedEntity.AddComponent<SpriteRendererComponent>();
					sc.Color = spriteRendererComponent["Color"].as<glm::vec4>();
#if RY_DISABLE_WEAK_PTR
					// if (Ref<Texture> tex = sc.Texture)
						assetHandleError = DeserializeAssetFormate<Texture>(spriteRendererComponent["Texture"], &sc.Texture, async) || assetHandleError;
#else
					// if(Ref<Texture> tex = sc.Texture.lock())

					Ref<Texture> tex = nullptr;
					// DeserializeAssetFormate<Texture>(spriteRendererComponent["Texture"], &tex, false);
					
					if (YAML::Node nodeE = spriteRendererComponent["Texture"])
					{
						std::string path = nodeE["Path"].as<std::string>();
						AssetHandle handle = nodeE["Handle"].as<uint64_t>();
						AssetHandle handleP = Project::GetActive()->GetEditorAssetManger()->GetAssetHandle(path);
						RY_CORE_ASSERT(handleP == handle, "Not Simulare Asset Handle!");
						if (async)
						{
							if (handle != 0)
							{
								Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle, true);
								tex = std::static_pointer_cast<Texture>(asset);
							}
							else
							{
								Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handleP, true);
								tex = std::static_pointer_cast<Texture>(asset);
							}
							
						}
						else
						{
							if (handle != 0)
								tex = AssetManager::GetAsset<Texture>(handle);
							else
								tex = AssetManager::GetAsset<Texture>(handleP);
						}
						
					}
					sc.Texture = tex;
#endif
				}
#if 1
				
				if (YAML::Node geomtryComponent = entity["GeomtryComponent"])
				{
					GeomtryComponent& gc = deserializedEntity.AddComponent<GeomtryComponent>();

					if (async)
					{
						RY_CORE_ASSERT(false);
					}
					else
					{
						gc.Geometry = VertexArray::Create();
						if (geomtryComponent)
							gc.Geometry->SetPrimitv((VertexArray::Primitv)geomtryComponent["Primtiv"].as<int>());
					}

					for (YAML::detail::iterator_value vertexBuffer : geomtryComponent["VertexBuffers"])
					{
						YAML::Node layoute = vertexBuffer["Laoute"];
						std::vector<BufferElement> bufferElements;
						bufferElements.reserve(layoute.size());

						for (YAML::detail::iterator_value elementN : layoute)
						{
							BufferElement& bufferElement = bufferElements.emplace_back(BufferElement());
							bufferElement.Name = elementN["Name"].as<std::string>();
							bufferElement.Type = (ShaderDataType)elementN["Type"].as<int>();
							bufferElement.Normilized = elementN["Normilized"].as<bool>();
							bufferElement.Size = elementN["Size"].as<uint32_t>();
							bufferElement.Offset = elementN["Offset"].as<uint32_t>();
						}
						BufferLayout bufferLayout(bufferElements);
						std::vector<unsigned char> data;
						Utils::DeserializeDynamicDataLayout(vertexBuffer["BuferData"], bufferLayout, data);
						if (async)
						{
							RY_CORE_ASSERT(false);
						}
						else
						{
							Ref<VertexBuffer> vbuffer = VertexBuffer::Create(data.data(), data.size());
							vbuffer->SetLayout(bufferLayout);
							gc.Geometry->AddVertexBuffer(vbuffer);
						}
					}
					YAML::Node indexBufferN = geomtryComponent["IndexBuffer"];
					if (indexBufferN)
					{

						uint32_t size = indexBufferN["Size-Byte"].as<uint32_t>();

						std::vector<unsigned char> indexData;
						indexData.resize(size);
						uint32_t offset = 0;
						for (YAML::detail::iterator_value index : indexBufferN["Indicies"])
						{
							uint32_t* value = (uint32_t*)(indexData.data() + offset);
							*value = index.as<uint32_t>();
							offset += 4;
						}
						if(size!=0)
						{
							if (async)
							{
								RY_CORE_ASSERT(false);
							}
							else
							{
								Ref<IndexBuffer> ibuffer = IndexBuffer::Create((uint32_t*)indexData.data(), size, BufferDataUsage::StaticDraw);
								gc.Geometry->SetIndexBuffer(ibuffer);
							}
						}
					}
				}

#if 0
				if (YAML::Node materialComponent = entity["MaterialComponent"])
				{
					MaterialComponent& mc = deserializedEntity.AddComponent<MaterialComponent>();
					mc.Color = materialComponent["Color"].as<glm::vec3>();
					mc.AlgorithmFlags = materialComponent["AlgorithmFlags"].as<int>();
					
					assetHandleError = DeserializeAssetFormate(materialComponent["Texture"], &mc.Texture, async) || assetHandleError;
					assetHandleError = DeserializeAssetFormate(materialComponent["Shader"], &mc.Shader, async) || assetHandleError; // TODO: Make Async loding Posible for Shaders , async
					YAML::Node uniformLayoute = materialComponent["UniformLayoute"];
					mc.UniformLayoute.reserve(uniformLayoute.size());
					uint32_t index = 0;
					for (YAML::detail::iterator_value uniformN : uniformLayoute)
					{
						UniformElement& bufferElement = mc.UniformLayoute.emplace_back(UniformElement());
						bufferElement.Name = uniformN["Name"].as<std::string>();
						bufferElement.Type = (ShaderDataType)uniformN["Type"].as<int>();
						bufferElement.GloblelResurce = uniformN["GloblelResurce"].as<bool>();
						if (!bufferElement.GloblelResurce)
							DeserializeDynamicData(uniformN["LocalResurce"], bufferElement.Type, bufferElement.LocalResurce);

						bufferElement.ShResourceType = (ShaderResourceType)uniformN["ShaderResourceType"].as<int>();

						index++;
					}
				}
#endif


				if (YAML::Node modelMatrixComponentN = entity["ModelMatrixComponent"])
				{
					if(!deserializedEntity.HasComponent<ModelMatrixComponent>())
						deserializedEntity.AddComponent<ModelMatrixComponent>();
					ModelMatrixComponent& m4c = deserializedEntity.GetComponent<ModelMatrixComponent>();
					if(modelMatrixComponentN["Matrix4x4"])
					{
						m4c.Locale = modelMatrixComponentN["Matrix4x4"].as<glm::mat4>();
						m4c.Globle = modelMatrixComponentN["GlobleMatrix4x4"].as<glm::mat4>();
						RY_CORE_WARN("Old ModelMatrixComponent Name Confention");
					}
					if (modelMatrixComponentN["Locale"])
					{
						m4c.Locale = modelMatrixComponentN["Locale"].as<glm::mat4>();
						m4c.Globle = modelMatrixComponentN["Globle"].as<glm::mat4>();
					}
				}

				if (YAML::Node viewMatrixComponentN = entity["ViewMatrixComponent"])
				{
					if (!deserializedEntity.HasComponent<ViewMatrixComponent>())
						deserializedEntity.AddComponent<ViewMatrixComponent>();
					ViewMatrixComponent& viewMatC = deserializedEntity.GetComponent<ViewMatrixComponent>();
					viewMatC.Locale = viewMatrixComponentN["Locale"].as<glm::mat4>();
					viewMatC.Globle = viewMatrixComponentN["Globle"].as<glm::mat4>();
				}

				YAML::Node meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					MeshComponent& mc = deserializedEntity.AddComponent<MeshComponent>();
					if (async)
					{
						YAML::Node modelN = meshComponent["Model"];
						std::string path = modelN["Path"].as<std::string>();
						AssetHandle handle = modelN["Handle"].as<uint64_t>();
						
#if 0
						mc.ModelR = nullptr;
						threadProceses.emplace_back([](std::string pathLem, AssetHandle handleLem, Ref<Model>* modelLem) {
							


							AssetHandle handleP = Project::GetActive()->GetEditorAssetManger()->GetAssetHandle(pathLem);
							Ref<Model> model = nullptr;
							Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handleLem, true);
							model = std::static_pointer_cast<Model>(asset);

							if(*modelLem != nullptr)
								*modelLem = nullptr;

							*modelLem = model;

							}, path, handle, &mc.ModelR);
#else
						Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle, true);
						Ref<Model> model = nullptr;
						mc.ModelR = nullptr;
						model = std::static_pointer_cast<Model>(asset);
						mc.ModelR = model;
#endif
					}
					else
					{
						assetHandleError = Utils::DeserializeAssetFormate<Model>(meshComponent["Model"], &mc.ModelR, false) || assetHandleError;
					}
				}

				
				if (YAML::Node  staticMeshComponent = entity["StaticMeshComponent"])
				{
					StaticMeshComponent& smc = deserializedEntity.AddComponent<StaticMeshComponent>();
					smc.ModelR = nullptr;
					if (async)
					{
						YAML::Node meshN = meshComponent["Model"];
						std::string path = meshN["Path"].as<std::string>();
						AssetHandle handle = meshN["Handle"].as<uint64_t>();
#if 0
						smc.ModelR = nullptr;
						threadProceses.emplace_back([](std::string pathLem, AssetHandle handleLem, Ref<Model>* modelLem) {



							AssetHandle handleP = Project::GetActive()->GetEditorAssetManger()->GetAssetHandle(pathLem);
							Ref<Model> model = nullptr;
							Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handleLem, true);
							model = std::static_pointer_cast<Model>(asset);

							if (*modelLem != nullptr)
								*modelLem = nullptr;

							*modelLem = model;

							}, path, handle, &smc.ModelR);
#else
						
						Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle, true);
						Ref<Model> model = nullptr;
						smc.ModelR = nullptr;
						model = std::static_pointer_cast<Model>(asset);
						smc.ModelR = model;
#endif
					}
					else
					{
						assetHandleError = Utils::DeserializeAssetFormate<Model>(meshComponent["Model"], &smc.ModelR, false) || assetHandleError;
					}
#if 0
					assetHandleError = DeserializeAssetFormate<Model>(meshComponent["Model"], &smc.ModelR, false) || assetHandleError;
#else
					
#endif
				}

				
				if (YAML::Node dynamicMeshComponent = entity["DynamicMeshComponent"])
				{
					YAML::Node modelN = dynamicMeshComponent["Model-D"];
					DynamicMeshComponent& dmc = deserializedEntity.AddComponent<DynamicMeshComponent>();
					
					uint32_t index = modelN["Index"].as<uint32_t>();
					if(async)
					{
						std::string path = modelN["Path"].as<std::string>();
						AssetHandle handle = modelN["Handle"].as<uint64_t>();

#if RY_MODEL_NODE
						Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle, true);
						Ref<Model> model = nullptr;
						
						model = std::static_pointer_cast<Model>(asset);
						const auto& node = model->GetNodes()[index];
						dmc.MeshD.reserve(node.Meshes.size());
						for (auto& mesh : node.Meshes)
						{
							dmc.MeshD.emplace_back(mesh);
						}
						
					}
					else
					{
						Ref<Model> model = nullptr;
						assetHandleError = Utils::DeserializeAssetFormate<Model>(modelN, &model, false) || assetHandleError;// TODO: Make single Mesh Loding Posible!
						const auto& nodesRoot = model->GetNodes();
						const auto& node = nodesRoot[index];

						dmc.MeshD.reserve(node.Meshes.size());
						for (auto& mesh : node.Meshes)
						{
							dmc.MeshD.emplace_back(mesh);
						}
					}
#else

#if 0
						dmc.MeshR = nullptr;
						threadProceses.emplace_back([](uint32_t indexlem, std::string pathlem, AssetHandle handlelem, Ref<Mesh>* meshlem) {
							Ref<Model> model = nullptr;

							
							AssetHandle handleP = Project::GetActive()->GetEditorAssetManger()->GetAssetHandle(pathlem);

							Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handlelem, false);
							model = std::static_pointer_cast<Model>(asset);
							const Ref<Mesh> mesh = model->GetMesh(indexlem);
							if (*meshlem != nullptr)
								*meshlem = nullptr;
							*meshlem = mesh;
							}, index, modelN["Path"].as<std::string>(), modelN["Handle"].as<uint64_t>(), &dmc.MeshR);
#else

						Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle, true);
						Ref<Model> model = nullptr;
						dmc.MeshR = nullptr;
						model = std::static_pointer_cast<Model>(asset);
						const Ref<Mesh> mesh = model->GetMesh(index);
						dmc.MeshR = model->GetMesh(index);

#endif
					}
					else
					{
						Ref<Model> model = nullptr;
						assetHandleError = Utils::DeserializeAssetFormate<Model>(modelN, &model, false) || assetHandleError;// TODO: Make single Mesh Loding Posible!
						dmc.MeshR = model->GetMesh(index);
					}

#endif
					
					
				}

				if (YAML::Node textComponent = entity["TextComponent"])
				{
					deserializedEntity.AddComponent<TextComponent>();
					TextComponent& textC = deserializedEntity.GetComponent<TextComponent>();
					textC.FontAsset = Font::GetDefault();
					textC.TextString = textComponent["TextString"].as<std::string>();
					textC.Color = textComponent["Color"].as<glm::vec4>();
					
					textC.LineSpacing = textComponent["LineSpacing"].as<float>();
					textC.Kerning = textComponent["Kerning"].as<float>();

					RY_CORE_ASSERT(deserializedEntity.HasComponent<TextComponent>())
				}

				if (YAML::Node ambientComponent = entity["AmbientLigthComponent"])
				{
					AmbientLigthComponent& ambientC = deserializedEntity.AddComponent<AmbientLigthComponent>();
					ambientC.Color = ambientComponent["Color"].as<glm::vec3>();
					ambientC.Intensitie = ambientComponent["Intensitie"].as<float>();
				}

				if (YAML::Node drirektionleComponent = entity["DrirektionleLigthComponent"])
				{
					DrirektionleLigthComponent& drirektionleC = deserializedEntity.AddComponent<DrirektionleLigthComponent>();
					drirektionleC.Color = drirektionleComponent["Color"].as<glm::vec3>();
					drirektionleC.Intensitie = drirektionleComponent["Intensitie"].as<float>();
				}

				if (YAML::Node pointLigthComponent = entity["PointLigthComponent"])
				{
					PointLigthComponent& pointLigthC = deserializedEntity.AddComponent<PointLigthComponent>();
					pointLigthC.Color = pointLigthComponent["Color"].as<glm::vec3>();
					pointLigthC.Intensitie = pointLigthComponent["Intensitie"].as<float>();
					pointLigthC.Distence = pointLigthComponent["Distence"].as<float>();
				}

				if (YAML::Node spotLigthComponent = entity["SpotLigthComponent"])
				{
					SpotLigthComponent& spotLigthC = deserializedEntity.AddComponent<SpotLigthComponent>();
					spotLigthC.Color = spotLigthComponent["Color"].as<glm::vec3>();
					spotLigthC.Intensitie = spotLigthComponent["Intensitie"].as<float>();
					spotLigthC.Distence = spotLigthComponent["Distence"].as<float>();

					spotLigthC.Inner = spotLigthComponent["Inner"].as<float>();
					spotLigthC.Outer = spotLigthComponent["Outer"].as<float>();
				}

				if (YAML::Node frameBufferComponent = entity["FrameBufferComponent"])
				{
					FrameBufferComponent& frameC = deserializedEntity.AddComponent<FrameBufferComponent>();
					if (YAML::Node frameBuffer = frameBufferComponent["FrameBuffer"])
					{
						FramebufferSpecification frame;
						frame.Height = frameBuffer["Height"].as<uint32_t>();
						frame.Width = frameBuffer["Width"].as<uint32_t>();
						uint32_t size = frameBuffer["Attachments"].size();
						frame.Attachments.Attachments.reserve(size);
						for (YAML::Node& attchment : frameBuffer["Attachments"] )
						{
							YAML::Node warp = attchment["TextureWrapping"];

							frame.Attachments.Attachments.emplace_back(FramebufferTextureSpecification(
								(TexFrom)attchment["TextureFormat"].as<int>(),
								attchment["Samples"].as<uint32_t>(),
								{
									(TexWarp)warp["S"].as<int>(),
									(TexWarp)warp["T"].as<int>(),
									(TexWarp)warp["R"].as<int>()
								},
								(TexFilter)attchment["TextureFiltering"].as<int>()
							));
						}
						frameC.FrameBuffer = Framebuffer::Create(frame);
					}
					frameC.ClearColor = frameBufferComponent["ClearColor"].as<glm::vec3>();
					frameC.FramebufferSize = (FrameBufferImageSize)frameBufferComponent["FramebufferSize"].as<int>();
				}
#endif
			}

			for (std::thread& t : threadProceses)
			{
				t.join();
			}
			// m_Scene = scene;
			if (assetHandleError)
				Serialize(filepath);

		}
		RY_CORE_INFO("Ende Scene Deserializetion");
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime not Implementet!");
		return false;
	}

}





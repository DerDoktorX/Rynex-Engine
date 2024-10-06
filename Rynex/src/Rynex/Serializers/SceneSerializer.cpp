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
			out <<  v[i].x << v[i].y << v[i].z << v[i].w ;
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
			
			out << v[i].x << v[i].y << v[i].z ;
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
				rhs[i].x = node[0 + i*3].as<float>();
				rhs[i].y = node[0 + i*3].as<float>();
				rhs[i].z = node[0 + i*3].as<float>();
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
				rhs[i].x = node[0 + i*4].as<float>();
				rhs[i].y = node[1 + i*4].as<float>();
				rhs[i].z = node[2 + i*4].as<float>();
				rhs[i].w = node[3 + i*4].as<float>();
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
				rhs[i].x = node[0 + i*3].as<int>();
				rhs[i].y = node[1 + i*3].as<int>();
				rhs[i].z = node[1 + i*3].as<int>();
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
				rhs[i].x = node[0 + i*4].as<int>();
				rhs[i].y = node[0 + i*4].as<int>();
				rhs[i].z = node[0 + i*4].as<int>();
				rhs[i].w = node[0 + i*4].as<int>();
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
				rhs[i].x = node[0 + i*3].as<uint32_t>();
				rhs[i].y = node[1 + i*3].as<uint32_t>();
				rhs[i].z = node[2 + i*3].as<uint32_t>();
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
				rhs[i].x = node[0 + i*4].as<uint32_t>();
				rhs[i].y = node[1 + i*4].as<uint32_t>();
				rhs[i].z = node[2 + i*4].as<uint32_t>();
				rhs[i].w = node[3 + i*4].as<uint32_t>();
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
		Matrix4x4Component Matrix4x4Component;
		StaticMeshComponent StaticMeshComponent;
		DynamicMeshComponent DynamicMeshComponent;
	};

	struct SceneSerialData
	{
		uint32_t EntitiySize;
		std::vector<EntitySerialData> Enitity;
	};

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
				glm::mat<3,3, float>* value = (glm::mat<3, 3, float>*)data.data();
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
				glm::vec<2, float>* value = (glm::vec<2,float>*)data.data();
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
			for (auto& ellement : layout)
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
						glm::vec<2, float>* value = (glm::vec<2, float>*)(data.data()+ offset);
						out  << YAML::Value << *value;
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
		data.resize(size* node.size());
		uint32_t offset = 0;
		uint32_t index = 0;
		const std::vector<BufferElement>& elements = layout.GetElements();
		uint32_t elementsSize = elements.size();
		for (auto& element : node)
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
				glm::vec<3, float>* dataV = (glm::vec<3, float>*)(data.data()+ offset);
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
	void DeserializeAssetFormate(YAML::Node& nodeE, Ref<T>* entityC)
	{
		if (nodeE)
		{
			std::string path = nodeE["Path"].as<std::string>();
			AssetHandle handle = nodeE["Handle"].as<uint64_t>();
			AssetHandle handleP = Project::GetActive()->GetEditorAssetManger()->GetAssetHandle(path);
			RY_CORE_ASSERT(handleP == handle, "Not Simulare Asset Handle!");
			*entityC = AssetManager::GetAsset<T>(handleP);
		}
	}


	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	
	

	static void SerializerEntity(YAML::Emitter& out, Entity entity, const Ref<Scene>& scene)
	{
		Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();

		RY_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Error: Entity has not IDComponent");
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
	
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
	
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Transaltion" << YAML::Value << tc.Transaltion;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cc = entity.GetComponent<CameraComponent>();
			auto& camera = cc.Camera;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType"		<< YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectivVerticleFOV" << YAML::Value << camera.GetPerspectivVerticleFOV();
			out << YAML::Key << "PerspectivNearClipe"	<< YAML::Value << camera.GetPerspectivNearClipe();
			out << YAML::Key << "PerspectivFarClipe"	<< YAML::Value << camera.GetPerspectivFarClipe();
			out << YAML::Key << "OrthographicSize"		<< YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNearClipe" << YAML::Value << camera.GetOrthographicNearClipe();
			out << YAML::Key << "OrthographicFarClipe"	<< YAML::Value << camera.GetOrthographicFarClipe();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRotaion" << YAML::Value << cc.FixedAspectRotaion;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			auto& sc = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << sc.Name;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& sc = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << sc.Color;
			if (sc.Texture)
			{
				SerializerAssetFormate(out, "Texture", sc.Texture->Handle);
			}

			out << YAML::EndMap;
		}

		if (entity.HasComponent<GeomtryComponent>())
		{
			auto& gc = entity.GetComponent<GeomtryComponent>();
			out << YAML::Key << "GeomtryComponent";
			out << YAML::BeginMap;
			if (gc.Geometry)
			{
				out << YAML::Key << "Primtiv" << YAML::Value << (int)gc.Geometry->GetPrimitv();
				out << YAML::Key << "VertexBuffers" << YAML::Value;
				out << YAML::BeginSeq;
				if (gc.Geometry->GetVertexBuffers().size())
				{

					for (auto& buffer : gc.Geometry->GetVertexBuffers())
					{
						auto& layout = buffer->GetLayout();
						out << YAML::BeginMap;
						out << YAML::Key << "Laoute" << YAML::Value;
						out << YAML::BeginSeq;
						for (auto& element : layout)
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
						SerializerDynamicDataLayout(out, "BuferData", layout, buferData);
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
						std::vector<unsigned char> dataIndex = indexBuffer->GetBufferData();

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

		if (entity.HasComponent<MaterialComponent>())
		{
			out << YAML::Key << "MaterialComponent" << YAML::Value;
			out << YAML::BeginMap;
			auto& mc = entity.GetComponent<MaterialComponent>();
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
				for (auto& ellement : mc.UniformLayoute)
				{
					out << YAML::BeginMap;
					out << YAML::Key << "Name" << YAML::Value << ellement.Name.c_str();
					out << YAML::Key << "Type" << YAML::Value << (int)ellement.Type;
					out << YAML::Key << "ShaderResourceType" << YAML::Value << (int)ellement.ShaderResourceType;
					out << YAML::Key << "GloblelResurce" << YAML::Value << ellement.GloblelResurce;
					if (!ellement.GloblelResurce)
						SerializerDynamicData(out, "LocalResurce", ellement.Type, ellement.LocalResurce);
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}

			out << YAML::EndMap;
			
		}

		if (entity.HasComponent<RealtionShipComponent>())
		{
			auto& rSc = entity.GetComponent<RealtionShipComponent>();
			
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

		if (entity.HasComponent<Matrix4x4Component>())
		{
			out << YAML::Key << "Matrix4x4Component";
			out << YAML::BeginMap;
			auto& m4c = entity.GetComponent<Matrix4x4Component>();
			out << YAML::Key << "Matrix4x4" << YAML::Value << m4c.Matrix4x4;
			out << YAML::Key << "GlobleMatrix4x4" << YAML::Value << m4c.GlobleMatrix4x4;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>())
		{
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;
			auto& meshC = entity.GetComponent<MeshComponent>();
			if(meshC.ModelR)
			{
				SerializerAssetFormate(out, "Model", meshC.ModelR->Handle);
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<StaticMeshComponent>())
		{
			out << YAML::Key << "StaticMeshComponent";
			out << YAML::BeginMap;
			auto& meshS_C = entity.GetComponent<StaticMeshComponent>();
			if (meshS_C.ModelR)
			{
				SerializerAssetFormate(out, "Model-S", meshS_C.ModelR->Handle);
			}
			out << YAML::EndMap;
		}

		if (entity.HasComponent<DynamicMeshComponent>())
		{
			out << YAML::Key << "DynamicMeshComponent";
			out << YAML::BeginMap;
			auto& meshD_C = entity.GetComponent<DynamicMeshComponent>();
			if (meshD_C.MeshR)
			{
				auto& rSc = entity.GetComponent<RealtionShipComponent>();
				Entity parent = scene->GetEntitiyByUUID(rSc.ParentID);
				auto& meshDP_C = parent.GetComponent<MeshComponent>();
				uint32_t index = 0;
				for (auto& meshP : meshDP_C.ModelR->GetMeshes())
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
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}


	void SceneSerializer::Serialize(const std::string& filepath)
	{
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
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{

		RY_CORE_ASSERT(false, "SceneSerializer::SerializeRuntime not Implementet!");
	}


	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		m_Scene->ClearAll();
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		Ref<EditorAssetManager> editorAssetManger = Project::GetActive()->GetEditorAssetManger();

		YAML::Node data = YAML::Load(strStream.str());
		if(!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		RY_CORE_ASSERT("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if(entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				RY_CORE_ASSERT("Deserializing entity withe ID = {0}, name = {1}", sceneName);

				Entity deserializedEntity = m_Scene->CreateEntityWitheUUID(uuid, name);


				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Transaltion = transformComponent["Transaltion"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto realtionShipComponent = entity["RealtionShipComponent"];
				if (realtionShipComponent)
				{
					auto& rSc = deserializedEntity.GetComponent<RealtionShipComponent>();
					rSc.FirstID = realtionShipComponent["First"].as<uint64_t>();
					rSc.PreviusID = realtionShipComponent["Previus"].as<uint64_t>();
					rSc.NextID = realtionShipComponent["Next"].as<uint64_t>();
					rSc.ParentID = realtionShipComponent["Parent"].as<uint64_t>();				
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
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

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& tc = deserializedEntity.AddComponent<ScriptComponent>();
					tc.Name = scriptComponent["ClassName"].as<std::string>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& sc = deserializedEntity.AddComponent<SpriteRendererComponent>();
					sc.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					DeserializeAssetFormate<Texture>(spriteRendererComponent["Texture"], &sc.Texture);
				}
#if 1
				auto geomtryComponent = entity["GeomtryComponent"];
				if (geomtryComponent)
				{
					auto& gc = deserializedEntity.AddComponent<GeomtryComponent>();
					gc.Geometry = VertexArray::Create();
					if(geomtryComponent)
						gc.Geometry->SetPrimitv((VertexArray::Primitv)geomtryComponent["Primtiv"].as<int>());
					
					for (auto vertexBuffer : geomtryComponent["VertexBuffers"])
					{
						auto layoute = vertexBuffer["Laoute"];
						std::vector<BufferElement> bufferElements;
						bufferElements.reserve(layoute.size());

						for (auto elementN : layoute)
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
						DeserializeDynamicDataLayout(vertexBuffer["BuferData"], bufferLayout, data);
						Ref<VertexBuffer> vbuffer = VertexBuffer::Create(data.data(), data.size());
						vbuffer->SetLayout(bufferLayout);
						gc.Geometry->AddVertexBuffer(vbuffer);
					}
					auto indexBufferN = geomtryComponent["IndexBuffer"];
					if (indexBufferN)
					{
						
						uint32_t size = indexBufferN["Size-Byte"].as<uint32_t>();

						std::vector<unsigned char> indexData;
						indexData.resize(size);
						uint32_t offset = 0;
						for (auto index : indexBufferN["Indicies"])
						{
							uint32_t* value = (uint32_t*)(indexData.data() + offset);
							*value = index.as<uint32_t>();
							offset+=4;
						}
						Ref<IndexBuffer> ibuffer = IndexBuffer::Create((uint32_t*)indexData.data(), size);
						gc.Geometry->SetIndexBuffer(ibuffer);
					}
				}

				auto materialComponent = entity["MaterialComponent"];
				if (materialComponent)
				{
					auto& mc = deserializedEntity.AddComponent<MaterialComponent>();
					mc.Color = materialComponent["Color"].as<glm::vec3>();
					mc.AlgorithmFlags = materialComponent["AlgorithmFlags"].as<int>();
					DeserializeAssetFormate(materialComponent["Texture"], &mc.Texture);
					DeserializeAssetFormate(materialComponent["Shader"], &mc.Shader);
					auto uniformLayoute = materialComponent["UniformLayoute"];
					mc.UniformLayoute.reserve(uniformLayoute.size());
					uint32_t index = 0;
					for (auto uniformN : uniformLayoute)
					{
						UniformElement& bufferElement = mc.UniformLayoute.emplace_back(UniformElement());
						bufferElement.Name = uniformN["Name"].as<std::string>();
						bufferElement.Type = (ShaderDataType)uniformN["Type"].as<int>();
						bufferElement.GloblelResurce = uniformN["GloblelResurce"].as<bool>();
						if (!bufferElement.GloblelResurce)
							DeserializeDynamicData(uniformN["LocalResurce"], bufferElement.Type, bufferElement.LocalResurce);
						bufferElement.ShaderResourceType = (ShaderResourceType)uniformN["ShaderResourceType"].as<int>();
						
						index++;
					}
				}

				

				auto matrix4x4Component = entity["Matrix4x4Component"];
				if (matrix4x4Component)
				{
					auto& m4c = deserializedEntity.GetComponent<Matrix4x4Component>();
					m4c.Matrix4x4 = matrix4x4Component["Matrix4x4"].as<glm::mat4>();
					m4c.GlobleMatrix4x4 = matrix4x4Component["GlobleMatrix4x4"].as<glm::mat4>();
				}

				auto meshComponent = entity["MeshComponent"];
				if (meshComponent)
				{
					auto& mc = deserializedEntity.AddComponent<MeshComponent>();
					DeserializeAssetFormate<Model>(meshComponent["Model"], &mc.ModelR);
				}

				auto staticMeshComponent = entity["StaticMeshComponent"];
				if (staticMeshComponent)
				{
					auto& smc = deserializedEntity.AddComponent<StaticMeshComponent>();
					DeserializeAssetFormate<Model>(meshComponent["Model"], &smc.ModelR);
				}

				auto dynamicMeshComponent = entity["DynamicMeshComponent"];
				if (dynamicMeshComponent)
				{
					auto modelN = dynamicMeshComponent["Model-D"];
					auto& dmc = deserializedEntity.AddComponent<DynamicMeshComponent>();
					Ref<Model> model = nullptr;
					DeserializeAssetFormate<Model>(modelN, &model);
					uint32_t index = modelN["Index"].as<uint32_t>();
					dmc.MeshR = model->GetMesh(index);
				}
#endif
			}

		}

		
		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		RY_CORE_ASSERT(false, "SceneSerializer::DeserializeRuntime not Implementet!");
		return false;
	}

}





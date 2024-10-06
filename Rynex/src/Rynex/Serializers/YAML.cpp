#include "rypch.h"
#include "YAML.h"


#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

namespace YAML {
#if 0
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
#endif

#if 1
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
				node.SetStyle(EmitterStyle::Flow);
			}
			return node;
		}

		static bool decode(const Node& node, glm::mat3& rhs)
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
				node.SetStyle(EmitterStyle::Flow);
			}
			return node;
		}

		static bool decode(const Node& node, glm::mat4& rhs)
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

	
}

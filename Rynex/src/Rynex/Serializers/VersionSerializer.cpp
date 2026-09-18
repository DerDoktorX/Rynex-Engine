
#include "rypch.h"
#include "VersionSerializer.h"


#include <yaml-cpp/yaml.h>

namespace Rynex {

    namespace Utils {
        template<typename T>
        inline static bool SerializeValue(YAML::Emitter& out, const VersionSerializer::Tag& tag, const T& value)
        {
            if (tag.empty())
                return false;

            out << YAML::Key << tag.c_str() << YAML::Value << value;
            return true;
        }

        template<typename T>
        inline static bool DeserializeValue(YAML::Node& parent, const VersionSerializer::Tag& tag, T& value)
        {
            YAML::Node node = parent[tag.c_str()];
            if (!node)
                return false;

            value = node.as<T>();
            return true;
        }
    }

    VersionSerializer::VersionSerializer(const uint64_t version, const std::map<Version, Tag>& tagMap)
        : m_TagMap(tagMap)
        , m_Version(version)
    {
        RY_CORE_ASSERT(!m_TagMap.empty(), "No versions in Tag map!");
    }

    void VersionSerializer::Serialize(YAML::Emitter& out, const uint64_t& value)
    {
        Tag tag = m_TagMap[m_Version];
        bool result = Utils::SerializeValue(out ,tag, value);
        RY_CORE_ASSERT(result, "Could not serialize version current!");
    }

    void VersionSerializer::Deserialize(YAML::Node parent, uint64_t& value)
    {
        bool result = false;
        for (auto[version, tag] : m_TagMap)
        {
            result = Utils::DeserializeValue<uint64_t>(parent, tag, value);
            RY_CORE_WARN_IF(!result, "Tag: {} was not valid from version: {}", tag, version);
            if (result)
                break;
        }
        RY_CORE_ASSERT(result, "Could not Deserialize version current!");
    }
}

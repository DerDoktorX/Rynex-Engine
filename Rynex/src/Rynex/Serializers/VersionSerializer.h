#pragma once


namespace YAML {
    class Emitter;
    class Node;
}

namespace Rynex {
    class VersionSerializer
    {
    public:
        using Version = uint64_t;
        using Tag = std::string;
        using Pair = std::pair<Version, Tag>;


        // VersionSerializer(uint64_t version, std::initializer_list<Pair> list);
        VersionSerializer(uint64_t version, const std::map<Version, Tag>& tagMap);
        ~VersionSerializer() = default;


        void Serialize(YAML::Emitter& out, const uint64_t& value);
        // void Serialize(YAML::Emitter& out, const uint32_t& value);
        // void Serialize(YAML::Emitter& out, const int& value);
        // void Serialize(YAML::Emitter& out, const float& value);
        // void Serialize(YAML::Emitter& out, const double& value);
        // void Serialize(YAML::Emitter& out, const std::string& value);
        // void Serialize(YAML::Emitter& out, std::string_view value);
        // void Serialize(YAML::Emitter& out, const std::filesystem::path& value);


        void Deserialize(YAML::Node node, uint64_t& value);
        // void Deserialize(YAML::Node node, uint32_t& value);
        // void Deserialize(YAML::Node node, int& value);
        // void Deserialize(YAML::Node node, float& value);
        // void Deserialize(YAML::Node node, double& value);
        // void Deserialize(YAML::Node node, std::string& value);
        // void Deserialize(YAML::Node node, std::string_view& value);
        // void Deserialize(YAML::Node node, std::filesystem::path& value);
    private:
        std::map<Version, Tag> m_TagMap;
        uint64_t m_Version;
    };

}

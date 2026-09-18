#pragma once

namespace Rynex::UI {
    bool DrawIVec4ControllerR(const std::string& label, glm::ivec4& values, const glm::ivec4 resetValue, const float collumWith = 80.0f);
    bool DrawVec4Controller(const std::string& label, glm::vec4& values, float resetValue = 0.0f, float collumWith = 80.0f);
    bool DrawVec4ControllerR(const std::string& label, glm::vec4& values, glm::vec4 resetValue, float collumWith = 80.0f);
    bool DrawVec3ControllerR(const std::string& label, glm::vec3& values, glm::vec3 resetValue, float collumWith = 80.0f);
    bool DrawVec3Controller(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float collumWith = 80.0f);
    bool DrawVec2Controller(const std::string & label, glm::vec2& values, float resetValue = 0.0f, float collumWith = 80.0f);
    bool DrawFloatController(const std::string& label, float* values, float resetValue = 0.0f, float min = 0.0f, float max = 0.0f, float collumWith = 80.0f);

}




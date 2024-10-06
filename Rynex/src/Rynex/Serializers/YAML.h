#pragma once
#include "Rynex/Project/Project.h"
#include "Rynex/Renderer/API/Buffer.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace YAML {
	class Emitter;
#if 0
	Emitter& operator<<(Emitter& out, const std::string_view& v);


	Emitter& operator<<(Emitter& out, const glm::vec2& v);
	Emitter& operator<<(Emitter& out, const glm::vec3& v);
	Emitter& operator<<(Emitter& out, const glm::vec4& v);


	Emitter& operator<<(Emitter& out, const glm::mat3& v);
	Emitter& operator<<(Emitter& out, const glm::mat4& v);

	Emitter& operator<<(Emitter& out, const glm::vec<2, int>& v);
	Emitter& operator<<(Emitter& out, const glm::vec<3, int>& v);
	Emitter& operator<<(Emitter& out, const glm::vec<4, int>& v);

	Emitter& operator<<(Emitter& out, const glm::mat<3, 3, int>& v);
	Emitter& operator<<(Emitter& out, const glm::mat<4, 4, int>& v);

	Emitter& operator<<(Emitter& out, const glm::vec<2, uint32_t>& v);
	Emitter& operator<<(Emitter& out, const glm::vec<3, uint32_t>& v);
	Emitter& operator<<(Emitter& out, const glm::vec<4, uint32_t>& v);

	Emitter& operator<<(Emitter& out, const glm::mat<3, 3, uint32_t>& v);
	Emitter& operator<<(Emitter& out, const glm::mat<4, 4, uint32_t>& v);


#endif
}

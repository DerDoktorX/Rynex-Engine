#pragma once

#include <glm/glm.hpp>
#include <random>

namespace Rynex {

	class RYNEX_API Random
	{
	public:
		static void Init();

		static float Float();
		static float Float(float maxValue);
		static float Float(float center, float aabbRanage);

		static glm::vec3 Float3(const glm::vec3& center, float aabbRanage);
		static glm::vec3 Float3(const glm::vec3& center, const glm::vec3& aabbRanage);

		static glm::vec4 Float4(const glm::vec4& center, float aabbRanage);
		static glm::vec4 Float4(const glm::vec4& center, const glm::vec4& aabbRanage);
	private:
		static std::mt19937 s_RandomeEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};

}



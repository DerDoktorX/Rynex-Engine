#include "rypch.h"
#include "Random.h"

namespace Rynex {

	void Random::Init()
	{
		s_RandomeEngine.seed(std::random_device()());
	}

	float Random::Float()
	{
		return (float)s_Distribution(s_RandomeEngine) / (float)std::numeric_limits<uint32_t>::max();
	}

	float Random::Float(float maxValue)
	{
		return (maxValue / (float)s_Distribution(s_RandomeEngine) / (float)std::numeric_limits<uint32_t>::max());
	}

	float Random::Float(float center, float aabbRanage)
	{
		return (Float(1.0f) * 2 - 1) * aabbRanage + center;
	}


	glm::vec3 Random::Float3(const glm::vec3& center, float aabbRanage)
	{
		return {
			(Float(1.0f) * 2 - 1) * aabbRanage + center.x,
			(Float(1.0f) * 2 - 1) * aabbRanage + center.y,
			(Float(1.0f) * 2 - 1) * aabbRanage + center.z
		};
	}

	glm::vec3 Random::Float3(const glm::vec3& center, const glm::vec3& aabbRanage)
	{
		return {
			(Float(1.0f) * 2 - 1) * aabbRanage.x + center.x,
			(Float(1.0f) * 2 - 1) * aabbRanage.y + center.y,
			(Float(1.0f) * 2 - 1) * aabbRanage.z + center.z
		};
	}


	glm::vec4 Random::Float4(const glm::vec4& center, float aabbRanage)
	{
		return {
			(Float(1.0f) * 2 - 1) * aabbRanage + center.x,
			(Float(1.0f) * 2 - 1) * aabbRanage + center.y,
			(Float(1.0f) * 2 - 1) * aabbRanage + center.z,
			(Float(1.0f) * 2 - 1) * aabbRanage + center.w
		};
	}

	glm::vec4 Random::Float4(const glm::vec4& center, const glm::vec4& aabbRanage)
	{
		return {
			(Float(1.0f) * 2 - 1) * aabbRanage.x + center.x,
			(Float(1.0f) * 2 - 1) * aabbRanage.y + center.y,
			(Float(1.0f) * 2 - 1) * aabbRanage.z + center.z,
			(Float(1.0f) * 2 - 1) * aabbRanage.w + center.w
		};
	}	
	
	static std::mt19937 s_RandomeEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
}
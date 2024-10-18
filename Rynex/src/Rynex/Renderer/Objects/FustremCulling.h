#pragma once

#include "glm/glm.hpp"

namespace Rynex {

	struct AABB{
		glm::vec3 Min;
		glm::vec3 Max;

		AABB(const std::vector<glm::vec3>& vertices)
			: Min(glm::vec3(std::numeric_limits<float>::max()))
			, Max(glm::vec3(std::numeric_limits<float>::min()))
		{
			for (const auto& vertex : vertices) {
				Min = glm::min(Min, vertex);
				Max = glm::max(Max, vertex);
			}
		}

		AABB(const AABB&) = default;
		AABB(AABB&&) = default;
	};

	struct Sphere {
		glm::vec3 Center;
		float Radius;

		Sphere(const std::vector<glm::vec3>& vertices)
			: Center(0.0f)
			, Radius(0.0f)
		{
			for (const auto& vertex : vertices) {
				Center += vertex;
			}
			Center /= static_cast<float>(vertices.size());
			for (const auto& vertex : vertices) {
				float distance = glm::distance(Center, vertex);
				Radius = glm::max(Radius, distance);
			}
		}

		Sphere(const Sphere&) = default;
		Sphere(Sphere&&) = default;
	};

	struct Plane {
		glm::vec3 Normal;
		float Distance;

		
		Plane(glm::vec3 normal, float distance)
			: Normal(normal)
			, Distance(distance)
		{ }
		Plane() = default;
		Plane(const Plane&) = default;
		Plane(Plane&&) = default;
	};



	class FustremCulling
	{
	public:
		FustremCulling();
		~FustremCulling();

		void UpdateFrustum(const glm::mat4& viewProjection);
		bool SphereInFrustum(const Sphere& sphere) const;
		bool AABBInFrustum(const AABB& aabb) const;
	private:
		std::array<Plane, 6> planes;
	};
}


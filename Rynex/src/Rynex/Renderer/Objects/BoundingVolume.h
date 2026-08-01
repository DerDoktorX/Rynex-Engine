#pragma once
#include "Rynex/Renderer/API/Buffer.h"
#include <glm/glm.hpp>

namespace Rynex {

	struct AABB {
		glm::vec3 Min;
		glm::vec3 Max;

		AABB()
			: Min(glm::vec3(std::numeric_limits<float>::max()))
			, Max(glm::vec3(std::numeric_limits<float>::min()))
		{
		}

		AABB(const AABB&) = default;
		

		AABB(const glm::vec3& max, const glm::vec3& min)
			: Max(max)
			, Min(min)
		{ }

		AABB(const std::vector<glm::vec3>& vertices)
			: Min(glm::vec3(std::numeric_limits<float>::max()))
			, Max(glm::vec3(std::numeric_limits<float>::min()))
		{
			for (const auto& vertex : vertices) {
				Min = glm::min(Min, vertex);
				Max = glm::max(Max, vertex);
			}
		}

		

		void operator=(const AABB& rigth)
		{
			Max = rigth.Max;
			Min = rigth.Min;
		}
	};

	
	struct Sphere {
		glm::vec3 Center;
		float Radius;

		Sphere() = default;
		Sphere(const Sphere&) = default;

		Sphere(const glm::vec3& center, float radius)
			: Center(center)
			, Radius(radius)
		{ }

		Sphere(const glm::vec3& max, const glm::vec3& min)
			: Center(glm::vec3{ 0.0f, 0.0f, 0.0f })
			, Radius(0.0f)
		{
			glm::vec3 center(max - min);
			center *= 0.5f;
			glm::vec3 reltive = center - max;
			Center = center;
			Radius = glm::length(reltive);
		}

		Sphere(const std::vector<glm::vec3>& vertices)
			: Center(glm::vec3{ 0.0f, 0.0f, 0.0f })
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

		
		void operator=(const Sphere& rigth)
		{
			Center = rigth.Center;
			Radius = rigth.Radius;

		}
	};

	struct Plane
	{
		glm::vec3 Normale;
		float Constant;

		Plane()
			: Normale(1.0f, 0.0f, 0.0f)
			, Constant(0.0f)
		{
		}
		Plane(const Plane&) = default;

		Plane(const glm::vec3& normale, float constant)
			: Normale(normale)
			, Constant(constant)
		{
		}

		Plane(float x, float y, float z, float constant)
			: Normale(x, y, z)
			, Constant(constant)
		{
		}
		
		void Normalize()
		{
			float normleLength = glm::length(Normale);
			float inverseNormalLength = 1.0f / normleLength;

			Normale *= inverseNormalLength;
			Constant *= inverseNormalLength;
		}

		void operator=(const Plane& rigth)
		{
			Normale = rigth.Normale;
			Constant = rigth.Constant;
		}
	};

	class BoundingVolume
	{
	public:
		BoundingVolume();
		~BoundingVolume();
		BoundingVolume(const BoundingVolume& b);
		BoundingVolume(const glm::vec3& max, const glm::vec3& min);
		BoundingVolume(const glm::vec3& max, const glm::vec3& min, const glm::vec3& center, float radius);
		BoundingVolume(const AABB& aabb, const Sphere& sphere);

		BoundingVolume(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement);
		BoundingVolume(std::array<glm::vec3, 8>& points); 
		void SetBoxAABB(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement);
		void SetBoxAABB(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, BufferElement& postionElement);

		void SetBoxAABB(std::array<glm::vec3, 8>& points);

		const glm::vec3& GetMin() const { return m_AABB.Min; }
		const glm::vec3& GetMax() const { return m_AABB.Max; }

		glm::vec3 GetMin(const glm::mat4& model) const;
		glm::vec3 GetMax(const glm::mat4& model) const;


		const AABB& GetAABB() const { return m_AABB; }
		AABB GetAABB(const glm::mat4& model) const;
		const Sphere& GetSphere() const { return m_Sphere; }
		Sphere GetSphere(const glm::mat4& model) const;
		const glm::vec3& GetCenter() const { return m_Sphere.Center; }
		glm::vec3 GetCenter(const glm::mat4& model) const;

		float GetRadius() const { return m_Sphere.Radius; }
		bool GetRadius(const glm::mat4& model, const glm::mat4& inverseViewProjtion) const;

		void operator=(const BoundingVolume& rigth);
	
	private:
#if 0
		template<glm::length_t L, typename T>
		void CheckValues(glm::vec<L, T>* value);
#endif
		
		void CalculatMinMax(const std::vector<unsigned char>& data, uint32_t offset, BufferElement element);
		void FindeRadius(const std::vector<unsigned char>& data, uint32_t offset, BufferElement element);

		void CheckValues(glm::vec<2, float>* value);
		void CheckValues(glm::vec<3, float>* value);
		void CheckValues(glm::vec<4, float>* value);

		void CheckValues(glm::vec<2, int>* value);
		void CheckValues(glm::vec<3, int>* value);
		void CheckValues(glm::vec<4, int>* value);

		void CheckValues(glm::vec<2, uint32_t>* value);
		void CheckValues(glm::vec<3, uint32_t>* value);
		void CheckValues(glm::vec<4, uint32_t>* value);

		void CheckRadius(glm::vec<2, float>* value);
		void CheckRadius(glm::vec<3, float>* value);
		void CheckRadius(glm::vec<4, float>* value);

		void CheckRadius(glm::vec<2, int>* value);
		void CheckRadius(glm::vec<3, int>* value);
		void CheckRadius(glm::vec<4, int>* value);

		void CheckRadius(glm::vec<2, uint32_t>* value);
		void CheckRadius(glm::vec<3, uint32_t>* value);
		void CheckRadius(glm::vec<4, uint32_t>* value);

	private:

		AABB	m_AABB;
		Sphere	m_Sphere;
		

		// glm::vec3 m_Max;
		// glm::vec3 m_Min;
		// glm::vec3 m_CenterPoint;
		// float m_Radius;
	};
	

	
}


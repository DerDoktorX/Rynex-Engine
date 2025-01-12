#pragma once
#include "Rynex/Renderer/API/Buffer.h"
#include <glm/glm.hpp>

namespace Rynex {
	class BoxAABB
	{
	public:
		BoxAABB();
		~BoxAABB();
		BoxAABB(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement);
		BoxAABB(std::array<glm::vec3, 8>& points);
		void SetBoxAABB(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement);
		void SetBoxAABB(std::array<glm::vec3, 8>& points);

		const glm::vec3& GetMin() const { return m_Min; }
		const glm::vec3& GetMax() const { return m_Max; }

	private:
#if 0
		template<glm::length_t L, typename T>
		void CheckValues(glm::vec<L, T>* value);
#endif
		
		void CalculatMinMax(const std::vector<unsigned char>& data, uint32_t offset, BufferElement element);

		void CheckValues(glm::vec<2, float>* value);
		void CheckValues(glm::vec<3, float>* value);
		void CheckValues(glm::vec<4, float>* value);

		void CheckValues(glm::vec<2, int>* value);
		void CheckValues(glm::vec<3, int>* value);
		void CheckValues(glm::vec<4, int>* value);

		void CheckValues(glm::vec<2, uint32_t>* value);
		void CheckValues(glm::vec<3, uint32_t>* value);
		void CheckValues(glm::vec<4, uint32_t>* value);
	private:
		glm::vec3 m_Max;
		glm::vec3 m_Min;
		glm::vec3 m_CenterPoint;
	};
	
}


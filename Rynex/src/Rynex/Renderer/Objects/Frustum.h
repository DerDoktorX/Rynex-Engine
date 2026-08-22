#pragma once
#include <Rynex/Renderer/Objects/BoundingVolume.h>

namespace Rynex {


	class Frustum
	{
	public:
		Frustum(const glm::mat4& view, const glm::mat4& projtion);
		~Frustum();

		void SetFromProjectionMatrix(const glm::mat4& view, const glm::mat4& projtion);
	private:
		static Plane SetPlaneFromProjectionAdd(const glm::mat4& m, int colume);
		static Plane SetPlaneFromProjectionSub(const glm::mat4& m, int colume);

	private:
		Plane m_Planes[6];
	};
}



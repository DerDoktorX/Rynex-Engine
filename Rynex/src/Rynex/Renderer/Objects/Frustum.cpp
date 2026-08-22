#include "rypch.h"
#include "Frustum.h"

namespace Rynex {

	Frustum::Frustum(const glm::mat4& view, const glm::mat4& projtion)
	{
		SetFromProjectionMatrix(view, projtion);
	}
	Frustum::~Frustum()
	{
	}
	void Frustum::SetFromProjectionMatrix(const glm::mat4& view, const glm::mat4& projtion)
	{
		glm::mat4 viewProjtion = view * projtion;
		m_Planes[0] = SetPlaneFromProjectionSub(viewProjtion, 0);
		m_Planes[1] = SetPlaneFromProjectionAdd(viewProjtion, 0);

		m_Planes[2] = SetPlaneFromProjectionAdd(viewProjtion, 1);
		m_Planes[3] = SetPlaneFromProjectionSub(viewProjtion, 1);

		m_Planes[4] = SetPlaneFromProjectionSub(viewProjtion, 2);
		m_Planes[5] = SetPlaneFromProjectionAdd(viewProjtion, 2);

	}

	Plane Frustum::SetPlaneFromProjectionAdd(const glm::mat4& m, int colume)
	{
		const float& m3_03 = m[0][3];
		const float& mX_0c = m[0][colume];
		

		const float& m7_13 = m[1][3];
		const float& mX_1c = m[1][colume];

		const float& m11_23 = m[2][3];
		const float& mX_2c  = m[2][colume];

		const float& m15_33 = m[3][3];
		const float& mX_3c  = m[3][colume];


		float x = m3_03	 + mX_0c;
		float y = m7_13  + mX_1c;
		float z = m11_23 + mX_2c;
		float w = m15_33 + mX_1c;

		Plane plane(x, y, z, w);
		plane.Normalize();
		return plane;
	}

	Plane Frustum::SetPlaneFromProjectionSub(const glm::mat4& m, int colume)
	{
		const float& m3_03 = m[0][3];
		const float& mX_0c = m[0][colume];


		const float& m7_13 = m[1][3];
		const float& mX_1c = m[1][colume];

		const float& m11_23 = m[2][3];
		const float& mX_2c  = m[2][colume];

		const float& m15_33 = m[3][3];
		const float& mX_3c  = m[3][colume];


		float x = m3_03  - mX_0c;
		float y = m7_13  - mX_1c;
		float z = m11_23 - mX_2c;
		float w = m15_33 - mX_1c;

		Plane plane(x, y, z, w);
		plane.Normalize();
		return plane;
	}
}
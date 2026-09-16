#pragma once
#include "rypch.h"

namespace Rynex {
	namespace Batch {
		struct Render3DMeshObject
		{
			glm::mat4 m_ModelMatrix;
			glm::mat4 m_NormaleMatrix;
			glm::ivec4 m_IValues;
		};

		struct Render3DMeshObjectTransform
		{
			glm::mat4 m_ModelMatrix;
		};


	}
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObject, == , &&, m_ModelMatrix, m_NormaleMatrix, m_IValues);
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObject, != , ||, m_ModelMatrix, m_NormaleMatrix, m_IValues);

	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObjectTransform, == , &&, m_ModelMatrix);
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObjectTransform, != , || , m_ModelMatrix);

}
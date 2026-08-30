#pragma once
#include "rypch.h"

namespace Rynex {
	namespace Batch {
		struct Render3DMeshObject
		{
			glm::mat4 modelMatrix;
			glm::mat4 normaleMatrix;
			glm::ivec4 iValues;
		};

		struct Render3DMeshObjectTrasform
		{
			glm::mat4 modelMatrix;
		};


	}
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObject, == , &&, modelMatrix, normaleMatrix, iValues);
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObject, != , ||, modelMatrix, normaleMatrix, iValues);

	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObjectTrasform, == , &&, modelMatrix);
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::Batch::Render3DMeshObjectTrasform, != , || , modelMatrix);

}
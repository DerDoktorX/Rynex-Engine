#include <rypch.h>
#include "IndirectDrawMap.h"




namespace Rynex {




#pragma region TypeTempletFunc_IndirectDrawMap

	template<>
	RenderMeshObjectData IndirectDrawMap<RenderMeshObjectData>::SetRendererDataType(const glm::mat4& modelMatrix, const int entityID, const int materielIndex)
	{
		RenderMeshObjectData object;
		object.TransformMatrix = modelMatrix;
		object.NormleMatrix = glm::mat4(-0.1f);
		object.EnitityID = entityID;
		object.MaterielID = materielIndex;
		object.Empty0 = -10;
		object.Empty1 = -11;
		return object;
	}

	template<>
	MeshRenderObjectTransform IndirectDrawMap<MeshRenderObjectTransform>::SetRendererDataType(const glm::mat4& modelMatrix, const int entityID, const int materielIndex)
	{
		MeshRenderObjectTransform object;
		object.TransformMatrix = modelMatrix;
		object.NormleMatrix = glm::transpose( glm::inverse( modelMatrix) );
		object.MaterielIndex = materielIndex;
		object.EntityID = entityID;
		return object;
	}

	template<>
	glm::mat4 IndirectDrawMap<glm::mat4>::SetRendererDataType(const glm::mat4& modelMatrix, const int entityID, const int materielIndex)
	{
		return modelMatrix;
	}

	template<typename T>
	typename IndirectDrawMap<T>::_ObjectRenderData IndirectDrawMap<T>::SetRendererDataType(const glm::mat4& modelMatrix, const int entityID, const int materielIndex)
	{
		static_assert(false);   
		return _ObjectRenderData();
	}

#pragma endregion
	
}
#include <rypch.h>
#include "Material.h"


#include <Rynex/Renderer/Materials/DefaultMaterial.h>
#include <Rynex/Renderer/Rendering/Renderer.h>



namespace Rynex {

	

	

	



	glm::vec3 Material::GetColor() const
	{
		RY_CORE_WARN("Material default Impl!");
		return glm::vec3(-1.0f, 0.0f, -1.0f);
	}

	float Material::GetAlpha() const
	{
		RY_CORE_WARN("Material default Impl!");
		return -1.0f;
	}

	int Material::GetShadeRenderMode() const 
	{
		return RenderMode::CallFace_Back
			| RenderMode::PrimitivReset
			| RenderMode::Death_Buffer
			| RenderMode::A_Buffer;
	}

	int Material::GetDepthRenderMode() const 
	{ 
		return RenderMode::Death_Buffer
			| RenderMode::PrimitivReset
			| RenderMode::CallFace_None;
	}

	bool Material::IsRady() const
	{
		RY_CORE_WARN("Material default Impl!");
		return false;
	}

	

	template<typename T>
	T Material::GetMaterielDataFromMateriel(const Ref<Material>& material)
	{
		static_assert(false, "No Default GetMaterielDataFromMateriel");
	    return T();
	}

	template<>
	MaterielShaderData Material::GetMaterielDataFromMateriel<MaterielShaderData>(const Ref<Material>& material)
	{
		const void* ptr = material->GetMaterielDataPtr();
		const MaterielShaderData* dataPtr = static_cast<const MaterielShaderData*>(ptr);
		MaterielShaderData data = *dataPtr;
		return data;
	}

	template<typename T>
	void Material::SetupMaterielObject(T& materielDataObject, int textureAlbedoIndex, int textureSpecularIndex, int textureHeightIndex)
	{
		static_assert(false);
	}

	template<>
	void Material::SetupMaterielObject<MaterielShaderData>(MaterielShaderData& materielDataObject, int textureAlbedoIndex, int textureSpecularIndex, int textureHeightIndex)
	{
		materielDataObject.UseTexure = textureAlbedoIndex;
		RY_CORE_ASSERT(textureSpecularIndex == -1, "Not Expected!");
		RY_CORE_ASSERT(textureHeightIndex == -1, "Not Expected!");
	}
}
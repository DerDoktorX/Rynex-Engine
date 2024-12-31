#include "rypch.h"
#include "Material.h"


#include "Rynex/Renderer/Materials/ShaderMaterial.h"
#include "Rynex/Renderer/Materials/BasicMaterial.h"
#include "Rynex/Renderer/Materials/DeferredMaterial.h"


namespace Rynex {

	

	Ref<Material> Material::CreateBasic(const std::vector<std::filesystem::path>& paths)
	{
	
		return CreateRef<BasicMaterial>( paths);
	}

	Ref<Material> Material::CreateDefeard()
	{
		return Ref<DeferredMaterial>();
	}

	Ref<Material> Material::CreateShader(ShaderMaterialDefaultNames& names)
	{
		return CreateRef<ShaderMaterial>(names);
	}


	Ref<Material> Material::CreateBasicAsysnc(std::vector<std::filesystem::path>&& paths)
	{
		return CreateRef<BasicMaterial>( std::move(paths));
	}

	Ref<Material> Material::CreateDefeardAsysnc()
	{
		return Ref<DeferredMaterial>();
	}

	Ref<Material> Material::CreateShaderAsysnc()
	{
		return Ref<ShaderMaterial>();
	}



	const glm::vec3& Material::GetColor() const
	{
		return glm::vec3(1.0f, 0.0f, 1.0f);
	}

	float Material::GetAlpha() const
	{
		return 0.0f;
	}

	const Ref<Shader>& Material::GetShader() const
	{
		return nullptr;
	}

	int Material::GetFlage()
	{
		return 0;
	}

	const glm::mat4& Material::GetMatrix() const
	{
		return glm::mat4(1.0f);
	}

	
	bool Material::IsRady()
	{
		RY_CORE_WARN("Materil default Impl!");
		return false;
	}

}
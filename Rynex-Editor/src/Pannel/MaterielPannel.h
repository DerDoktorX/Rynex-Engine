#pragma once
#include <Rynex/Renderer/Materials/Material.h>

namespace Rynex {

	class MaterielPannel
	{
	public:
		MaterielPannel();
		~MaterielPannel();

		void OnImGuiRender();
		void OnDetache();

		void SetMateriel(Ref<Material>& material);

		void CreateMaterieInternel(const std::string& name);
		void CreateMaterieAsset(const std::string& name);

		void MaterielList();

		void SaveChange();
	private:
		Ref<Material> m_Material;

	};
}

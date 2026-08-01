#pragma once

namespace Rynex {
	class Material;
	class MaterialSerializer
	{
	public:
		MaterialSerializer(Ref<Material>& materiel);

		bool Serlize(const std::filesystem::path& path);
		bool Deserlize(const std::filesystem::path& path);

	private:
		Ref<Material> m_Materiel;
	};
}

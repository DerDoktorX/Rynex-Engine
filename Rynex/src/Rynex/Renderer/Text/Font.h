#pragma once
#include <Rynex/Renderer/API/Texture.h>

namespace Rynex {

	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& path);
		~Font();

		const MSDFData* GetMSDFData() const { return m_Data; }
		Ref<Texture> GetAtlasTexture() { return m_AtlasTexture; }
		static Ref<Font> GetDefault();
	private:
		MSDFData* m_Data;
		Ref<Texture> m_AtlasTexture;
	};

}



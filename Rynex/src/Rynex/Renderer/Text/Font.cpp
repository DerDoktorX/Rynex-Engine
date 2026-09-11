#include "rypch.h"
#include "Font.h"

#if RY_ENABLE_FONT
#undef INFINITE
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <msdf-atlas-gen/FontGeometry.h>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <Rynex/Renderer/Text//MSDFData.h>
#endif

namespace Rynex {


	template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
	static Ref<Texture> CreateAndCacheAtlas(const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
		const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
	{
		msdf_atlas::GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
		generator.setAttributes(attributes);
		generator.setThreadCount(8);
		generator.generate(glyphs.data(), (int)glyphs.size());
		
		msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

	
		uint32_t widthUint = static_cast<uint32_t>(bitmap.width);
		uint32_t heightUint = static_cast<uint32_t>(bitmap.height);
		uint32_t uintSize = sizeof(T);
		uint32_t bytesSize = uintSize * widthUint * heightUint * 3u;
		T* dataTypePtr = const_cast<T*>(bitmap.pixels);
		void* dataPtr = reinterpret_cast<void*>(dataTypePtr);

		TextureSpecification spec;
		spec.Width = widthUint;
		spec.Height = heightUint;
		spec.Depth = 1u;
		spec.Format = TextureFormat::RGB8;
		spec.GenerateMips = false;
		spec.FilteringMode = TextureFilteringMode::Linear;
		spec.Target = TextureTarget::Texture2D;

		Ref<Texture> texture = Texture::Create(spec, dataPtr, bytesSize);
		texture->SetData(dataPtr, bytesSize);

		return texture;
	}


	Font::Font(const std::filesystem::path& filepath)
		: m_Data(new MSDFData())
	{

		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		RY_CORE_ASSERT(ft);

		std::string fileString = filepath.string();

		// TODO(Yan): msdfgen::loadFontData loads from memory buffer which we'll need 
		msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
		if (!font)
		{
			RY_CORE_ERROR("Failed to load font: {}", fileString);
			return;
		}

		struct CharsetRange
		{
			uint32_t Begin, End;
		};

		// From imgui_draw.cpp
		static const CharsetRange charsetRanges[] =
		{
			{ 0x0020, 0x00FF }
		};

		msdf_atlas::Charset charset;
		for (CharsetRange range : charsetRanges)
		{
			for (uint32_t c = range.Begin; c <= range.End; c++)
				charset.add(c);
		}

		double fontScale = 1.0;
		m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
		int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
		RY_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());


		double emSize = 40.0;

		msdf_atlas::TightAtlasPacker atlasPacker;
		atlasPacker.setPixelRange(2.0);
		atlasPacker.setMiterLimit(1.0);
		atlasPacker.setPadding(0);
		atlasPacker.setScale(emSize);
		int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());
		RY_CORE_ASSERT(remaining == 0);

		int width, height;
		atlasPacker.getDimensions(width, height);
		emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
		// if MSDF || MTSDF

		uint64_t coloringSeed = 0;
		bool expensiveColoring = false;
		if (expensiveColoring)
		{
			msdf_atlas::Workload([&glyphs = m_Data->Glyphs, &coloringSeed](int i, int threadNo) -> bool {
				unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
				glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
				return true;
				}, m_Data->Glyphs.size()).finish(THREAD_COUNT);
		}
		else {
			unsigned long long glyphSeed = coloringSeed;
			for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
			{
				glyphSeed *= LCG_MULTIPLIER;
				glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
			}
		}


		m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeometry, width, height);


		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
	}

	Font::~Font()
	{
		delete m_Data;
	}

	void Font::ResetDefault()
	{
		m_Default.reset();
		m_Default = nullptr;
	}

	Ref<Font> Font::GetDefault()
	{		
		
		if (!m_Default)
			m_Default = CreateRef<Font>("../Rynex-Editor/Resources/fonts/Open_Sans/static/OpenSans-Bold.ttf");

		return m_Default;
	}

}
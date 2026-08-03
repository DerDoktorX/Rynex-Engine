#pragma once
#if RY_ENABLE_FONT
#include <vector>
#undef INFINITE
#include "msdf-atlas-gen.h"
#endif
namespace Rynex {
	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
	};

}
#pragma once
#include "Rynex/Renderer/API/Texture.h"



namespace Rynex {
	class Texture2DSerialiazer
	{
	public:
		static bool Serlize(const std::filesystem::path& filepath, Ref<Texture> texture);
		static Ref<Texture> Deserlize(const std::filesystem::path& filepath);
	};
}



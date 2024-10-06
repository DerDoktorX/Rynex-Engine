#pragma once
#include "Rynex/Core/Base.h"
#include "Rynex/Renderer/API/Framebuffer.h"



namespace Rynex {

	class RYNEX_API FrambufferSerializer
	{
	public:
		static bool Serilze(const std::filesystem::path& filepath, Ref<Framebuffer> frambuffer);
		static Ref<Framebuffer> Deserilze(const std::filesystem::path& filepath);
	};
}



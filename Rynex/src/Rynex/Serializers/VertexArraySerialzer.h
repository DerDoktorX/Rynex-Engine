#pragma once
#include "Rynex/Renderer/API/VertexArray.h"

namespace Rynex {
	class VertexArraySerialzer
	{
	public:
		static bool Serialzer(const std::filesystem::path& path, Ref<VertexArray> texture);
		static Ref<VertexArray> Deserlize(const std::filesystem::path& path);
	};
}



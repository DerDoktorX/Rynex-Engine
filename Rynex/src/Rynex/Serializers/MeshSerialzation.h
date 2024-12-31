#pragma once
#include <Rynex/Renderer/Objects/Mesh.h>

namespace Rynex {
	class MeshSerialzation
	{
	public:
		static bool Serilze(const std::filesystem::path& filepath, Ref<Mesh> mesh);
		static Ref<Mesh> Deserilze(const std::filesystem::path& filepath);
	};
}


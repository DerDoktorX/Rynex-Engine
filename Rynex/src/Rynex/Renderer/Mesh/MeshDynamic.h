#pragma once
#include <Rynex/Renderer/Mesh/Mesh.h>

namespace Rynex{
	class RYNEX_API MeshDynamic : public Mesh
	{
	public:
		virtual Mesh::Type GetMeshType() const override { return Mesh::Type::Dynamic; }
	private:

	};
}


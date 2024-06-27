#pragma once

#include "Rynex/Renderer/Buffer.h"
#include "Rynex/Renderer/VertexArray.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Rynex {
	class SceneGeomtry
	{

	enum class GeomtryShape
	{
		Quad2D = 0,
		Kube3D = 1, Sphere3D = 2, Plane3D = 3, Pyrmed3D = 4,
	};

	enum class GeomtryPrimitiv
	{
		Line = 0, LineSegments = 1, LineLoop = 2,
		Triangle = 3, TriangleStrips = 4,
		Points = 5,
	};

	public:
		
	public:
		SceneGeomtry::~SceneGeomtry();

		void CreateShape(const BufferLayout& layout, GeomtryShape shape, GeomtryPrimitiv primitiv = GeomtryPrimitiv::Triangle);
		void QuadShape2D(const BufferLayout& layout,float scale);

		void SetShape(GeomtryShape shape) { m_Shape = shape;  }
		void SetPrimitiv(GeomtryPrimitiv primitiv) { m_Primitiv = primitiv; CalculateIndexs();}

		const GeomtryPrimitiv GetPrimitif() const { return m_Primitiv; }
		const GeomtryShape GetShape() const { return m_Shape; }
		//const Ref<VertexArray>& GetVertexArray() const{ return m_VertexArray; }
	private:
		void CalculateIndexs();
		void CalculateVertex();

		void CalculateQuad2D();
		void CalculateKube3D();
		void CalculateSphere3D();
		void CalculatePlane3D();
		void CalculatePyrmed3D();

	private:
		//Ref<VertexArray> m_VertexArray;
		GeomtryShape m_Shape;
		GeomtryPrimitiv m_Primitiv;
		BufferLayout& m_Layout;

		void* m_VertexData = nullptr;
		uint32_t m_VertexSize = 0;

		uint32_t* m_IndexData = nullptr;
		uint32_t m_IndexSize = 0;
	};
}



#include "rypch.h"
#include "SceneGeomtry.h"


namespace Rynex {
	
	SceneGeomtry::~SceneGeomtry()
	{
		delete[] m_VertexData;
		delete[] m_IndexData;
	}

	void SceneGeomtry::CreateShape(const BufferLayout& layout, GeomtryShape shape, GeomtryPrimitiv primitiv)
	{
		m_Shape = shape;
		m_Primitiv = primitiv;

		m_VertexSize = layout.GetLength();
		CalculateVertex();
	}

	void SceneGeomtry::CalculateIndexs()
	{
		uint32_t* size = new uint32_t[m_IndexSize];
		switch (m_Shape)
		{
		case GeomtryShape::Quad2D:
			{
			break;
			}
		case GeomtryShape::Kube3D:
		{
			break;
		}
		case GeomtryShape::Sphere3D:
		{
			break;
		}
		case GeomtryShape::Plane3D:
		{
			break;
		}
		case GeomtryShape::Pyrmed3D:
			{
			break;
			}
			
		default:
			RY_CORE_ASSERT(false, "Not Definede Set GeomtryShape!");
			break;
		}
		delete[] size;
	}

	void SceneGeomtry::CalculateVertex()
	{
		switch (m_Shape)
		{
			case GeomtryShape::Quad2D: CalculateQuad2D(); break;
			case GeomtryShape::Kube3D: CalculateKube3D(); break;
			case GeomtryShape::Sphere3D: CalculateSphere3D(); break;
			case GeomtryShape::Plane3D: CalculatePlane3D(); break;
			case GeomtryShape::Pyrmed3D: CalculatePyrmed3D(); break;
			default: RY_CORE_ASSERT(false, "Not Definede Set GeomtryShape!"); break;
		}
	}

	void SceneGeomtry::CalculateQuad2D()
	{

		m_VertexData = new void*[m_VertexSize*4];
		void* vertex = new void* [m_VertexSize];
		m_IndexSize = 6;
		m_IndexData[6];

		for (int i = 0; i < 4; i++)
		{
			 
			
		}
		


		
	}

	void SceneGeomtry::CalculateKube3D()
	{
		m_VertexData = new void* [m_VertexSize];
		uint32_t* size = new uint32_t[m_IndexSize];

		delete[] size;
	}

	void SceneGeomtry::CalculateSphere3D()
	{
		m_VertexData = new void* [m_VertexSize];
		uint32_t* size = new uint32_t[m_IndexSize];

		delete[] size;
	}

	void SceneGeomtry::CalculatePlane3D()
	{
		m_VertexData = new void* [m_VertexSize];
		uint32_t* size = new uint32_t[m_IndexSize];

		delete[] size;
	}

	void SceneGeomtry::CalculatePyrmed3D()
	{
		m_VertexData = new void* [m_VertexSize];
		uint32_t* size = new uint32_t[m_IndexSize];
	}

	

}
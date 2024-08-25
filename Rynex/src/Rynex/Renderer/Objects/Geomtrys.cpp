
#include "rypch.h"
#include "Geomtrys.h"

namespace Rynex {

	struct VertexTestCube
	{
		glm::vec3 Postion;
		glm::vec2 UV;
		glm::vec3 Normale;

		VertexTestCube() = default;
		VertexTestCube(glm::vec3 postion, glm::vec2 uv, glm::vec3 normale)
			: Postion(postion)
			, UV(uv)
			, Normale(normale)
		{}
	};
	
	void Geomtrys::SetCubeVertex(const Ref<VertexArray>& vertexArray, Ref<VertexBuffer> vertexBuffer, const glm::vec3& size, const BufferLayout& layoute)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_WARN("Geomtrys::GetCubeVertex is not Final Radey!");
		
		vertexBuffer->SetLayout({ 
			{  ShaderDataType::Float3,	"a_Postion"	},
			{  ShaderDataType::Float2,	"a_UV"	},
			{  ShaderDataType::Float3,	"a_Normals"	},
		});
		vertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t count = 0;

		RY_CORE_MULTY_MEMORY_ALICATION("vertex", "Geomtrys::SetCubeVertex", sizeof(VertexTestCube) * 18);
		VertexTestCube *vertex = new VertexTestCube[18];
		// up
		vertex[0] = VertexTestCube(glm::vec3( -0.5f,  0.5f, -0.5f  ), glm::vec2( 0.0f, 0.0f ), glm::vec3( 0.0f,  1.0f,  0.0f ));
		vertex[1] = VertexTestCube(glm::vec3(  0.5f,  0.5f, -0.5f  ), glm::vec2( 0.0f, 0.0f ), glm::vec3( 0.0f,  1.0f,  0.0f ));
		vertex[2] = VertexTestCube(glm::vec3( -0.5f,  0.5f,  0.5f  ), glm::vec2( 0.0f, 0.0f ), glm::vec3( 0.0f,  1.0f,  0.0f ));
		vertex[3] = VertexTestCube(glm::vec3(  0.5f,  0.5f,  0.5f  ), glm::vec2( 0.0f, 0.0f ), glm::vec3( 0.0f,  1.0f,  0.0f ));

		// front										  
		vertex[4] = VertexTestCube(glm::vec3( -0.5f, -0.5f,  0.5f ), glm::vec2( 0.0f, 1.0f ), glm::vec3( 0.0f,  0.0f,  1.0f ));
		vertex[5] = VertexTestCube(glm::vec3(  0.5f, -0.5f,  0.5f ), glm::vec2( 0.0f, 1.0f ), glm::vec3( 0.0f,  0.0f,  1.0f ));
		// down
		vertex[6] = VertexTestCube(glm::vec3( -0.5f, -0.5f, -0.5f ), glm::vec2( 0.0f, 1.0f ), glm::vec3( 0.0f, -1.0f,  0.0f));
		vertex[7] = VertexTestCube(glm::vec3(  0.5f, -0.5f, -0.5f ), glm::vec2( 0.0f, 1.0f ), glm::vec3( 0.0f, -1.0f,  0.0f));
		// backe
		vertex[8] = VertexTestCube(glm::vec3( -0.5f,  0.5f, -0.5f  ), glm::vec2(0.0f, 1.0f), glm::vec3(   0.0f,  0.0f, -1.0f ));
		vertex[9] = VertexTestCube(glm::vec3(  0.5f,  0.5f, -0.5f  ), glm::vec2(0.0f, 1.0f), glm::vec3(   0.0f,  0.0f, -1.0f ));
		// left
		vertex[10] = VertexTestCube(glm::vec3(  0.5f,  0.5f,  0.5f  ), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vertex[11] = VertexTestCube(glm::vec3(  0.5f, -0.5f,  0.5f  ), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vertex[12] = VertexTestCube(glm::vec3(  0.5f,  0.5f, -0.5f  ), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vertex[13] = VertexTestCube(glm::vec3(  0.5f, -0.5f, -0.5f  ), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		// rigth
		vertex[14] = VertexTestCube(glm::vec3( -0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		vertex[15] = VertexTestCube(glm::vec3( -0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		vertex[16] = VertexTestCube(glm::vec3( -0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		vertex[17] = VertexTestCube(glm::vec3( -0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));


		vertexBuffer->SetData(vertex, 18 * sizeof(VertexTestCube));
		RY_CORE_MULTY_FREE_ALICATION("vertex", "Geomtrys::SetCubeVertex");
		delete[] vertex;
	}


	void Geomtrys::SetCubeIndex(const Ref<VertexArray>& vertexArray)
	{
		RY_PROFILE_FUNCTION();
		uint32_t* indcies;
		uint32_t count;
		switch (vertexArray->GetPrimitv())
		{
			case VertexArray::Primitv::Traingle:
			{
				count = 673;
				//count = 150; //36
				indcies = new uint32_t[count * sizeof(uint32_t)];
				RY_CORE_MULTY_MEMORY_ALICATION("indcies", "Geomtrys::SetCubeIndex", count * sizeof(uint32_t));
				uint32_t index[] = {
					0, 1, 2,	// 1
					2, 3, 0,	// 2

					4, 5, 6,	// 3
					6, 7, 4,	// 3

					0, 1, 4,	// 4
					5, 6, 1,	// 5

					0, 0, 0,	// 6
					0, 0, 0,	// 7
					0, 0, 0,	// 8
					0, 0, 0,	// 9
					0, 0, 0,	// 10
					0, 0, 0,	// 11
				};
				for (int i = 0; i < count; i++)
				{
					indcies[i] = index[i];
				}
				break;
			}
			case VertexArray::Primitv::TraingleStrips:
			{
				count = 21; //21
				indcies = new uint32_t[count * sizeof(uint32_t)];
				RY_CORE_MULTY_MEMORY_ALICATION("indcies", "Geomtrys::SetCubeIndex", count * sizeof(uint32_t));
				uint32_t index[] = {
					0, 1, 2, 3, 4, 5, 6, 7, 8 , 9, 9, 10, 10, 11, 12, 13, 14, 14, 15, 16, 17
				};
				

				for (int i = 0; i < count; i++)
				{
					indcies[i] = index[i];
				}
				break;
			}

			default:
				RY_CORE_ERROR("Not implemnt Index Primitv, in Geomtrys::SetCubeIndex!");
				return;

		}

		Ref<IndexBuffer> index = IndexBuffer::Create(indcies, count);
		vertexArray->SetIndexBuffer(index);
		delete[] indcies;
		RY_CORE_MULTY_FREE_ALICATION("indcies", "Geomtrys::SetCubeIndex");
	}

}
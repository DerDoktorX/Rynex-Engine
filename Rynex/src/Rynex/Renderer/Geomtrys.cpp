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
	
	void Geomtrys::GetCubeVertex(const Ref<VertexArray>& vertexArray, Ref<VertexBuffer> vertexBuffer, const glm::vec3& size, const BufferLayout& layoute)
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

		VertexTestCube *vertex = new VertexTestCube[8];
		
		vertex[0] = VertexTestCube(glm::vec3( -0.5f, -0.5f,  0.5f ),	glm::vec2(0.0f, 0.0f), glm::vec3(0.75f, 0.25f, 0.25f));
		vertex[1] = VertexTestCube(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		vertex[2] = VertexTestCube(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		vertex[3] = VertexTestCube(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.75f));

		vertex[4] = VertexTestCube(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		vertex[5] = VertexTestCube(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		vertex[6] = VertexTestCube(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f));
		vertex[7] = VertexTestCube(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f));


		vertexBuffer->SetData(vertex, 8 * sizeof(VertexTestCube));
		
		delete[] vertex;
	}


	void Geomtrys::SetCubeIndex(const Ref<VertexArray>& vertexArray)
	{
		RY_PROFILE_FUNCTION();
#if RY_MESH
		uint32_t indcies[] = {
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
		uint32_t count = 36;
#endif

#if RY_MESHSTRIPS
		uint32_t indcies[] = {
			 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 1, 7, 3, 5, 4, 4, 2, 6, 0
		};
		uint32_t count = 7;
#endif
		Ref<IndexBuffer> index = IndexBuffer::Create(indcies, count);
		vertexArray->SetIndexBuffer(index);
	}

}
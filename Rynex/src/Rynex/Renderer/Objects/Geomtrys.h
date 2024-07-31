#pragma once

#include "Rynex/Renderer/API/Buffer.h"
#include "Rynex/Renderer/API/VertexArray.h"

#include <glm/glm.hpp>
#include <glm/geometric.hpp>

namespace Rynex {
#define STANDERD_3DLAYOUTE BufferLayout{\
		{  ShaderDataType::Float3,	"a_Postion"	},\
		{  ShaderDataType::Float2,	"a_UV"	},\
		{  ShaderDataType::Float3,	"a_Normals"	},\
	}

	class Geomtrys
	{
	public:
		// Genrate Vertexes
		static void SetCubeVertex(const Ref<VertexArray>& vertexArray, Ref<VertexBuffer> vertexBuffer,const glm::vec3& size = { 0.5f, 0.5f, 0.5f }, const BufferLayout& layoute = STANDERD_3DLAYOUTE);
		static void SetSphereVertex(const Ref<VertexArray>& vertexArray, const glm::vec3& size = { 0.5f, 0.5f, 0.5f }, const BufferLayout& layoute = STANDERD_3DLAYOUTE);
		static void SetPyremadVertex(const Ref<VertexArray>& vertexArray, const glm::vec3& size = { 0.5f, 0.5f, 0.5f }, const BufferLayout& layoute = STANDERD_3DLAYOUTE);
		static void SetPlaneVertex(const Ref<VertexArray>& vertexArray, const glm::vec3& size = { 0.5f, 0.5f, 0.5f }, const BufferLayout& layoute = STANDERD_3DLAYOUTE);
		static void SetEditVertex(const Ref<VertexArray>& vertexArray, void* vertexData, uint32_t count = 1, const BufferLayout& layoute = STANDERD_3DLAYOUTE);

		static void SetQuadVertex(const Ref<VertexArray>& vertexArray, const glm::vec2& size = { 0.5f, 0.5f }, const BufferLayout& layoute = STANDERD_3DLAYOUTE);


		// Genrate Indecies
		static void SetCubeIndex(const Ref<VertexArray>& vertexArray);
		static void SetSphereIndex(const Ref<IndexBuffer>& indexBuffer);
		static void SetPyremadIndex(const Ref<IndexBuffer>& indexBuffer);
		static void SetPlaneIndex(const Ref<IndexBuffer>& indexBuffer);
		static void SetEditIndex(const Ref<IndexBuffer>& indexBuffer);

		static void SetQuadIndex(const Ref<IndexBuffer>& indexBuffer);

	};
}



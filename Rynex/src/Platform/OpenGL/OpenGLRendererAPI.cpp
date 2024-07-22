#include "rypch.h"
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace Rynex {

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		//glEnable(GL_FRONT);
	}

	void OpenGLRendererAPI::CreateComputePipline(glm::vec3& size)
	{
		
	}

	void OpenGLRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth)
	{
		glViewport(x, y, withe, heigth);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGLRendererAPI::DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawBatcheingIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{

	}

	void OpenGLRendererAPI::DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount )
	{
		
		glDrawElements(GL_TRIANGLE_STRIP, vertexArray->GetIndexBuffers()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		glDrawElements(GL_LINES, vertexArray->GetIndexBuffers()->GetCount(), GL_UNSIGNED_INT, nullptr);
		
	}

	void OpenGLRendererAPI::DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? vertexArray->GetIndexBuffers()->GetCount() : indexCount;
		glDrawElements(GL_LINE_LOOP, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::ComputePipline()
	{
		uint16_t count = 4;
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, nullptr);
	}

}
#include "rypch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Rynex {

	void OpenGLRendererAPI::Init()
	{
		RY_PROFILE_FUNCTION();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_DEPTH_TEST);

		//glEnable(GL_CULL_FACE);
		//glEnable(GL_FRONT);
	}

	void OpenGLRendererAPI::CreateComputePipline(glm::vec3& size)
	{
		RY_PROFILE_FUNCTION();
	}

	void OpenGLRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth)
	{
		RY_PROFILE_FUNCTION();
		glViewport(x, y, withe, heigth);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		RY_PROFILE_FUNCTION();
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		RY_PROFILE_FUNCTION();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGLRendererAPI::DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RY_PROFILE_FUNCTION();
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawBatcheingIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RY_PROFILE_FUNCTION();
	}

	void OpenGLRendererAPI::DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount )
	{
		RY_PROFILE_FUNCTION();
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RY_PROFILE_FUNCTION();
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RY_PROFILE_FUNCTION();
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_LINE_LOOP, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedPoints(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RY_PROFILE_FUNCTION();
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_POINT, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::ComputePipline()
	{
		RY_PROFILE_FUNCTION();
		uint16_t count = 4;
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, nullptr);

	}

	void OpenGLRendererAPI::DrawError()
	{
		RY_PROFILE_FUNCTION();
		uint16_t count = 4;
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, nullptr);
	}

}
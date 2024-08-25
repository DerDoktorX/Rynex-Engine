#include "rypch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Rynex {

	void OpenGLRendererAPI::Init()
	{
		RY_PROFILE_FUNCTION();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		SetDethTest(true);
		//glEnable(GL_CULL_FACE);
		
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

	void OpenGLRendererAPI::SetFace(CallFace callFace)
	{
		switch (callFace)
		{
		case CallFace::None:
			glDisable(GL_CULL_FACE);
			break;
		case  CallFace::Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glFrontFace(GL_CCW);
			break;
		case  CallFace::Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			break;
		case  CallFace::FrontBacke:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
			RY_CORE_WARN("Not Known Type! Thsi is Funkioning but maby not korekt!");
			break;
		default:
			RY_CORE_ASSERT(false, "NOT Defiend CallFace");
			break;
		}
	}

	void OpenGLRendererAPI::SetDethTest(bool aktiv)
	{
		if (aktiv)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
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

	void OpenGLRendererAPI::DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		RY_PROFILE_FUNCTION();
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_PATCHES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::AktivePolyGunMode(bool active)
	{
		if(active)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
#include "rypch.h"
#include "OpenGLRendererAPI.h"

#include "Rynex/Renderer/Rendering/Renderer.h"

#include <glad/glad.h>

namespace Rynex {

	void OpenGLRendererAPI::Init()
	{
		RY_PROFILE_FUNCTION();		
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

	void OpenGLRendererAPI::SetMode(int mode)
	{
		if (IsModeEqual(mode)) 
			return;
		int curentBitNumber = 0;
		for (int i = 0; i < 32 && !IsModeEqual(mode); i++)
		{
			curentBitNumber = BIT(i);
			if (BIT_EQUAL(mode, curentBitNumber) && !BIT_EQUAL(m_CurentMode, curentBitNumber))
			{
				ModeEnable(curentBitNumber);
				m_CurentMode = BIT_SET_ON(1, curentBitNumber, m_CurentMode);
			}
			else if (!BIT_EQUAL(mode, curentBitNumber) && BIT_EQUAL(m_CurentMode, curentBitNumber))
			{
				ModeDisenable(curentBitNumber);
				m_CurentMode = BIT_SET_ON(0, curentBitNumber, m_CurentMode);
			}
		}
	}

	int OpenGLRendererAPI::GetMode()
	{
		return m_CurentMode;
	}

	bool OpenGLRendererAPI::IsModeEqual(int mode)
	{
		return mode == m_CurentMode;
	}


	void OpenGLRendererAPI::Clear()
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void OpenGLRendererAPI::DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
#if 0
		RY_CORE_INFO("DrawIndexedMesh IndexBuffer renderID = {0}, count = {1}, indexCount {2} ",vertexArray->GetIndexBuffers()->GetRenderID(), count, indexCount);
#endif
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawBatcheingIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
	}

	void OpenGLRendererAPI::DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount )
	{

		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_LINE_LOOP, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedPoints(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffers()->GetCount();
		glDrawElements(GL_POINT, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
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

	void OpenGLRendererAPI::DispatcheCompute(const glm::vec<3, uint32_t>& groups)
	{
		glDispatchCompute(groups.x, groups.y, groups.z);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	void OpenGLRendererAPI::DrawError()
	{
		uint16_t count = 4;
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_SHORT, nullptr);
	}

	void OpenGLRendererAPI::ModeEnable(int bitCount)
	{
		switch (bitCount)
		{
			case Renderer::A_Buffer:
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			}
			case Renderer::Death_Buffer:
			{
				glEnable(GL_DEPTH_TEST);
				break;
			}
			case Renderer::WireFrame:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			}
			case Renderer::CallFace_Nono:
			{
				glDisable(GL_CULL_FACE);
				break;
			}
			case Renderer::CallFace_Front:
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glFrontFace(GL_CCW);
				break;
			}
			case Renderer::CallFace_Back:
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				break;
			}
			case Renderer::CallFace_FrontBack:
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
				glFrontFace(GL_CCW);
				RY_CORE_WARN("Not Known Type! Thsi is Funkioning but maby not korekt!");
				break;
			}
		
		}
	}

	void OpenGLRendererAPI::ModeDisenable(int bitCount)
	{
		switch (bitCount)
		{
			case Renderer::A_Buffer:
			{
				glDisable(GL_BLEND);
				break;
			}
			case Renderer::Death_Buffer:
			{
				glDisable(GL_DEPTH_TEST);
				break;
			}
			case Renderer::WireFrame:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			}
		}
	}

}
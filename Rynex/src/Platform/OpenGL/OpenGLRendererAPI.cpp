#include "rypch.h"
#include "OpenGLRendererAPI.h"

#include <Rynex/Renderer/Rendering/Renderer.h>

#include <Platform/OpenGL/OpenGLFramebuffer.h>
#include <Platform/OpenGL/OpenGLBase.h>
#include <Platform/OpenGL/OpenGLVertexArray.h>
#include <Platform/OpenGL/OpenGLBuffer.h>
// #define RY_OPENGL_ALL_BINDING_RESET

#define CHECK_BINDINGS_ASSERTi(type)\
{\
	GLint bufferType = 0; \
	glGetIntegerv(type, &bufferType); \
	RY_CORE_ASSERT(bufferType != 0, #type " has Zero Bindings!");\
}

#define CHECK_BINDINGS_ASSERTb(type, isEquel)\
{\
	GLboolean bufferType = GL_FALSE; \
	glGetBooleanv( type, &bufferType); \
	RY_CORE_ASSERT(bufferType == isEquel, #type " is not Equel " #isEquel "!");\
}
#if 0
namespace robin_hood {

	template<>
	struct hash<Rynex::AktiveBindlesTexture, void>
	{
		std::size_t operator()(const Rynex::AktiveBindlesTexture& bindelssTexture) const
		{
			std::size_t hashV = bindelssTexture.GetHash();
			return hashV;
		}




	};
}
#endif

#define RY_CHECK_BIND_ON_MAIN_THREAD()	RY_CORE_ASSERT(::Rynex::Asset::CurrentOnMainThread(), "Bining aktion on not Main Thread!")

#if 1
			
#if 0
	#define RY_CHECK_BUFFER_BIND_STATE(x)	\
		RY_CHECK_BIND_ON_MAIN_THREAD();		\
		if(x == renderID)					\
			return;							\
											\
		x = renderID						
#else
	#define RY_CHECK_BUFFER_BIND_STATE(x)	\
		if(x == renderID)					\
			return;							\
											\
		x = renderID						
#endif

#else
	#define RY_CHECK_BUFFER_BIND_STATE(x)	x = renderID
#endif

namespace Rynex {


	void OpenGLRendererAPI::Init()
	{

		RY_PROFILE_FUNCTION();
		m_BindFrameBufferState = 0u;
		m_BindShaderState = 0u;
		m_BindIndexBufferState = 0u;
		m_BindIndirectState = 0u;
		m_BindVertexArrayState = 0u;
		m_BindUnifomrBufferState = 0u;
		m_BindVertexBufferState = 0u;
		m_BindStorageBufferState = 0u;


		m_CurentMode = 0;

		m_HigestBindSamplerStateSlot = -1;
		m_HigestBindStorageStateSlot = -1;
		m_HigestBindTextureStateSlot = -1;
		m_HigestBindUniformBufferStateSlot = -1;
		m_HigestBindVertexBufferStateSlot = -1;

		GLint maxTextureCount = 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureCount);
		m_BindTextureStateVec.resize(maxTextureCount, 0u);
		m_BindSamplerStateVec.resize(maxTextureCount, 0u);
		RY_CORE_INFO("OpenGL Max Sampler/Texture Bindings Points {}", maxTextureCount);

		GLint maxUniformsCount = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformsCount);
		m_BindUniformBufferStateVec.resize(maxUniformsCount, 0u);
		RY_CORE_INFO("OpenGL Max Uniform Buffer Bindings Points {}", maxUniformsCount);

		GLint maxStorageCount = 0;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxStorageCount);
		m_BindStorageStateVec.resize(maxStorageCount, 0u);
		RY_CORE_INFO("OpenGL Max Shader Storage Buffer Bindings Points {}", maxStorageCount);

		GLint maxVertexCount = 0;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxVertexCount);
		m_BindVertexBufferStateVec.resize(maxVertexCount, 0u);
		RY_CORE_INFO("OpenGL Max Shader Storage Buffer Bindings Points {}", maxVertexCount);

		GLint defaultFrambufferRenderID = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &defaultFrambufferRenderID);
		m_DefaultFrambufferRenderID = static_cast<uint32_t>(defaultFrambufferRenderID);

		OpenGLRenderCommand::Init(this);
	}

	void OpenGLRendererAPI::ShutDown()
	{
		OpenGLRenderCommand::ShutDown();
	}

	void OpenGLRendererAPI::CreateComputePipline(glm::vec3& size)
	{
		RY_CORE_NOT_IMPL();
	}

	void OpenGLRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth)
	{
		glViewport(x, y, withe, heigth);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{

		glClearColor(color.r, color.g, color.b, color.a);
	}


	void OpenGLRendererAPI::BindFrambuffer(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindFrameBufferState);
		glBindFramebuffer(GL_FRAMEBUFFER, renderID);
	}

	void OpenGLRendererAPI::BindShader(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindShaderState);
		glUseProgram(renderID);
	}

	void OpenGLRendererAPI::BindVertexArray(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindVertexArrayState);
		glBindVertexArray(renderID);
	}
#ifdef RY_OPENGL_UN_RESOURCESE
	void OpenGLRendererAPI::UnBindBuffer(uint32_t target)
	{
		constexpr uint32_t resetBindNumber = 0u;
		switch (target)
		{
		case GL_ELEMENT_ARRAY_BUFFER:
			BindIndexBuffer(resetBindNumber);
			break;
		case GL_DRAW_INDIRECT_BUFFER:
			BindIndrectBuffer(resetBindNumber);
			break;
		case GL_SHADER_STORAGE_BUFFER:
			BindStorageBuffer(resetBindNumber);
			break;
		case GL_ARRAY_BUFFER:
			BindVertexArray(resetBindNumber);
			break;
		case GL_UNIFORM_BUFFER:
			BindUniformBuffer(resetBindNumber);
			break;
		default:
			RY_CORE_ASSERT(false);
			break;
		}
	}

	void OpenGLRendererAPI::UnBindIndexBuffer(uint32_t renderID)
	{
		
		
	}

	void OpenGLRendererAPI::UnBindUniformBuffer(uint32_t renderID)
	{
		

	}

	

	void OpenGLRendererAPI::UnBindVertexBufferSlot(uint32_t slot, uint32_t renderID)
	{
		constexpr uint32_t resetBindSlotBuffer = 0u;
		uint32_t i = 0u;
		for (uint32_t& buffer : m_BindVertexBufferStateVec)
		{
			if (buffer == renderID)
			{
				buffer = resetBindSlotBuffer;
				glBindBufferBase(GL_ARRAY_BUFFER, i, resetBindSlotBuffer);
				return;
			}
			i++;
		}
	}

	void OpenGLRendererAPI::UnBindUniformBufferSlot(uint32_t slot, uint32_t renderID)
	{
		constexpr uint32_t resetBindSlotBuffer = 0u;
		uint32_t i = 0u;
		for (uint32_t& buffer : m_BindUniformBufferStateVec)
		{
			if (buffer == renderID)
			{
				buffer = resetBindSlotBuffer;
				glBindBufferBase(GL_UNIFORM_BUFFER, i, resetBindSlotBuffer);
				return;
			}
			i++;
		}
	}

	void OpenGLRendererAPI::UnBindStorageBufferSlot(uint32_t slot, uint32_t renderID)
	{
		constexpr uint32_t resetBindSlotBuffer = 0u;
		uint32_t i = 0u;
		for (uint32_t& buffer : m_BindStorageStateVec)
		{
			if (buffer == renderID)
			{
				buffer = resetBindSlotBuffer;
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, resetBindSlotBuffer);
				return;
			}
			i++;
		}
	}
#endif
	void OpenGLRendererAPI::BindBuffer(uint32_t target, uint32_t renderID)
	{
		switch (target)
		{
		case GL_ELEMENT_ARRAY_BUFFER:
			BindIndexBuffer(renderID);
			break;
		case GL_DRAW_INDIRECT_BUFFER:
			BindIndrectBuffer(renderID);
			break;
		case GL_SHADER_STORAGE_BUFFER:
			BindStorageBuffer(renderID);
			break;
		case GL_ARRAY_BUFFER:
			BindVertexArray(renderID);
			break;
		case GL_UNIFORM_BUFFER:
			BindUniformBuffer(renderID);
			break;
		default:
			RY_CORE_ASSERT(false);
			break;
		}
	}

	void OpenGLRendererAPI::BindBufferSlot(uint32_t target, uint32_t slot, uint32_t renderID)
	{

		switch (target)
		{
		case GL_ARRAY_BUFFER:
			BindVertexBufferSlot(slot, renderID);
			break;
		case GL_SHADER_STORAGE_BUFFER:
			BindStorageBufferSlot(slot, renderID);
			break;

		case GL_UNIFORM_BUFFER:
			BindUniformBufferSlot(slot, renderID);
			break;
		
		default:
			RY_CORE_ASSERT(false);
			break;
		}
	}

	void OpenGLRendererAPI::BindIndexBuffer(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindIndexBufferState);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderID);
	}

	void OpenGLRendererAPI::BindIndrectBuffer(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindIndirectState);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderID);
	}

	void OpenGLRendererAPI::BindVertexBuffer(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindVertexBufferState);

		glBindBuffer(GL_ARRAY_BUFFER, renderID);
	}

	void OpenGLRendererAPI::BindStorageBuffer(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindStorageBufferState);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderID);
	}

	void OpenGLRendererAPI::BindTexture(uint32_t renderID)
	{
		RY_CORE_NOT_IMPL();

	}

	void OpenGLRendererAPI::BindUniformBuffer(uint32_t renderID)
	{
		RY_CHECK_BUFFER_BIND_STATE(m_BindUnifomrBufferState);

		glBindBuffer(GL_UNIFORM_BUFFER, renderID);
	}


	void OpenGLRendererAPI::BindTextureSlot(uint32_t slot, uint32_t renderID)
	{		
		RY_CORE_ASSERT(slot < m_BindTextureStateVec.size());
		if (m_BindTextureStateVec[slot] == renderID)
			return;
		m_BindTextureStateVec[slot] = renderID;
		if (0u != renderID && m_HigestBindTextureStateSlot < static_cast<int>(slot))
			m_HigestBindTextureStateSlot = static_cast<int>(slot);
		else if (0u == renderID && static_cast<int>(slot) == m_HigestBindTextureStateSlot)
			m_HigestBindTextureStateSlot--;
		glBindTextureUnit(slot, renderID);
	}

	void OpenGLRendererAPI::BindSamplerSlot(uint32_t slot, uint32_t renderID)
	{
		RY_CORE_ASSERT(slot < m_BindSamplerStateVec.size());
		if (m_BindSamplerStateVec[slot] == renderID)
			return;
		m_BindSamplerStateVec[slot] = renderID;
		if (0u != renderID && m_HigestBindSamplerStateSlot < static_cast<int>(slot) )
			m_HigestBindSamplerStateSlot = static_cast<int>(slot);
		else if (0u == renderID && static_cast<int>(slot) == m_HigestBindSamplerStateSlot)
			m_HigestBindSamplerStateSlot--;
		glBindSampler(slot, renderID);
	}

	
	void OpenGLRendererAPI::BindVertexBufferSlot(uint32_t slot, uint32_t renderID)
	{
		RY_CORE_ASSERT(slot < m_BindVertexBufferStateVec.size());
		if (m_BindVertexBufferStateVec[slot] == renderID)
			return;
		m_BindVertexBufferStateVec[slot] = renderID;
		if (0u != renderID && m_HigestBindVertexBufferStateSlot < static_cast<int>(slot))
			m_HigestBindVertexBufferStateSlot = static_cast<int>(slot);
		else if (0u == renderID && static_cast<int>(slot) == m_HigestBindVertexBufferStateSlot)
			m_HigestBindVertexBufferStateSlot--;
		glBindBufferBase(GL_ARRAY_BUFFER, slot, renderID);

	}

	void OpenGLRendererAPI::BindUniformBufferSlot(uint32_t slot, uint32_t renderID)
	{
		RY_CORE_ASSERT(slot < m_BindUniformBufferStateVec.size());
		if (m_BindUniformBufferStateVec[slot] == renderID)
			return;
		m_BindUniformBufferStateVec[slot] = renderID;
		if (0u != renderID && m_HigestBindUniformBufferStateSlot < static_cast<int>(slot))
			m_HigestBindUniformBufferStateSlot = slot;
		else if (0u == renderID && static_cast<int>(slot) == m_HigestBindUniformBufferStateSlot)
			m_HigestBindUniformBufferStateSlot--;
		glBindBufferBase(GL_UNIFORM_BUFFER, slot, renderID);
	}

	void OpenGLRendererAPI::BindStorageBufferSlot(uint32_t slot, uint32_t renderID)
	{
		RY_CORE_ASSERT(slot < m_BindStorageStateVec.size());
		if (m_BindStorageStateVec[slot] == renderID)
			return;
		m_BindStorageStateVec[slot] = renderID;
		if (0u != renderID && m_HigestBindStorageStateSlot < static_cast<int>(slot))
			m_HigestBindStorageStateSlot = slot;
		else if (0u == renderID && static_cast<int>(slot) == m_HigestBindStorageStateSlot)
			m_HigestBindStorageStateSlot--;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, renderID);
	}

	void OpenGLRendererAPI::SetFace(CallFace callFace)
	{
		constexpr int allFaceActive = RenderMode::CallFace_None | RenderMode::CallFace_FrontBack | RenderMode::CallFace_Front | RenderMode::CallFace_Back;
		constexpr int allFaceInverse = BIT_NOT(allFaceActive);
		m_CurentMode &= allFaceInverse;
		switch (callFace)
		{
		case CallFace::None:
			glDisable(GL_CULL_FACE);
			m_CurentMode |= RenderMode::CallFace_None;

			break;
		case  CallFace::Front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glFrontFace(GL_CCW);
			m_CurentMode |= RenderMode::CallFace_Front;

			break;
		case  CallFace::Back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			m_CurentMode |= RenderMode::CallFace_Back;

			break;
		case  CallFace::FrontBacke:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
			m_CurentMode |= RenderMode::CallFace_FrontBack;
			
			RY_CORE_WARN("Not Known Type! Thsi is Funkioning but maby not korekt!");
			break;
		default:
			RY_CORE_ASSERT(false, "NOT Defiend CallFace");
			break;
		}
	}

	

	void OpenGLRendererAPI::SetBiasGPU(float factor, float units)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(factor, units);
	}

	void OpenGLRendererAPI::DisableBiasGPU()
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
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
			bool modeIsEqual = BIT_EQUAL(mode, curentBitNumber);
			bool curentModeIsEqual = BIT_EQUAL(m_CurentMode, curentBitNumber);
			if (modeIsEqual && !curentModeIsEqual)
			{
				ModeEnable(curentBitNumber);
				m_CurentMode = BIT_SET_ON(1, curentBitNumber, m_CurentMode);
			}
			else if (!modeIsEqual && curentModeIsEqual)
			{
				ModeDisenable(curentBitNumber);
				m_CurentMode = BIT_SET_ON(0, curentBitNumber, m_CurentMode);
			}
		}
	}

	void OpenGLRendererAPI::SetModeForce(int mode)
	{
		int curentBitNumber = 0;
		for (int i = 0; i < 32 && !IsModeEqual(mode); i++)
		{
			curentBitNumber = BIT(i);
			bool modeIsEqual = BIT_EQUAL(mode, curentBitNumber);
			bool curentModeIsEqual = BIT_EQUAL(m_CurentMode, curentBitNumber);
			if (modeIsEqual)
			{
				ModeEnable(curentBitNumber);
				m_CurentMode = BIT_SET_ON(1, curentBitNumber, m_CurentMode);
			}
			else if (!modeIsEqual)
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

	void OpenGLRendererAPI::ClearNoDepth()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLRendererAPI::ClearDepth()
	{
		
		glClear(GL_DEPTH_BUFFER_BIT);
		// glDepthMask(GL_TRUE);
		// glDepthFunc(GL_LESS);
		// glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
#if 0
		RY_CORE_INFO("DrawIndexedMesh IndexBuffer renderID = {0}, count = {1}, indexCount {2} ",vertexArray->GetIndexBuffers()->GetRenderID(), count, indexCount);
#endif
		
		GL_CHECK_LOOP();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexedMeshInstecing(uint32_t instecing, const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		
		glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instecing);
		
	}

	typedef  struct {
		GLuint  count;
		GLuint  instanceCount;
		GLuint  firstIndex;
		GLuint  baseVertex;
		GLuint  baseInstance;
	} DrawElementsIndirectCommandOpenGL;

	void OpenGLRendererAPI::DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, void* indrictDraw, uint32_t indirectStrideSize)
	{
		uint32_t stride = sizeof(DrawElementsIndirectCommandOpenGL) - indirectStrideSize;
		Ref<OpenGLVertexArray> vertexArrayGL = std::static_pointer_cast<OpenGLVertexArray>(vertexArray);
		GLenum mode = vertexArrayGL->GetPrimitvOpenGLMode();

		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, indrictDraw, drawCount, stride);
	}

	void OpenGLRendererAPI::DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, uint32_t indirectStrideSize)
	{
		uint32_t stride = sizeof(DrawElementsIndirectCommandOpenGL) - indirectStrideSize;
		vertexArray->Bind();
		Ref<OpenGLVertexArray> vertexArrayGL = std::static_pointer_cast<OpenGLVertexArray>(vertexArray);
		GLenum mode = vertexArrayGL->GetPrimitvOpenGLMode();


		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT,nullptr, drawCount, stride);
	}

	

	void OpenGLRendererAPI::DrawElement(const Ref<VertexArray>& vertexArray, const Mesh::PerDrawObject& drawObject)
	{
		vertexArray->Bind();
		// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

#if GL_CHECK_BINDINGS_BEFOR_DRAW
		CHECK_BINDINGS_ASSERTi(GL_DRAW_FRAMEBUFFER_BINDING);
		CHECK_BINDINGS_ASSERTi(GL_CURRENT_PROGRAM);
		CHECK_BINDINGS_ASSERTi(GL_VERTEX_ARRAY_BINDING);
		CHECK_BINDINGS_ASSERTi(GL_UNIFORM_BUFFER_BINDING);
		CHECK_BINDINGS_ASSERTi(GL_SHADER_STORAGE_BUFFER_BINDING);
#endif

		Ref<OpenGLVertexArray> vertexArrayGL = std::static_pointer_cast<OpenGLVertexArray>(vertexArray);
		GLenum mode = vertexArrayGL->GetPrimitvOpenGLMode();

		uint32_t indexCount = drawObject.Count;
		uint32_t indexfirst = drawObject.FirstIndex;
		uint32_t instancesCount = drawObject.InstancesCount;
		uint32_t baseInstances = drawObject.BaseInstance;
		int baseVertex = drawObject.BaseVertex;
		void* indexOffset = (void*)(indexfirst * sizeof(uint32_t));
		
		GL_CHECK_LOOP();
		glDrawElementsInstancedBaseVertexBaseInstance(mode, indexCount, GL_UNSIGNED_INT, indexOffset, instancesCount, baseVertex, baseInstances);
		GL_CHECK_LOOP();

	}

	void OpenGLRendererAPI::DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer, uint32_t drawCount)
	{
		RY_CORE_ASSERT(indriectBuffer->GetCount() >= drawCount, "To many DrawCounts");
		uint32_t stride = sizeof(DrawElementsIndirectCommandOpenGL) - indriectBuffer->GetStrideSize();
		vertexArray->Bind();
		indriectBuffer->Bind();
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, drawCount, stride);
	}

	void OpenGLRendererAPI::DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer)
	{
		uint32_t stride = sizeof(DrawElementsIndirectCommandOpenGL) - indriectBuffer->GetStrideSize();
		vertexArray->Bind();
		indriectBuffer->Bind();
		// glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

#if GL_CHECK_BINDINGS_BEFOR_DRAW
		CHECK_BINDINGS_ASSERTi(GL_DRAW_FRAMEBUFFER_BINDING);
		CHECK_BINDINGS_ASSERTi(GL_CURRENT_PROGRAM);
		CHECK_BINDINGS_ASSERTi(GL_VERTEX_ARRAY_BINDING);
		CHECK_BINDINGS_ASSERTi(GL_UNIFORM_BUFFER_BINDING);
		CHECK_BINDINGS_ASSERTi(GL_SHADER_STORAGE_BUFFER_BINDING);
#endif
		// CHECK_BINDINGS_ASSERTb(GL_DEPTH_TEST, GL_TRUE);


		
	
		Ref<OpenGLVertexArray> vertexArrayGL = std::static_pointer_cast<OpenGLVertexArray>(vertexArray);
		GLenum mode = vertexArrayGL->GetPrimitvOpenGLMode();
		uint32_t count = indriectBuffer->GetCount();
#if 0
		const DrawElementsIndirectCommandOpenGL* indirect = (const DrawElementsIndirectCommandOpenGL*)indriectBuffer->GetDataPtr();
		
		GLsizei n;
		for (n = 0; n < count; n++) 
		{
			const DrawElementsIndirectCommandOpenGL* cmd;
			if (stride != 0) 
			{
				cmd = (const DrawElementsIndirectCommandOpenGL*)((char*)indirect + n * stride);
				RY_CORE_ASSERT(false, "Check is Corect!");
			}
			else 
			{
				cmd = (const DrawElementsIndirectCommandOpenGL*)indirect + n;
			}

			glDrawElementsInstancedBaseVertexBaseInstance(mode,
				cmd->count,
				GL_UNSIGNED_INT,
				(void*)(cmd->firstIndex * sizeof(uint32_t)),
				cmd->instanceCount,
				cmd->baseVertex,
				cmd->baseInstance);
		}
#else
		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, nullptr, count, stride);
#endif
		GL_CHECK_LOOP();

		// glFinish();
		// glFlush();
	}
	

	void OpenGLRendererAPI::DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount )
	{

		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);

	}

	

	void OpenGLRendererAPI::DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_LINE_LOOP, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexedPoints(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_POINT, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		
		glDrawElements(GL_PATCHES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::RestPipline()
	{

#ifdef RY_OPENGL_ALL_BINDING_RESET
		GLint maxTextureUnits= 0;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
#else
		int maxTextureUnits = m_HigestBindSamplerStateSlot < m_HigestBindTextureStateSlot ? m_HigestBindTextureStateSlot : m_HigestBindSamplerStateSlot;
		maxTextureUnits += 1;
#endif
		for (int i = 0; i < maxTextureUnits; i++) 
		{
			OpenGLRendererAPI::BindTextureSlot(i, 0u);      // Textur-Binding reset
			OpenGLRendererAPI::BindSamplerSlot(i, 0u);		// Sampler-Binding reset
		}
		m_HigestBindSamplerStateSlot = -1;
		m_HigestBindTextureStateSlot = -1;


#ifdef RY_OPENGL_ALL_BINDING_RESET
		GLint maxUniformBufferSlots = 0;
		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferSlots);
#else
		int maxUniformBufferSlots = m_HigestBindUniformBufferStateSlot + 1;
#endif

		for (int i = 0; i < maxUniformBufferSlots; i++) 
		{
			OpenGLRendererAPI::BindUniformBufferSlot(i, 0u);
		}
		m_HigestBindUniformBufferStateSlot = -1;
		
		
#ifdef RY_OPENGL_ALL_BINDING_RESET
		GLint maxShaderStorageBufferSlots = 0;
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxUniformBufferSlots);
#else
		int maxShaderStorageBufferSlots = m_HigestBindStorageStateSlot + 1;
#endif

		for (int i = 0; i < maxShaderStorageBufferSlots; i++)
		{
			OpenGLRendererAPI::BindStorageBufferSlot(i, 0);
		}
		m_HigestBindStorageStateSlot = -1;
		

#ifdef RY_OPENGL_ALL_BINDING_RESET
		GLint maxVertexBufferSlots = 0;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &maxVertexBufferSlots);
#else
		int maxVertexBufferSlots = m_HigestBindVertexBufferStateSlot + 1;
#endif
		for (int i = 0; i < maxVertexBufferSlots; i++)
		{
			OpenGLRendererAPI::BindVertexBufferSlot(i, 0);
		}
		m_HigestBindVertexBufferStateSlot = -1;

		
		OpenGLRendererAPI::BindVertexArray(0u);
		OpenGLRendererAPI::BindIndrectBuffer(0u);
		OpenGLRendererAPI::BindIndexBuffer(0u);
		OpenGLRendererAPI::BindShader(0u);
		OpenGLRendererAPI::BindFrambuffer(m_DefaultFrambufferRenderID);
		glm::uvec2 size = OpenGL::GetMainWindowCurentSize();
		SetViewPort(0, 0, size.x, size.y);

	}

	void OpenGLRendererAPI::PrintCurentStatePipline()
	{
		RY_CORE_WARN("Frame Buffer({}) Bind", m_BindFrameBufferState);
		RY_CORE_WARN("Shader ({}) Bind", m_BindShaderState);
		RY_CORE_WARN("VertexArray ({}) Bind", m_BindVertexArrayState);
		RY_CORE_WARN("Index Buffer({}) Bind", m_BindIndexBufferState);
		RY_CORE_WARN("Indrect Buffer ({}) Bind", m_BindIndirectState);
		int slotIndex = 0;
		for (uint32_t i : m_BindVertexBufferStateVec)
		{
			if (i != 0u)
			{
				RY_CORE_WARN("Vertex Buffer({}) Slot: {}", slotIndex, i);
			}

			slotIndex++;
		}
		slotIndex = 0;
		for (uint32_t i : m_BindTextureStateVec)
		{
			if (i != 0u)
			{
				RY_CORE_WARN("Texture ({}) Slot: {}", slotIndex, i);
			}

			if (m_BindSamplerStateVec[slotIndex] != 0u)
			{
				RY_CORE_WARN("Sampler Buffer({}) Slot: {}", slotIndex, m_BindSamplerStateVec[slotIndex]);
			}
			slotIndex++;
		}
		slotIndex = 0;
		for (uint32_t i : m_BindUniformBufferStateVec)
		{
			if (i != 0u)
			{
				RY_CORE_WARN("Uniform Buffer({}) Slot: {}", slotIndex, i);
			}
			slotIndex++;
		}
		slotIndex = 0;
		for (uint32_t i : m_BindStorageStateVec)
		{
			if(i != 0u)
			{
				RY_CORE_WARN("Storage Buffer({}) Slot: {}", slotIndex, i);
			}
			slotIndex++;
		}
		
		

		

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
		glDrawElements(GL_TRIANGLE_STRIP, count, GL_UNSIGNED_INT, nullptr);
	}


	


	void OpenGLRendererAPI::ModeEnable(int bitCount)
	{
		switch (bitCount)
		{
		case RenderMode::None:
			{
				break;
			}

			case RenderMode::A_Buffer:
			{
				glEnable(GL_DITHER);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			}
			case RenderMode::Death_Buffer:
			{
				glEnable(GL_DEPTH_TEST);
				break;
			}
			case RenderMode::WireFrame:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			}
			case RenderMode::CallFace_None:
			{
				glDisable(GL_CULL_FACE);
				break;
			}
			case RenderMode::CallFace_Front:
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				glFrontFace(GL_CCW);
				break;
			}
			case RenderMode::CallFace_Back:
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				break;
			}
			case RenderMode::CallFace_FrontBack:
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
				glFrontFace(GL_CCW);
				RY_CORE_WARN("Not Known Type! Thsi is Funkioning but maby not korekt!");
				break;
			}
			case RenderMode::Gamma:
			{
				glEnable(GL_FRAMEBUFFER_SRGB);
				break;
			}
			case RenderMode::PrimitivReset:
			{
				glEnable(GL_PRIMITIVE_RESTART);
				uint32_t restValue = static_cast<uint32_t>(-1);
				glPrimitiveRestartIndex(restValue);
				break;
			}
			
		}
	}

	void OpenGLRendererAPI::ModeDisenable(int bitCount)
	{
		switch (bitCount)
		{
			case RenderMode::None:
			{
				break;
			}
#if 0
			case Renderer::CallFace_None:
			{
				glDisable(GL_CULL_FACE);
				break;
			}
			case Renderer::CallFace_Front:
			{
				glDisable(GL_CULL_FACE);
				// glCullFace(GL_FRONT);
				// glFrontFace(GL_CCW);
				break;
			}
			case Renderer::CallFace_Back:
			{
				glDisable(GL_CULL_FACE);
				// glCullFace(GL_BACK);
				// glFrontFace(GL_CCW);
				break;
			}
			case Renderer::CallFace_FrontBack:
			{
				glDisable(GL_CULL_FACE);
				// glCullFace(GL_FRONT_AND_BACK);
				// glFrontFace(GL_CCW);
				RY_CORE_WARN("Not Known Type! Thsi is Funkioning but maby not korekt!");
				break;
			}
#endif
			case RenderMode::A_Buffer:
			{
				glDisable(GL_DITHER);
				glDisable(GL_BLEND);
				break;
			}
			case RenderMode::Death_Buffer:
			{
				glDisable(GL_DEPTH_TEST);
				break;
			}
			case RenderMode::WireFrame:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			}
			case RenderMode::Gamma:
			{
				glDisable(GL_FRAMEBUFFER_SRGB);
				break;
			}
			case RenderMode::PrimitivReset:
			{
				glDisable(GL_PRIMITIVE_RESTART);
				break;
			}
			
		}
	}

	uint32_t OpenGLRendererAPI::GetDefaultFrambufferRenderID() const
	{
		return m_DefaultFrambufferRenderID;
	}

}
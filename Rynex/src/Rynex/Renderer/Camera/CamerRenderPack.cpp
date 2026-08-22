#include "rypch.h"
#include "CamerRenderPack.h"

#include <Rynex/Renderer/Rendering/Renderer.h>

namespace Rynex {

	namespace Utils {


		template<typename N>
		static void UpdatePackegeBuffer(BufferDataPackage<UniformBuffer, N>& packege, uint32_t slot = 0)
		{

			if (packege.HasNoBuffer())
			{
				const N& data = packege.GetData();
				const N* dataPtr = &data;
				uint32_t sizeByte = sizeof(N);

				Ref<UniformBuffer> buffer = UniformBuffer::Create(dataPtr, sizeByte);
				packege.SetBuffer(buffer);
				packege.ForceLoadeAllDataUpNeeded();

				
				
			}
			else if (packege.NeedUpdated())
			{
				packege.ForceLoadeAllDataUpNeeded();
			}
		}

	}

	CamerRenderPackages::CamerRenderPackages()
		: m_RenderTarget()
		, m_CamerPackage(CamerPackage())
		, m_DisplayPackage(DisplayPackage())
		, m_DebugePackage(DebugCamerPackage())
	{
	}

	CamerRenderPackages::CamerRenderPackages(const Ref<Framebuffer>& fb, const glm::ivec4& display, const glm::mat4& model, const glm::mat4& projection)
		: m_RenderTarget(fb)
		, m_CamerPackage(CamerPackage())
		, m_DisplayPackage(display)
		, m_DebugePackage(DebugCamerPackage())
	{
		CameraData& camerData = m_CamerPackage.ConfigData();
		camerData.ProjectionMatrix = projection;
		SetCamerDataModel(model);
	}

	CamerRenderPackages::CamerRenderPackages(const Ref<Framebuffer>& fb, const glm::mat4& model, const glm::mat4& projection)
		: m_RenderTarget(fb)
		, m_CamerPackage(CamerPackage())
		, m_DisplayPackage(DisplayPackage(DisplayData()))
		, m_DebugePackage(DebugCamerPackage())
	{
		DisplayData diplayData = m_DisplayPackage.ConfigData();
		const glm::uvec3& size = fb->GetFrambufferSize();
		diplayData.ViewPortConf.x = static_cast<int>(size.x);
		diplayData.ViewPortConf.y = static_cast<int>(size.y);
		diplayData.ViewPortConf.w = 0;
		diplayData.ViewPortConf.z = 0;

		CameraData& camerData = m_CamerPackage.ConfigData();
		camerData.ProjectionMatrix = projection;
		SetCamerDataModel(model);
	}

	

	CamerRenderPackages::~CamerRenderPackages()
	{
		DestoryBuffers();
	}

	void CamerRenderPackages::SetFromViewPass(const ViewPassData& viewPass)
	{
		CameraData& camerData = m_CamerPackage.ConfigData();
		glm::vec3 forward = glm::normalize(
			glm::vec3(
				viewPass.ViewMatrix[0].z,
				viewPass.ViewMatrix[1].z,
				viewPass.ViewMatrix[2].z
			));
		camerData = CameraData{
			viewPass.ProjetionViewMatrix,
			viewPass.ViewMatrix,
			viewPass.ProjetionMatrix,
			viewPass.Postion,
			forward,
			viewPass.GammeCorection
		};
		
		SetDisplayDataModel(viewPass.ViewSpace);

		glm::mat4 debugMat4 = glm::inverse(camerData.ViewProjectionMatrix);
		m_DebugePackage.SetData(debugMat4);

		SetFrambuffer(viewPass.FrameBuffer);
	}

	

	void CamerRenderPackages::SetCamerDataModel(const glm::mat4& model)
	{
		CameraData& camerData = m_CamerPackage.ConfigData();
		glm::mat4 view = glm::inverse(model);
		camerData.ViewMatrix = view;
		camerData.ViewProjectionMatrix = camerData.ViewMatrix * camerData.ProjectionMatrix;
		camerData.Position = glm::vec3(model[3].x, model[3].y, model[3].z);
		camerData.ViewDirection = glm::vec3(view[0].z, view[1].z, view[2].z);

		
		glm::mat4 debugMat4 = glm::inverse(camerData.ViewProjectionMatrix);
		m_DebugePackage.SetData(debugMat4);
	}

	void CamerRenderPackages::SetCamerDataView(const glm::mat4& view)
	{ 
		CameraData& camerData = m_CamerPackage.ConfigData();
		glm::mat4 model = glm::inverse(view);
		camerData.ViewMatrix = view;
		camerData.ViewProjectionMatrix = camerData.ViewMatrix * camerData.ProjectionMatrix;
		camerData.Position = glm::vec3(model[3]);
		camerData.ViewDirection = glm::vec3(view[0].z, view[1].z, view[2].z);

		glm::mat4 debugMat4 = glm::inverse(camerData.ViewProjectionMatrix);
		m_DebugePackage.SetData(debugMat4);
	}

	void CamerRenderPackages::SetCamerDataProjection(const glm::mat4& projection)
	{
		CameraData& camerData = m_CamerPackage.ConfigData();
		camerData.ProjectionMatrix = projection;
		camerData.ViewProjectionMatrix = camerData.ViewMatrix * camerData.ProjectionMatrix;


		glm::mat4 debugMat4 = glm::inverse(camerData.ViewProjectionMatrix);
		m_DebugePackage.SetData(debugMat4);
	}

	void CamerRenderPackages::SetCamerDataViewProjection(const glm::mat4& view, const glm::mat4& projection)
	{
		CameraData& camerData = m_CamerPackage.ConfigData();


		glm::mat4 model = glm::inverse(view);
		camerData.ViewMatrix = view;
		camerData.ProjectionMatrix = projection;

		camerData.ViewProjectionMatrix = camerData.ViewMatrix * camerData.ProjectionMatrix;
		camerData.Position = glm::vec3(model[3]);
		camerData.ViewDirection = glm::vec3(view[0].z, model[1].z, model[2].z);

		glm::mat4 debugMat4 = glm::inverse(camerData.ViewProjectionMatrix);
		m_DebugePackage.SetData(debugMat4);
	}

	void CamerRenderPackages::SetDisplayDataModel(const glm::ivec4& display)
	{
		m_DisplayPackage.SetData(display);
		m_RenderTarget.ResizeView(display);
	}


	void CamerRenderPackages::SetCamerPackageOnSlot(Ref<ShaderDrawList>& list, uint32_t bindSlot)
	{
		RY_CORE_ASSERT(bindSlot < list->GetBindeUniform().size(), "Data Overflow!");
		list->GetBindeUniform()[bindSlot] = m_CamerPackage.GetBuffer();
	}

	void CamerRenderPackages::SeteDisplayPackageOnSlot(Ref<ShaderDrawList>& list, uint32_t bindSlot)
	{
		RY_CORE_ASSERT(bindSlot < list->GetBindeUniform().size(), "Data Overflow!");
		list->GetBindeUniform()[bindSlot] = m_DisplayPackage.GetBuffer();
	}

	void CamerRenderPackages::SeteDebugePackageOnSlot(Ref<ShaderDrawList>& list, uint32_t bindSlot)
	{
		RY_CORE_ASSERT(bindSlot < list->GetBindeUniform().size(), "Data Overflow!");
		list->GetBindeUniform()[bindSlot] = m_DebugePackage.GetBuffer();
	}

	void CamerRenderPackages::DrawPass()
	{
		UpdateBuffers();
		m_RenderTarget.DrawPilines();
		m_RenderTarget.DrawBufferList();

		m_RenderTarget.SortePilineAlphaList();
		m_RenderTarget.DrawAlphaPilines();

		m_RenderTarget.ClearPilines();
		m_RenderTarget.ClearAlphaPilines();
	}

	void CamerRenderPackages::DrawPass(int modes)
	{
		UpdateBuffers();
		m_RenderTarget.DrawPilines(modes);
		m_RenderTarget.DrawBufferList(modes);
		
		m_RenderTarget.SortePilineAlphaList();
		m_RenderTarget.DrawAlphaPilines(modes);

	}

	void CamerRenderPackages::DestoryBuffers()
	{	
		m_CamerPackage.DestroyPackege();
		m_DisplayPackage.DestroyPackege();
		m_DebugePackage.DestroyPackege();

		m_RenderTarget.ClearShaderDrawList();
		m_RenderTarget.ClearFrambuffer();
	}

	void CamerRenderPackages::UpdateBuffers()
	{
		if (m_CamerPackage.IsNotRady())
			Utils::UpdatePackegeBuffer(m_CamerPackage, s_DefaultBindSlotCamerPackage);

		if (m_DebugePackage.IsNotRady())
			Utils::UpdatePackegeBuffer(m_DebugePackage, s_DefaultBindSlotDebugePackage);

		if (m_DisplayPackage.IsNotRady())
			Utils::UpdatePackegeBuffer(m_DisplayPackage, s_DefaultBindSlotDisplayPackage);
	}


}
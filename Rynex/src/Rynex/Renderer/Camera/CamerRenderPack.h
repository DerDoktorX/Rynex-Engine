#pragma once
#include <Rynex/Renderer/Camera/CameraBufferLayouts.h>
#include <Rynex/Renderer/Rendering/RenderTarget.h>
#include <Rynex/Renderer/Objects/BufferDataPack.h>

namespace Rynex {
	struct ViewPassData;

	class CamerRenderPackages
	{
	public:
		using CamerPackage = UniformDataPackage<CameraData>;
		using DisplayPackage = UniformDataPackage<DisplayData>;
		using DebugCamerPackage = UniformDataPackage<glm::mat4>;
	public:
		CamerRenderPackages();
		CamerRenderPackages(const Ref<Framebuffer>& fb, const glm::ivec4& display, const glm::mat4& model, const glm::mat4& projection);
		CamerRenderPackages(const Ref<Framebuffer>& fb, const glm::mat4& model, const glm::mat4& projection);
		~CamerRenderPackages();

		RenderTarget& GetRenderTarget()  { return m_RenderTarget; }
		const RenderTarget& GetRenderTarget() const { return m_RenderTarget; }
		const Ref<Framebuffer>& GetFramebuffer() const { return m_RenderTarget.GetFramebuffer(); }
		
		ShaderDrawResource GetDrawPassOrAdd(const Ref<Shader>& shader) { return m_RenderTarget.GetDrawPassOrAdd(shader); }

		CamerPackage& GetCamerPackage() { UpdateBuffers();  return m_CamerPackage; }
		DisplayPackage& GetDisplayPackage() { UpdateBuffers();  return m_DisplayPackage; }
		DebugCamerPackage& GetDebugPackage() { UpdateBuffers(); return m_DebugePackage; }

		[[nodiscard]] glm::vec4 GetViewSpace() const { return m_DisplayPackage.GetData().ViewPortConf; }
		


		void SetFromViewPass(const ViewPassData& viewPass);
		void SetShadeRenderTarget(const RenderTarget& rendertarget) { m_RenderTarget = rendertarget; }
		void SetFrambuffer(const Ref<Framebuffer>& fb) { m_RenderTarget.SetFramebuffer(fb); }


		void SetCamerDater(const CameraData& cameraDater) { m_CamerPackage.SetData(cameraDater);  }
		void SetDisplayDater(const DisplayData& diplayDater) { m_DisplayPackage.SetData( diplayDater); }
		void SetDebugCamerDater(const glm::mat4& debugeDater) { m_DebugePackage.SetData(debugeDater); }


		void SetCamerDataModel(const glm::mat4& model);
		void SetCamerDataView(const glm::mat4& view);
		void SetCamerDataProjection(const glm::mat4& projection);
		void SetCamerDataViewProjection(const glm::mat4& view, const glm::mat4& projection);

		void SetDisplayDataModel(const glm::ivec4& display);
		

		void SetCamerPackageOnSlot(Ref<ShaderDrawList>& list, uint32_t bindSlot);
		void SeteDisplayPackageOnSlot(Ref<ShaderDrawList>& list, uint32_t bindSlot);
		void SeteDebugePackageOnSlot(Ref<ShaderDrawList>& list, uint32_t bindSlot);

		void DrawPass();
		void DrawPass(int modes);

		void DestoryBuffers();

		uint32_t GetPilineBaseCount() const { return m_RenderTarget.GetPilineBaseCount(); }
		uint32_t GetDrawListCount() const { return m_RenderTarget.GetDrawListCount(); }

	private:
		void UpdateBuffers();
	private:
		RenderTarget m_RenderTarget;

		CamerPackage m_CamerPackage;
		DisplayPackage m_DisplayPackage;
		DebugCamerPackage m_DebugePackage;
		

		constexpr static uint32_t s_DefaultBindSlotCamerPackage = 0u;
		constexpr static uint32_t s_DefaultBindSlotDebugePackage = 1u;
		constexpr static uint32_t s_DefaultBindSlotDisplayPackage = 2u;
		
	};
}

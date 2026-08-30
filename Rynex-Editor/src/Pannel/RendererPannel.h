#pragma once

#include <Rynex/Events/Event.h>
#include <Rynex/Events/KeyEvent.h>
#include <Rynex/Events/MouseEvent.h>
#include <Rynex/Core/TimeStep.h>
#include <Rynex/Renderer/Rendering/Renderer.h>

namespace Rynex {
	class EditorLayer;
	class Framebuffer;

	class RendererPannel
	{
	public:
		RendererPannel(const std::string& name = "Renderer");
		~RendererPannel();

		void OnAttache(EditorLayer* editorLayer);
		void OnDetache();

		void OnEvent(Event& e);
		void OpenWindow();
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtenPressedEvent& e);

		void OnUpdate(TimeStep ts);
		void OnImGuiRender();
		
	private:
		void ImGuiRenderTimeAverage();
		void ImGuiRenderTimeHigest();
		void ImGuiRenderTimeLowest();
		void ImGuiDrawRenderTimeMic(const std::string& name, int64_t renderUpdateTimeMicroSec);
		void ImGuiRenderStatePass(const std::string& name, StatusRenderPasses& pass);
		void ImGuiVSyncCheckBox();
		void ImGuiRenderTimeAutoRest();
		void ImGuiSelectFrameSubmitFunc();
		void ImGuiRenderMode();
		void UpdateAregeTime(double timePoint);
		void ImGuiRenderPiplines();
		void ImGuiRenderComputeInvoction();
		void ImGuiRenderPiplinesType(uint32_t id, const VectorViewPassStorage& vec, const std::string& name, int flags);
		void ImGuiRenderViewPass(uint32_t id, const ViewPassStorage& viewPass);
		void UpdateTimer();
		void ImGuiRenderSettings();
	private:
		struct Timers 
		{
			float FPS = 0.0f;
			float SecFrame = 0.0f;
			double Time = 0.0;
		};
		class SubmitFrame3DRendererStata
		{
		public:
			enum
			{
				None = 0,
				SubmitRenderIndrectDrawList = 1,
				SubmitRenderSingleIndrectDrawList = 2,
				SubmitRenderMeshDrawList = 3
			};
		};
		
	private:

		std::string m_Name = "";
		std::vector<float> m_FrameTimesVec;
		std::vector<Timers> m_Timers;

		EditorLayer* m_EditorLayer = nullptr;
		Ref<Framebuffer> m_MainFrameB = nullptr;
		Ref<PlatformTimer> m_FrameTimer;

		TimeStep m_CurentTS;
		TimeStep m_HigestTS[2];
		TimeStep m_LowestTS[2];
		Timers m_AveregeTime;


		int64_t m_FrameTimeMircoSecDisplay;
		int64_t m_FrameTimeMircoSec;
		int64_t m_SwapUpdateTimeMircoSec;
		int64_t m_ImGuiUpdateTimeMircoSec;
		int64_t m_LayersUpdateTimeMircoSec;
		int64_t m_VieportSceneUpdateTimeMircoSec;
		int64_t m_SceneRenderTime;
		int64_t m_SceneUpdateTime;
		int64_t m_SceneSubmit3DObjects;
		StatusRender m_StatusCopy;
		glm::ivec3  m_ComputeInvoction = { 8, 16, 1 };

		uint32_t m_FramePasstCount;
		uint32_t m_MemoryVRam;
		uint32_t m_VaildFrameTimeCount;
		uint32_t m_FirstFrameTimesIndex;
		uint32_t m_EndFrameTimesIndex;
		int m_RestFrameTimeExtrem;
		int m_SubmitFrame3DSceneFuncState;
		int m_RendererMode = 0;
		float m_FPS;
		float m_TimePast;
		float m_TimerPast;
		float m_FrameTimeGrafhShow;


		bool m_WindowOpen = true;
		bool m_FramTimerOpen = true;

	};
}


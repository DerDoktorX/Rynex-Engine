#include <rypch.h>
#include "RendererPannel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <EditorLayer.h>
#include <ImGuiObj/ImGuiObjects.h>

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Core/Application.h>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>
#include <Rynex/Renderer/Rendering/PiplineVec.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>
#include <Rynex/Renderer/RenderProxy/StaticeRenderProxys.h>


#include <magic_enum.hpp>
// #define IMGUI_BOOL_CHECK_BOX(value) { #value, &value }
#define IMGUI_BOOL_CHECK_BOX(value) ImGuiFlagsUser::CheckBoxBool{ RY_STRINGIFY_MOAKRO(#value), &value }


namespace Rynex {

	namespace Utils {

	}

	RendererPannel::RendererPannel(const std::string& name)
		: m_Name(name)
		, m_RendererMode(
			RenderMode::CallFace_None
			| RenderMode::Death_Buffer
			| RenderMode::A_Buffer
			| RenderMode::PrimitivReset
		)
		, m_TimerPast(1.0f)
		, m_LowestTS{ TimeStep(10000.0f), TimeStep(10000.0f) }
		, m_HigestTS{ TimeStep(-10000.0f), TimeStep(10000.0f) }
		, m_RestFrameTimeExtrem(10)
		, m_SubmitFrame3DSceneFuncState(SubmitFrame3DRendererStata::SubmitRenderIndrectDrawList)
		, m_SwapUpdateTimeMircoSec(1ull)
		, m_FrameTimeMircoSecDisplay(1ull)
		, m_FrameTimeMircoSec(1ull)
		, m_ImGuiUpdateTimeMircoSec(1ull)
		, m_LayersUpdateTimeMircoSec(1ull)
		, m_SceneRenderTime(1ull)
		, m_SceneUpdateTime(1ull)
		, m_SceneSubmit3DObjects(1ull)
		, m_FrameTimer(nullptr)
		, m_FramePasstCount(0ul)
		, m_TimePast(0.0f)
		, m_FrameTimeGrafhShow(0.0f)
		, m_VaildFrameTimeCount(100u)
	{
		m_FirstFrameTimesIndex = 0u;
		m_EndFrameTimesIndex = m_VaildFrameTimeCount - 1u;
		uint64_t reserveCount = m_TimerPast * 100;
		m_Timers.reserve(reserveCount);
		uint32_t frameTimeCount = m_VaildFrameTimeCount * 2u;
		m_FrameTimesVec.resize(frameTimeCount);
	}

	RendererPannel::~RendererPannel()
	{
	}

	void RendererPannel::OnAttache(EditorLayer* editorLayer)
	{
		m_FrameTimer = PlatformTimer::Create(&m_FrameTimeMircoSec);
		m_EditorLayer = editorLayer;
	}

	void RendererPannel::OnDetache()
	{
		m_EditorLayer = nullptr;
		m_StatusCopy.SecundaryPasses.clear();
	}

	void RendererPannel::OnEvent(Event& e)
	{

	}

	void RendererPannel::OpenWindow()
	{
		m_WindowOpen = true;
	}

	bool RendererPannel::OnKeyPressed(KeyPressedEvent& e)
	{
		return false;
	}

	bool RendererPannel::OnMousePressed(MouseButtenPressedEvent& e)
	{
		return false;
	}

	void RendererPannel::OnUpdate(TimeStep ts)
	{
		m_FrameTimer->Start();
		m_CurentTS = ts;
		m_TimePast += ts.GetMillsecounds();
		m_FramePasstCount++;
		double timePoint = m_CurentTS.GetAppRuningTime();
		int time = static_cast<int>(timePoint);
		int restAutoHigestLowestTime = -1;
		if(0 != m_RestFrameTimeExtrem)
			restAutoHigestLowestTime = time % (m_RestFrameTimeExtrem);

		static double lastFrameSecoundExe = 0.0;
		double timeDiffernz = timePoint - lastFrameSecoundExe;
		if (0 == restAutoHigestLowestTime && timeDiffernz >= m_RestFrameTimeExtrem)
		{
			m_HigestTS[1] = m_HigestTS[0];
			m_LowestTS[1] = m_LowestTS[0];
			m_HigestTS[0] = TimeStep::CreateMin();
			m_LowestTS[0] = TimeStep::CreateMax();
			lastFrameSecoundExe = timePoint;
		}

		if (m_TimePast >= 1000.0f)
		{
			m_AveregeTime.SecFrame = m_TimePast / static_cast<float>(m_FramePasstCount);
			m_FPS = static_cast<float>(m_FramePasstCount);
			m_FramePasstCount = 0ul;
			m_TimePast = 0.0f;

			UpdateTimer();
		}

		

		m_HigestTS[0] = m_CurentTS.GetMillsecounds() > m_HigestTS[0].GetMillsecounds() ? m_CurentTS : m_HigestTS[0];
		m_LowestTS[0] = m_CurentTS.GetMillsecounds() < m_LowestTS[0].GetMillsecounds() ? m_CurentTS : m_LowestTS[0];
		
#if 0
		constexpr float msTime = 1000.0f;
		constexpr float showGrapfhTimeSec = 1.0f;
		constexpr float showGrapfhTimeMs = msTime * showGrapfhTimeSec;
		while (showGrapfhTimeMs <= m_FrameTimeGrafhShow)
		{
			m_FrameTimeGrafhShow -= m_FrameTimesVec.front();
			m_FrameTimesVec.erase(m_FrameTimesVec.begin());
		}

		float deltatMsTime = ts.GetMillsecounds();
		m_FrameTimeGrafhShow += deltatMsTime;
		m_FrameTimesVec.push_back(deltatMsTime);
		m_FirstFrameTimesIndex = 0;
		m_EndFrameTimesIndex = m_FrameTimesVec.size();
#else
		
		float deltatMsTime = ts.GetMillsecounds();
		m_FrameTimesVec.at(m_EndFrameTimesIndex) = deltatMsTime;
		
		if (m_FirstFrameTimesIndex == m_VaildFrameTimeCount)
		{	
			float* srcPtr = m_FrameTimesVec.data() + m_FirstFrameTimesIndex;
			float* dstPtr = m_FrameTimesVec.data();

			uint32_t bytesSize = sizeof(float) * m_VaildFrameTimeCount;
			std::memcpy(dstPtr, srcPtr, bytesSize);
			m_FirstFrameTimesIndex = 0u;
			m_EndFrameTimesIndex = m_VaildFrameTimeCount - 1u;
		}
		m_FirstFrameTimesIndex++;
		m_EndFrameTimesIndex++;
#endif
		

		
	}

	void RendererPannel::OnImGuiRender()
	{
		
		if(m_WindowOpen)
		{
			ImGui::Begin(m_Name.c_str(), &m_WindowOpen, ImGuiWindowFlags_None);
			ImGui::Text("Renderer:");
			
			
			ImGuiRenderTimeAverage();
			ImGuiRenderTimeAutoRest();
			ImGuiRenderTimeLowest();
			ImGuiRenderTimeHigest();
			ImGuiDrawRenderTimeMic("Swap", m_SwapUpdateTimeMircoSec);
			ImGuiDrawRenderTimeMic("ImGui", m_ImGuiUpdateTimeMircoSec);
			// ImGuiDrawRenderTimeMic("Layers", m_LayersUpdateTimeMircoSec);
			// ImGuiDrawRenderTimeMic("ViewPort-Update-Scene", m_SceneUpdateTime);
			ImGuiDrawRenderTimeMic("ViewPort-Render-Scene", m_SceneRenderTime);
			ImGuiDrawRenderTimeMic("Render-Scene", m_SceneRenderTime);
			ImGuiDrawRenderTimeMic("Scene-3D-Submit", m_SceneSubmit3DObjects);
			ImGuiRenderStatePass("Main-Pass", m_StatusCopy.MainPass);
			uint32_t i = 0;
			for(StatusRenderPasses& pass : m_StatusCopy.SecundaryPasses)
			{
				std::string name = "Pass: " + std::to_string(i);
				ImGuiRenderStatePass(name, pass);
				i++;
			}

			ImGuiSelectFrameSubmitFunc();
			
			ImGuiRenderMode();

			ImGuiVSyncCheckBox();

			ImGuiRenderComputeInvoction();

			if (ViewPortPannel* viewPortPannel = m_EditorLayer->GetViewPortPannel())
			{
				
				bool* staicresultion = viewPortPannel->GetStaicResultionState();
				if(ImGui::Checkbox("Staitic Resultion", staicresultion))
					viewPortPannel->OnStaicResultion();
				glm::ivec2* size = viewPortPannel->GetStaicResultionSizePtr();
				if(!*staicresultion)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
					if (ImGui::DragInt2("Staice Size", glm::value_ptr(*size), 1.0f, 1, 8000))
						viewPortPannel->OnStaicResultion();

				if (!*staicresultion)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				int i = 0;
				for(const auto& viewPort : viewPortPannel->GetViewPorts())
				{
					ImGui::Text("Viewport %i", i);
					
					glm::ivec2 windowSize = viewPort->GetWindowSize();
					ImGui::InputInt2("Window Size", glm::value_ptr(windowSize));
					glm::ivec2 frameSize = viewPort->GetRenderSize();
					ImGui::InputInt2("Renderer Size", glm::value_ptr(frameSize));

					i++;
				}				
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
				
			}

#if RY_RENDER_3D
			{
				ImGui::Text("Renderer 3D:");
				Renderer3D::Statistics renderer = Renderer3D::GetStats();
				ImGui::Text("DrawCalls: %i", renderer.DrawCalls);
				ImGui::Text("Resterrizer (TrinagleCount): %i", renderer.RestericeCount);
				uint8_t index = 0;
				for(auto& drawPass : renderer.DrawTime)
				{
					ImGui::Text("DrawTime[%i]:  %.2lf mils", index, (double)drawPass);
					index++;
				}
				Renderer3D::ResetStats();
			}
			

			{
				float factor = 0.0;
				float uints = 0.0;
				bool active = false;
				Renderer3D::GetShadowMapsPolygonOffset(&factor, &uints, &active);

				ImGui::Text("Shadow Map Offset");
				if(ImGui::Checkbox("GPU Bias Seport:", &active))
					Renderer3D::SetShadowMapsPolygonOffset(factor, uints, active);
				
				
				{
					ImGuiIO& io = ImGui::GetIO();
					ImFont* boldFont = io.Fonts->Fonts[0];

					ImGui::PushID("FactorID");
					ImGui::Columns(2);
					{
						ImGui::SetColumnWidth(0, 80);
						ImGui::Text("Factor:");
						ImGui::NextColumn();
					}

					ImVec2 butenSize;
					{
						ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
						float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
						butenSize = { lineHeigth + 3.0f, lineHeigth };
					}

					{
						if (ImGui::DragFloat("##FactorInput", &factor, 0.01, -5.0, 5.0))
							Renderer3D::SetShadowMapsPolygonOffset(factor, uints, active);
					}

					{
						ImGui::SameLine();
						if (ImGui::Button("+", butenSize))
							Renderer3D::SetShadowMapsPolygonOffset(factor + 0.001, uints, active);
						ImGui::PopItemWidth();
					}

					{
						ImGui::SameLine();
						if (ImGui::Button("-", butenSize))
							Renderer3D::SetShadowMapsPolygonOffset(factor - 0.001, uints, active);
						// ImGui::PopItemWidth();
					}
					
					ImGui::Columns(1);
					ImGui::PopID();
					

				}
				{
					ImGuiIO& io = ImGui::GetIO();
					ImFont* boldFont = io.Fonts->Fonts[0];
					
					ImGui::PushID("UintsID");
					ImGui::Columns(2);
					{
						ImGui::SetColumnWidth(0, 80);
						ImGui::Text("Uints:");
						ImGui::NextColumn();
					}

					ImVec2 butenSize;
					{
						ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
						float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
						butenSize = { lineHeigth + 3.0f, lineHeigth };
					}
					{
						if (ImGui::DragFloat("##UintsInput", &uints, 0.01, -5.0, 5.0))
							Renderer3D::SetShadowMapsPolygonOffset(factor, uints, active);
					}

					{
						ImGui::SameLine();
						if (ImGui::Button("+", butenSize))
							Renderer3D::SetShadowMapsPolygonOffset(factor, uints + 0.001, active);
						ImGui::PopItemWidth();
					}

					{
						ImGui::SameLine();
						if (ImGui::Button("-", butenSize))
							Renderer3D::SetShadowMapsPolygonOffset(factor, uints - 0.001, active);
						// ImGui::PopItemWidth();
					}

					ImGui::PopItemWidth();
					ImGui::Columns(1);
					ImGui::PopID();
					
				}
				
			}
			{
				ImGui::Text("Renderer 2D:");
				Renderer2D::Statistics stats = Renderer2D::GetQuadeStats();
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quad   : %d", stats.QuadCount);
				ImGui::Text("Vertex : %d", stats.GetTotalVertexCount());
				ImGui::Text("Indexs : %d", stats.GetTotalIndexCount());
			}
#endif
			
			ImGuiRenderPiplines();

			ImGui::End();
		}
		
		if (m_FramTimerOpen)
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_None;
			
			ImGui::Begin("Frame Timer", &m_FramTimerOpen, flags);
			constexpr float maxGraph = (1.0f / 50.0f) * 1000.0f;

				

			float textLineHeigth = 0.0f;
			ImVec2 region = ImGui::GetContentRegionAvail();
			float regionSpaceX = region.x;
			region.x *= 6.0f / 8.0f;
			float textSpaceX = regionSpaceX - region.x;
			glm::vec4 color = glm::vec4{ 0.05f, 0.95f, 0.01f, 1.0f };
			ImVec2 curserPosBegin = ImGui::GetCursorScreenPos();
			textLineHeigth = ImGuiPerfomeceRendering::DrawVerticleBarVec("Frame Time", region, textSpaceX, m_FrameTimesVec, m_FirstFrameTimesIndex, m_EndFrameTimesIndex ,color, textLineHeigth);
			ImGui::SetCursorScreenPos(curserPosBegin);
			color.g = 0.0f;
			color.b = 1.0f;

			ImGui::End();
		}
		
		Renderer::ClearState();
	}


	

	void RendererPannel::ImGuiRenderTimeAverage()
	{
		ImGui::PushID("##ImGuiRenderTimeAverage");

		ImGui::Text("Update Time: %.2f milsec", m_AveregeTime.SecFrame);

		ImGui::PopID();
	}

	void RendererPannel::ImGuiRenderTimeHigest()
	{
		float fps = m_HigestTS[1].GetFPS();
		float sec = m_HigestTS[1].GetMillsecounds();
		ImGui::PushID("##ImGuiRenderTimeHigest");
#if 0
		ImGui::Text("Higest: Time(%.1f) / FPS(%.1f)", sec, fps);
#else
		ImGui::Text("Higest: Time(%.1f)", sec, fps);
#endif
		ImGui::PopID();
	}

	void RendererPannel::ImGuiRenderTimeLowest()
	{
		float fps = m_LowestTS[1].GetFPS();
		float sec = m_LowestTS[1].GetMillsecounds();

		ImGui::PushID("##ImGuiRenderTimeLowest");
#if 0
		ImGui::Text("Lowest: Time(%.1f) / FPS(%.1f)", sec, fps);
#else
		ImGui::Text("Higest: Time(%.1f)", sec, fps);
#endif

		ImGui::PopID();

	}

	void RendererPannel::ImGuiDrawRenderTimeMic(const std::string& name, int64_t renderUpdateTimeMicroSec)
	{

		std::string idName = "##ImGuiImGuiDrawRenderTimeMic" + name;
		const char* idNameStr = idName.c_str();
		ImGui::PushID(idNameStr);
		int renderUpdateTimeMicroSecI = static_cast<int>(renderUpdateTimeMicroSec);
		int frameTimeMicroSec = static_cast<int>(m_FrameTimeMircoSecDisplay);

		double renderUpdateTimeMicroSecD = static_cast<double>(renderUpdateTimeMicroSec);
		double frameTimeMicroSecD = static_cast<double>(m_FrameTimeMircoSecDisplay);


		double prozentFormFrameTime = (renderUpdateTimeMicroSecD / frameTimeMicroSecD) * 100.0;

		const char* nameStr = name.c_str();
		ImGui::Text("%s: Time(%lli) microsec / Prozent from Frame Time: %.3lf %%", nameStr, renderUpdateTimeMicroSec, prozentFormFrameTime);

		ImGui::PopID();
	}

	void RendererPannel::ImGuiRenderStatePass(const std::string& name, StatusRenderPasses& pass)
	{

	}

	void RendererPannel::ImGuiVSyncCheckBox()
	{
		Application& app = Application::Get();
		Window& window = app.GetWindow();

		bool vsSync = window.IsVSync();

		if (ImGui::Checkbox("VSync Status", &vsSync))
			window.SetVSync(vsSync);
	}

	void RendererPannel::ImGuiRenderTimeAutoRest()
	{
		if (ImGui::DragInt("auto reset sec", &m_RestFrameTimeExtrem, 0.5f, 1, MAXINT, "%i"))
		{
			RY_CORE_ASSERT(m_RestFrameTimeExtrem > 0);
		}

	}

	void RendererPannel::ImGuiSelectFrameSubmitFunc()
	{
		const Ref<Scene>& scene = m_EditorLayer->GetAktivScene();
		if (ImGui::RadioButton("SubmitRenderIndrectDrawList", &m_SubmitFrame3DSceneFuncState, SubmitFrame3DRendererStata::SubmitRenderIndrectDrawList))
			scene->SetFuncSubmit3DSceneDrawListToFrame(Renderer3D::SubmitRenderIndrectDrawList);

		if (ImGui::RadioButton("SubmitRenderSingleIndrectDrawList", &m_SubmitFrame3DSceneFuncState, SubmitFrame3DRendererStata::SubmitRenderSingleIndrectDrawList))
			scene->SetFuncSubmit3DSceneDrawListToFrame(Renderer3D::SubmitRenderSingleIndrectDrawList);

		if (ImGui::RadioButton("SubmitRenderMeshDrawList", &m_SubmitFrame3DSceneFuncState, SubmitFrame3DRendererStata::SubmitRenderMeshDrawList))
			scene->SetFuncSubmit3DSceneDrawListToFrame(Renderer3D::SubmitRenderMeshDrawList);
	}

	void RendererPannel::ImGuiRenderMode()
	{
		bool wireFrame = BIT_EQUAL(m_RendererMode, RenderMode::WireFrame),
			a_Buffer = BIT_EQUAL(m_RendererMode, RenderMode::A_Buffer),
			gamma = BIT_EQUAL(m_RendererMode, RenderMode::Gamma),
			death_Buffer = BIT_EQUAL(m_RendererMode, RenderMode::Death_Buffer),
			primitivReset = BIT_EQUAL(m_RendererMode, RenderMode::PrimitivReset),
			callFace_Nono = BIT_EQUAL(m_RendererMode, RenderMode::CallFace_None),
			callFace_Front = BIT_EQUAL(m_RendererMode, RenderMode::CallFace_Front),
			callFace_Back = BIT_EQUAL(m_RendererMode, RenderMode::CallFace_Back),
			callFace_FrontBack = BIT_EQUAL(m_RendererMode, RenderMode::CallFace_FrontBack);


		if (ImGui::Checkbox("Wirframe mode:", &wireFrame))
		{
			m_RendererMode = BIT_SET_ON(wireFrame, RenderMode::WireFrame, m_RendererMode);
		}
		if (ImGui::Checkbox("A-Buffer mode:", &a_Buffer))
		{
			m_RendererMode = BIT_SET_ON(a_Buffer, RenderMode::A_Buffer, m_RendererMode);
		}
		if (ImGui::Checkbox("Death-Buffer mode:", &death_Buffer))
		{
			m_RendererMode = BIT_SET_ON(death_Buffer, RenderMode::Death_Buffer, m_RendererMode);
		}
		if (ImGui::Checkbox("Gamma mode:", &gamma))
		{
			m_RendererMode = BIT_SET_ON(gamma, RenderMode::Gamma, m_RendererMode);
		}
		if (ImGui::Checkbox("Primitiv Reset mode:", &primitivReset))
		{
			m_RendererMode = BIT_SET_ON(primitivReset, RenderMode::PrimitivReset, m_RendererMode);
		}


		if (ImGui::RadioButton("None", callFace_Nono))
		{
			m_RendererMode = BIT_SET_ON(1, RenderMode::CallFace_None, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_Front, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_Back, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_FrontBack, m_RendererMode);
		}
		if (ImGui::RadioButton("Front", callFace_Front))
		{
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_None, m_RendererMode);
			m_RendererMode = BIT_SET_ON(1, RenderMode::CallFace_Front, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_Back, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_FrontBack, m_RendererMode);
		}
		if (ImGui::RadioButton("Back", callFace_Back))
		{
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_None, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_Front, m_RendererMode);
			m_RendererMode = BIT_SET_ON(1, RenderMode::CallFace_Back, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_FrontBack, m_RendererMode);
		}
		if (ImGui::RadioButton("Front Back", callFace_FrontBack))
		{
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_None, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_Front, m_RendererMode);
			m_RendererMode = BIT_SET_ON(0, RenderMode::CallFace_Back, m_RendererMode);
			m_RendererMode = BIT_SET_ON(1, RenderMode::CallFace_FrontBack, m_RendererMode);
		}

		bool gammaBackGround = Renderer::GetBackgroundGamma();
		if (ImGui::Checkbox("Background Gamma Corection:", &gammaBackGround))
			Renderer::SetBackgroundGamma(gammaBackGround);

		float gammaValue = Renderer::GetGammaValue();
		if (ImGui::DragFloat("Gamma Value:", &gammaValue, 0.1, 0.001f, 100.0f))
			Renderer::SetGammaValue(gammaValue);


		Renderer::SetSceneMode(m_RendererMode);

		ImGuiRenderSettings();
	}

	void RendererPannel::UpdateAregeTime(double timePoint)
	{
		float fps = m_CurentTS.GetFPS();
		
		uint32_t count = 0;
		uint32_t indexDelate = 0;
		
		m_AveregeTime = Timers{};
		timePoint -= m_TimerPast;
		for (Timers t : m_Timers)
		{
		
			m_AveregeTime.FPS += t.FPS;
			m_AveregeTime.SecFrame += t.SecFrame;
			
			if(t.Time <= timePoint)
				indexDelate = count;
			count++;
			
		}
		m_AveregeTime.FPS /= count;
		m_AveregeTime.SecFrame /= count;

		m_Timers.erase(m_Timers.begin(),m_Timers.begin() + indexDelate);

		
	}

	void RendererPannel::ImGuiRenderPiplines()
	{
	}

	void RendererPannel::ImGuiRenderComputeInvoction()
	{
		if(ViewPortPannel* viewPortPannel = m_EditorLayer->GetViewPortPannel())
		{
			ImGui::Text("ViewPort Compute");

			if (ImGui::InputInt3("Invoction Grupp", glm::value_ptr(m_ComputeInvoction)))
			{
				viewPortPannel->SetComputeInvocation(static_cast<glm::vec3>(m_ComputeInvoction));
			}
		}
	}

	void RendererPannel::ImGuiRenderPiplinesType(uint32_t id, const VectorViewPassStorage& vec, const std::string& name, int flags)
	{
		if (vec.empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
			flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		}

		std::string idName = name + std::to_string(id);
		const char* idNameStr = idName.c_str();
		ImGui::PushID(idNameStr);

		const char* nameStr = name.c_str();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		bool opende = ImGui::TreeNodeEx(&id, flags, nameStr);
		ImGui::PopStyleVar();

		if(opende)
		{
			for (const ViewPassStorage& viewPass : vec)
			{
				ImGuiRenderViewPass(id, viewPass);
			}
			if(!vec.empty())
				ImGui::TreePop();
		}
		ImGui::PopID();

	}

	void RendererPannel::ImGuiRenderViewPass(uint32_t id, const ViewPassStorage& viewPass)
	{
		int slot = viewPass.Slot;
		std::string name = "Slot: " + std::to_string(slot);

		std::string idName = name + std::to_string(id);
		const char* idNameStr = idName.c_str();
		
		ImGui::PushID(idNameStr);
		std::string_view viewStringViewPassType = magic_enum::enum_name(viewPass.Type);
		const char* nameStr = name.c_str();
		const char* strViewPassType = viewStringViewPassType.data();

		ImGui::Text("%s  (Type: %s)", nameStr, strViewPassType);

		

		ImGui::PopID();

	}

	void RendererPannel::UpdateTimer()
	{
		Application& app = Application::Get();
		Window& window = app.GetWindow();
		m_SwapUpdateTimeMircoSec = window.GetRenderTime();
		m_ImGuiUpdateTimeMircoSec = app.GetImGuiTime();
		m_LayersUpdateTimeMircoSec = app.GetLayerTime();
		m_FrameTimeMircoSecDisplay = m_FrameTimeMircoSec;
		m_SceneUpdateTime = m_EditorLayer->GetTimerUpdateViewPort();
		m_SceneRenderTime = m_EditorLayer->GetTimerRenderViewPort();
		m_SceneSubmit3DObjects = m_EditorLayer->GetTimerScene3DSubmit();
		m_StatusCopy = Renderer::GetStatus();
	}

	void RendererPannel::ImGuiRenderSettings()
	{
		RenderSettings& setings = Renderer::GetRenderSettings();
		bool& testDrawCall = StaticeRenderProxys::GetTestDrawCall();
		bool& renderProxyInstaecing = StaticeRenderProxys::GetRenderProxyInstaecing();
		bool& useDyamincDatatStruct = StaticeRenderProxys::GetUseDyamincDatatStruct();

#ifdef RY_ALBEDO_TEXTURE_ARRAY
		bool& testTexturArray = StaticeRenderProxys::GetTestTexturArray();
#endif

		ImGuiFlagsUser::CheckBoxBoolListUI({ 
			IMGUI_BOOL_CHECK_BOX(setings.useSceneRenderModeInMainPass)
			, IMGUI_BOOL_CHECK_BOX(setings.drawPiplinesFromRenderTarget)
			, IMGUI_BOOL_CHECK_BOX(setings.sortBeforDrawFromRenderTarget)
			, IMGUI_BOOL_CHECK_BOX(setings.drawShaderDrawListFromRenderTarget)
			, IMGUI_BOOL_CHECK_BOX(setings.drawRenderProxy)
			, IMGUI_BOOL_CHECK_BOX(setings.drawPiplinesFromRenderPass)
			, IMGUI_BOOL_CHECK_BOX(setings.submiteSceneEntityTo3DRender)
			, IMGUI_BOOL_CHECK_BOX(testDrawCall)
			, IMGUI_BOOL_CHECK_BOX(renderProxyInstaecing)
			, IMGUI_BOOL_CHECK_BOX(useDyamincDatatStruct)

#ifdef RY_ALBEDO_TEXTURE_ARRAY
			, IMGUI_BOOL_CHECK_BOX(testTexturArray)
#endif
		});
		if (ImGui::Button("Clear Batches RenderProxy!"))
			Renderer3D::ClearBatchesFromRenderProxy();
	}

}
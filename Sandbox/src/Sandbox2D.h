#pragma once
#include <Rynex.h>


class Sandbox2D : public Rynex::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;
	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Rynex::TimeStep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Rynex::Event& e) override;

	bool OnResize(Rynex::WindowResizeEvent& e);
	bool OnMouseMove(Rynex::MouseMoveEvent& e);
	bool OnWindowMove(Rynex::WindowMovedEvent& e);
	bool OnFocuse(Rynex::WindowFocuseEvent& e);
	bool OnFocuseLost(Rynex::WindowLostFocuseEvent& e);
	bool OnCurserEnter(Rynex::WindowCurserEnterEvent& e);
	bool OnCurserLeave(Rynex::WindowCurserLeaveEvent& e);
private:

	void ImGuiViewPortResize(const glm::uvec2& vPSize);
	void ImGuiViewPortRender();
private:
	Rynex::OrthograficCameraController		m_CameraController;

	Rynex::Ref<Rynex::Shader>				m_BlueShader;
	Rynex::Ref<Rynex::VertexArray>			m_SquareVA;
	Rynex::Ref<Rynex::Texture>				m_ChekbordTex;

	Rynex::Ref<Rynex::Framebuffer>			m_Framebuffer;

	Rynex::Ref<Rynex::Shader>				m_FullScreenShader;
	Rynex::Ref<Rynex::VertexArray>			m_FullScreenQuade;

	Rynex::Ref<Rynex::Scene>				m_AktiveScene;
	Rynex::Ref<Rynex::Project>				m_Project;
	Rynex::Ref<Rynex::RuntimeAssetManager>	m_AssetManger;

	bool m_Hovered = false;
	bool m_Resized = false;
	bool m_Focuse = false;
	bool m_FirstFrame = true;


	glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
	glm::uvec2 m_ViewPortSize = { 0ul, 0ul };
	glm::uvec2 m_WindowPos = { 0ul, 0ul };
	glm::vec2 m_MousePixelPos = { -1.5f, -1.5f };
	glm::vec2 m_MousePos = { -1.5f, -1.5f };
	glm::vec2 m_WindowBounds[2];
};


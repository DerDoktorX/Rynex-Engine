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
	

private:
	Rynex::OrthograficCameraController		m_CameraController;

	Rynex::Ref<Rynex::Shader>				m_BlueShader;
	Rynex::Ref<Rynex::VertexArray>			m_SquareVA;
	Rynex::Ref<Rynex::Texture2D>			m_ChekbordTex;
	Rynex::Ref<Rynex::Framebuffer>			m_Framebuffer;

	Rynex::Ref<Rynex::Scene>				m_AktiveScene;
	Rynex::Ref<Rynex::Project>				m_Project;
	Rynex::Ref<Rynex::RuntimeAssetManager>	m_AssetManger;

	float m_ChekebordRotaion;
	glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
};


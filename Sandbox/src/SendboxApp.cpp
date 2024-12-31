#define APP_EXAMPLE 0
#if APP_EXAMPLE


#include "Platform/OpenGL/OpenGLShader.h"
#include <imgui/imgui.h">
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#endif
#include <rypch.h>
#include "Sandbox2D.h"
#include <Rynex/Core/EntryPoint.h>

#define DEBUGE_APLICATION_SPEED


#if APP_EXAMPLE

class ExampleLayer : public Rynex::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
		, m_CameraController((1280.0f / 720.0f), true)
		, m_MoveSpeed(1.f)
		, m_EnableReset(false)
		, m_SquerPostion(0.0f)
	{
		{
			{
				m_VertexArray = Rynex::VertexArray::Create();

				float vertices[] = {
					-0.5f, -0.5f,		/*0.2f, 0.3f, 0.8f,*/		0.0f, 0.0f,
					 0.5f, -0.5f,		/*0.2f, 0.3f, 0.8f,*/		1.0f, 0.0f,
					 0.5f,  0.5f,		/*0.2f, 0.3f, 0.8f,*/		1.0f, 1.0f,
					-0.5f,  0.5f,		/*0.2f, 0.3f, 0.8f,*/		0.0f, 1.0f
				};

				Rynex::Ref<Rynex::VertexBuffer> vertexBuffer;
				vertexBuffer = Rynex::VertexBuffer::Create(vertices, sizeof(vertices));

				Rynex::BufferLayout layout = {
					{ Rynex::ShaderDataType::Float2, "a_Postion" },
					//{Rynex::ShaderDataType::Float3, "a_Color"},
					{ Rynex::ShaderDataType::Float2, "a_TexurCoord" }
				};
				vertexBuffer->SetLayout(layout);
				m_VertexArray->AddVertexBuffer(vertexBuffer);

				uint32_t indices[] = {
					0, 1, 2,
					0, 2, 3
				};
				Rynex::Ref<Rynex::IndexBuffer>indexBuffer;
				indexBuffer = Rynex::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
				m_VertexArray->SetIndexBuffer(indexBuffer);

				{
					m_Shader = Rynex::Shader::Create("../Rynex-Editor/Editor-Assets/shaders/SrcShader.glsl");
				}
			

				{
					//std::string textureFragmentSrc_, textureVertexSrc_;
					//File::ReadFile("../Rynex-Editor/shaders/Texture.glsl", textureFragmentSrc_);

					auto textureShader = m_ShaderLibary.Load("../Rynex-Editor/Editor-Assets/shaders/Texture.glsl");

					m_Texture = Rynex::Texture::Create("../Rynex-Editor/Editor-Assets/textures/Checkerboard.png");
					m_TrasperentTestTexture = Rynex::Texture::Create("../Rynex-Editor/Editor-Assets/textures/testBlending.png");

					std::dynamic_pointer_cast<Rynex::OpenGLShader>(textureShader)->Bind();
					std::dynamic_pointer_cast<Rynex::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

				}
			}
			{

				///////////////////////////////////////////////////////////////////////////////////////////////////////
				///////////////////// Blueshader //////////////////////////////////////////////////////////////////////
				///////////////////////////////////////////////////////////////////////////////////////////////////////

				m_SquareVA =Rynex::VertexArray::Create();

				float squareVertices[] = {
					-0.5f,  0.5f,
					 0.5f,  0.5f,
					 0.5f, -0.5f,
					-0.5f, -0.5f
				};

				Rynex::Ref<Rynex::VertexBuffer> squareVB;
				squareVB = Rynex::VertexBuffer::Create(squareVertices, sizeof(squareVertices));

				squareVB->SetLayout({
					{Rynex::ShaderDataType::Float2, "a_Postion"}
				});

				m_SquareVA->AddVertexBuffer(squareVB);

				uint32_t squareIndices[] = {
					0, 1, 2, //3,
					0, 2, 3
				};
				Rynex::Ref<Rynex::IndexBuffer> squareIB;
				squareIB = Rynex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
				m_SquareVA->SetIndexBuffer(squareIB);


				m_BlueShader = Rynex::Shader::Create("../Rynex-Editor/Editor-Assets/shaders/BlueShaderSrc.glsl");
				
			}
		}
	}

	void OnUpdate(Rynex::TimeStep ts) override
	{
		#ifdef DEBUGE_APLICATION_SPEED
		RY_TRACE("Delta time: {0}s ({1})ms\t\t{2} FPS", ts.GetSecounds(), ts.GetMillsecounds(), 1/ts);
		#endif

		m_CameraController.OnUpdate(ts);
		Rynex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Rynex::RenderCommand::Clear();
		////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////

		Rynex::Renderer::BeginScene(m_CameraController.GetCamera());

		
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		glm::vec4 redColor(0.8f, 0.2f, 0.3f, 1.0f);
		glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		std::dynamic_pointer_cast<Rynex::OpenGLShader>(m_BlueShader)->Bind();
		std::dynamic_pointer_cast<Rynex::OpenGLShader>(m_BlueShader)->UploadUniformFloat4("u_Color", m_SquareColor);

		for (int i = 0; i < 100; i++)
		{
			//glm::vec3 pos(i == 1 ? m_SquerPostion.x :i%10 * 0.11f, i == 1 ? m_SquerPostion.y : i/10 * 0.11f , 0.f);
			glm::vec3 pos(i == 0 ? m_SquerPostion.x : i % 10 * 0.11f, i == 0 ? m_SquerPostion.y : i / 10 * 0.11f, 0.1f);
			glm::mat4 tranform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Rynex::Renderer::Submit(m_BlueShader, m_SquareVA, tranform);
		}

		auto textureShader = m_ShaderLibary.Get("Texture");

		m_Texture->Bind();
		Rynex::Renderer::Submit(textureShader, m_VertexArray, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.f, -0.1f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_TrasperentTestTexture->Bind();
		Rynex::Renderer::Submit(textureShader, m_VertexArray, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		Rynex::Renderer::EndScene();
	}

	void OnEvent(Rynex::Event& e) override
	{
		m_CameraController.OnEnvent(e);

		if (e.GetEventType() == Rynex::EventType::WindowResize)
		{
			auto& re = (Rynex::WindowResizeEvent&)e;

			
		}
	}

	void KeyControllsAndOpsit( float&value, int keyAdd,int keySub, Rynex::TimeStep& ts) const
	{
		if (Rynex::Input::IsKeyPressed(keyAdd)) {
			value += m_MoveSpeed * ts;
		}
		else if (Rynex::Input::IsKeyPressed(keySub)) {
			value -= m_MoveSpeed * ts;
		}
	}


	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
private:
	Rynex::ShaderLibary					m_ShaderLibary;
	Rynex::Ref<Rynex::Shader>			m_Shader;
	Rynex::Ref<Rynex::VertexArray>		m_VertexArray;

	Rynex::Ref<Rynex::Shader>			m_BlueShader/*, m_TextureShader*/;
	Rynex::Ref<Rynex::VertexArray>		m_SquareVA;

	Rynex::Ref<Rynex::Texture>		m_Texture, m_TrasperentTestTexture;

	glm::vec3 m_SquerPostion;

	
	Rynex::OrthograficCameraController m_CameraController;

	bool m_EnableReset;
	float m_MoveSpeed;

	glm::vec4 m_SquareColor = { 0.2f,0.3f,0.8f, 1.0f };
};

#endif

class Sandbox : public Rynex::Application
{
public:
	Sandbox(const Rynex::ApplicationSpecification& spec)
		: Rynex::Application(spec)
	{
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}
};

Rynex::Application* Rynex::CreateApplication(Rynex::ApplicationCommandLineArgs args)
{	
	Rynex::ApplicationSpecification spec;
	spec.Name = "Rynex-SandBox";
	spec.CommandLineArgs = args;

	Project::New();
	return new Sandbox(spec);
}
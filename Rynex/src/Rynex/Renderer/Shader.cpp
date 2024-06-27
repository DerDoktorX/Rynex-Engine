#include "rypch.h"
#include "Shader.h"

#include "Rynex/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Rynex {
	
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Cuurently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(filepath);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Cuurently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	

	////////////////////////////////////////////////////////////////////////////
	//////// ShaderLibary //////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////

	void ShaderLibary::Add(const std::string& name,const Ref<Shader>& shader)
	{
		RY_CORE_ASSERT(!Exist(name), "Shader olrady exist");
		m_Shaders[name] = shader;
	}

	void ShaderLibary::Add(const Ref<Shader>& shader)
	{
		auto name = shader->GetName();;
		Add(name, shader);
	}

	

	Ref<Shader> ShaderLibary::Load(const std::string& filepath)
	{
		auto& shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	

	Ref<Shader> ShaderLibary::Load(const std::string& name, const std::string& filepath)
	{
		auto& shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibary::Get(const std::string& name)
	{
		RY_CORE_ASSERT(Exist(name), "Shader not exist!");
		return m_Shaders[name];
	}

	bool ShaderLibary::Exist(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}
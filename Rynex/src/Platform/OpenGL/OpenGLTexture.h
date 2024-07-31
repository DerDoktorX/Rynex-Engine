#pragma once

#include "Rynex/Renderer/API/Texture.h"

#include <glad/glad.h>

namespace Rynex {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification);
		OpenGLTexture2D(uint32_t withe, uint32_t height);
		OpenGLTexture2D(const std::string& path);
		OpenGLTexture2D(void* data, int withe, int heigth, int chanels);
		OpenGLTexture2D(const TextureSpecification& spec, void* data);
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeigth() const override { return m_Heigth; }
		virtual uint32_t GetRenderID() const override { return m_RendererID; }

		virtual const std::string& GetPath() const override { return m_Path; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other)const override 
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		};

		//static AssetType GetStaticType() { return AssetType::Texture2D; }
		//virtual AssetType GetType() const { return GetStaticType(); }
		//virtual AssetHandle GetHandle() const { return m_Handle; };
	private:
		TextureSpecification m_Specification;

		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Heigth;
		uint32_t m_RendererID;
		GLenum m_InternalFormate, m_DataFormate;
	};

}


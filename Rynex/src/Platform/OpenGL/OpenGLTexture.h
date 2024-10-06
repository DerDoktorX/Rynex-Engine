#pragma once

#include "Rynex/Renderer/API/Texture.h"

#include <glad/glad.h>

namespace Rynex {


	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const TextureSpecification& specification);
		
		OpenGLTexture(uint32_t withe, uint32_t height);
		OpenGLTexture(const std::string& path);
		OpenGLTexture(void* data, int withe, int heigth, int chanels);
		OpenGLTexture(const TextureSpecification& spec, void* data, uint32_t size);
		virtual ~OpenGLTexture();

		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		
		virtual uint32_t GetRenderID() const override { return m_RendererID; }

		virtual void SetData(void* data, uint32_t size) override;
		virtual const std::vector<unsigned char> GetCurrentRenderData() override;
		virtual const uint32_t GetCurrentRenderDataSize() const override { return m_Data.size(); };

		virtual void FreeCurrentData() override;


		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImage(Acces acces, uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other)const override
		{
			return m_RendererID == ((OpenGLTexture&)other).m_RendererID;
		};


		virtual void Resize(uint32_t width, uint32_t height) override;

		static void BindTex(uint32_t renderID, uint32_t slot = 0);
		
	private:
		void Invalidate(void* m_Data = nullptr);
	private:
		TextureSpecification m_Specification;

		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormate, m_DataFormate;
		std::vector<unsigned char> m_Data;
	};

	class OpenGLTextureLinking : public Texture
	{
	public:
		OpenGLTextureLinking(AssetHandle assetHandle, int index);
		virtual ~OpenGLTextureLinking();

		virtual const TextureSpecification& GetSpecification() const override;

		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;
		virtual uint32_t GetRenderID() const override;

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImage(Acces acces, uint32_t slot = 0) const override;

		virtual void SetData(void* data, uint32_t size) override;
		virtual const std::vector<unsigned char> GetCurrentRenderData() override;
		virtual const uint32_t GetCurrentRenderDataSize() const override;

		virtual void FreeCurrentData() override;

		virtual bool operator==(const Texture& other)const override
		{
			return m_AssetHandle == ((OpenGLTextureLinking&)other).m_AssetHandle && m_Index == ((OpenGLTextureLinking&)other).m_Index;
		};


		virtual void Resize(uint32_t width, uint32_t height) override;

		static void BindTex(uint32_t renderID, uint32_t slot = 0);
	private:
		AssetHandle m_AssetHandle;
		int m_Index;
	};
}


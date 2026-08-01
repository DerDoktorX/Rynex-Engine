#pragma once
#include <Rynex/Renderer/API/Framebuffer.h>

namespace Rynex {

	class OpenGLTextureStorageModern;
	class OpenGLLinkedTextureArray;

	class Texture;
	class LinkedTextureArray;


	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		~OpenGLFramebuffer();

		virtual void ClearAttachmentNull(uint32_t index) override;
		virtual void ClearAttachment(uint32_t index, int value) override;
		virtual void ClearAttachment(uint32_t index, const glm::ivec4& value) override;
		virtual void ClearAttachment(uint32_t index, const glm::uvec4& value) override;
		virtual void ClearAttachment(uint32_t index, const glm::vec3& value) override;
		virtual void ClearAttachment(const uint32_t index, const glm::vec4& value) override;

		virtual bool SetTextureForDepthAttchment(const Ref<Texture>& texture) override;
		virtual bool SetTextureForColorAttchment(const Ref<Texture>& texture, uint32_t atchmentIndex) override;
#ifdef RY_TEXTURE_STORE_ARRAY
		virtual bool SetTextureForDepthAttchment(const Ref<StoreTextureArray>& texture, uint32_t texArrayindex) override;
		virtual bool SetTextureForColorAttchment(const Ref<StoreTextureArray>& texture, uint32_t texArrayindex, uint32_t atchmentIndex) override;
#endif

		virtual void ClearDeathAttachment(float value) override;

		virtual const FramebufferSpecification& GetFramebufferSpecification() const override;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;
		virtual uint32_t GetDeathAttachmentRendererID() const override;

		virtual Ref<Texture> GetAttachmentTexture(uint32_t index = 0)const override;

		virtual const std::vector<Ref<Texture>>& GetAttachmentsTextures() const override;
		virtual const uint32_t GetAttachmentTexturesSize() const override;

		virtual Ref<Texture> GetDepthTexture() const override;

		virtual void Resize2D(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t index, int x, int y) override;
		virtual const glm::uvec3& GetFrambufferSize() override;

		virtual void Bind(float width = 0.0f, float height = 0.0f, float x = 0.0f, float y = 0.0f) override;
		virtual void UnBind() override;

		virtual void BindColorAttachment(uint32_t index = 0, uint32_t slot = 0) const override;
		virtual void BindDeathAttachment(uint32_t slot = 0) const override;

		virtual void BindColorAttachmentImage(Acces acces, uint32_t index = 0, uint32_t slot = 0) const override;
		virtual void BindDeathAttachmentImage(Acces acces, uint32_t slot = 0) const override;

		uint32_t GetRenderID() const { return m_RendererID; }

		void OnChildeSpecifcationChange(OpenGLTextureStorageModern* ptrTex);
		void OnChildeDataChange(OpenGLTextureStorageModern* ptrTex);
		void OnChildeDestroy(OpenGLTextureStorageModern* ptrTex);

		void AddTextureParentToTextures();
	private:
		Ref<OpenGLTextureStorageModern> GetAttechmentTextureFromIndex(uint32_t index) const;
		void CreateID();
		void DestroyID();


		void Invalidate();
		void CreateAttechmentTexture(Ref<OpenGLTextureStorageModern>& texture, uint32_t slot);

		void ConecetTextureToFramffbuffer(const Ref<OpenGLTextureStorageModern>& texture, uint32_t slot);
#ifdef RY_TEXTURE_STORE_ARRAY
		void ConecetTextureToFramffbuffer(const Ref<StoreTextureArray>& textureArray, uint32_t texArrayindex, uint32_t slot);
#endif

		void SetFrameBufferStates(uint32_t countColorTex);


		uint32_t SetColoarAtchments(const std::vector<Ref<Texture>>& colorAttachments, uint32_t openglTexTarget = 0u);

		uint32_t SetupTextures();
		void OnFramebufferDataChangeAction();
		void OnFramebufferDataChangeAction(uint32_t index);
		void OnFramebufferDataChangeAction(Ref<Texture>& texture);
		void OnFramebufferDataChangeAction(Ref<OpenGLTextureStorageModern>& texture);

	private:
		std::vector<Ref<Texture>> m_ColorAttachmentsTex;
		Ref<OpenGLTextureStorageModern> m_DepthAttachment = nullptr;
		FramebufferSpecification m_Specification;

		uint32_t m_RendererID = 0;
		glm::uvec3 m_Size;

		
	};
}


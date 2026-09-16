#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Renderer/API/Texture.h>


namespace Rynex {

	enum class RYNEX_API FrameBufferImageSize : uint8_t
	{
		Nono = 0,
		MainViewPort,
		RelativeToMainViewPort,
		StaticSize
	};



	struct RYNEX_API FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;

		FramebufferTextureSpecification(const FramebufferTextureSpecification&) = default;

		FramebufferTextureSpecification(const TextureFormat format)
			: m_TextureFormat(format) { }

		FramebufferTextureSpecification(const TextureFormat format, const uint32_t samples)
			: m_TextureFormat(format), m_Samples(samples) { }

		FramebufferTextureSpecification(const TextureFormat format, const uint32_t samples, const TextureWrappingSpecification wrapping)
			: m_TextureFormat(format), m_Samples(samples), m_TextureWrapping(wrapping) { }

		FramebufferTextureSpecification(const TextureFormat format, const uint32_t samples, const TextureFilteringMode filtering)
			: m_TextureFormat(format), m_Samples(samples), m_TextureFiltering(filtering) { }

		FramebufferTextureSpecification(const TextureFormat format, const uint32_t samples, const TextureWrappingSpecification wrapping, const TextureFilteringMode filtering)
			: m_TextureFormat(format), m_Samples(samples), m_TextureWrapping(wrapping), m_TextureFiltering(filtering) { }

		FramebufferTextureSpecification(const TextureFormat format, const uint32_t samples, const TextureWrappingSpecification wrapping, const TextureFilteringMode filtering, const TextureCompareModes compare)
			: m_TextureFormat(format), m_Samples(samples), m_TextureWrapping(wrapping), m_TextureFiltering(filtering), Compare(compare) { }
		

		TextureFormat m_TextureFormat = TextureFormat::RGBA8;
		
		uint32_t m_Samples = 1;
		TextureWrappingSpecification m_TextureWrapping{
			TextureWrappingMode::ClampEdge,
			TextureWrappingMode::ClampEdge,
			TextureWrappingMode::ClampEdge
		};
		TextureFilteringMode m_TextureFiltering = TextureFilteringMode::Linear;
		TextureCompareModes Compare = TextureCompareModes::None;
		bool operator ==(const FramebufferTextureSpecification framebufferTextureSpecification) const
        {
			return (framebufferTextureSpecification.m_TextureFormat == m_TextureFormat) &&
				(framebufferTextureSpecification.m_TextureFiltering == m_TextureFiltering) &&
				(framebufferTextureSpecification.m_TextureWrapping == m_TextureWrapping);
		}
	};

	struct RYNEX_API FramebufferAttachmentSpecification
	{
	    std::vector<FramebufferTextureSpecification> m_Attachments;

		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification> attachments)
			: m_Attachments(attachments) {}

        explicit FramebufferAttachmentSpecification(const std::vector<FramebufferTextureSpecification>& attachments)
			: m_Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification>::iterator begin()
		{
			return m_Attachments.begin();
		}

		std::vector<FramebufferTextureSpecification>::iterator end()
		{
			return m_Attachments.end();
		}

		std::vector<FramebufferTextureSpecification>::const_iterator begin() const
		{
			return m_Attachments.begin();
		}

		std::vector<FramebufferTextureSpecification>::const_iterator end() const
		{
			return m_Attachments.end();
		}

		FramebufferTextureSpecification& operator[](const uint32_t index)
		{
			RY_CORE_ASSERT(index < m_Attachments.size());
			return m_Attachments[index];
		}

		const FramebufferTextureSpecification& operator[](const uint32_t index) const
		{
			RY_CORE_ASSERT(index < m_Attachments.size());
			return m_Attachments[index];
		}


	};

	struct RYNEX_API FramebufferSpecification
	{
		uint32_t m_Width, m_Height, m_Depth;
		FramebufferAttachmentSpecification m_Attachments;
		TextureTarget m_Target;
		uint32_t m_Samples;
		bool m_SwapChainTarget;

		FramebufferSpecification()
			: m_Width(1u), m_Height(1u), m_Depth(1u)
			, m_Attachments({}), m_Target(TextureTarget::Texture2D)
			, m_Samples(1u), m_SwapChainTarget(false)
		{
		}

		FramebufferSpecification(const uint32_t width, const uint32_t height, const uint32_t depth
			, const FramebufferAttachmentSpecification& attachment = FramebufferAttachmentSpecification()
			, const TextureTarget target = TextureTarget::Texture2D
			, const uint32_t samples = 1u
			, const bool swapChainTarget = false
			)
			: m_Width(width), m_Height(height), m_Depth(depth)
			, m_Attachments(attachment), m_Target(target)
			, m_Samples(samples), m_SwapChainTarget(swapChainTarget)
		{

		}

		FramebufferSpecification(const uint32_t width, const uint32_t height
			, const FramebufferAttachmentSpecification& attachment = FramebufferAttachmentSpecification()
			, const uint32_t samples = 1u
			, const bool swapChainTarget = false
		)
			: m_Width(width), m_Height(height), m_Depth(1u)
			, m_Attachments(attachment), m_Target(TextureTarget::Texture2D)
			, m_Samples(samples), m_SwapChainTarget(swapChainTarget)
		{
		}

	};


	class RYNEX_API Framebuffer : public Asset
	{
	public:

		virtual ~Framebuffer() = default;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

		virtual void ClearAttachmentNull(uint32_t index) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
		virtual void ClearAttachment(uint32_t index, const glm::ivec4& value) = 0;
		virtual void ClearAttachment(uint32_t index, const glm::uvec4& value) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, const glm::vec3& value) = 0;
		virtual void ClearAttachment(uint32_t attachmentIndex, const glm::vec4& value) = 0;
		virtual void ClearDeathAttachment(float value = 1.0f) = 0;
		virtual const FramebufferSpecification& GetFramebufferSpecification() const = 0;

		virtual bool SetTextureForDepthAttachment(const Ref<Texture>& texture) = 0;
		virtual bool SetTextureForColorAttachment(const Ref<Texture>& texture, uint32_t attachmentIndex) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDeathAttachmentRendererID() const = 0;

		virtual Ref<Texture> GetAttachmentTexture(uint32_t index = 0) const = 0;

		virtual const std::vector<Ref<Texture>>& GetAttachmentsTextures() const = 0;
		virtual const uint32_t GetAttachmentTexturesSize() const = 0;

		virtual Ref<Texture> GetDepthTexture() const = 0;

		virtual void Resize2D(uint32_t withe, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentsIndex, int x, int y) = 0;
		virtual const glm::uvec3& GetFramebufferSize() = 0;

		virtual void Bind(float width = 0.0f, float height = 0.0f, float x = 0.0f, float y = 0.0f) = 0;
		virtual void UnBind() = 0;

		virtual void BindColorAttachment(uint32_t index = 0, uint32_t slot = 0) const = 0;
		virtual void BindDeathAttachment(uint32_t slot = 0) const = 0;

		virtual void BindColorAttachmentImage(Access access, uint32_t index = 0, uint32_t slot = 0) const = 0;
		virtual void BindDeathAttachmentImage(Access access, uint32_t slot = 0) const = 0;

		static AssetType GetStaticType() { return AssetType::Framebuffer; }
		AssetType GetType() const override { return GetStaticType(); }
	};
}



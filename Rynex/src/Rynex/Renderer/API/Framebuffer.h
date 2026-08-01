#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Renderer/API/Texture.h>


namespace Rynex {
#if 1
	
#endif
	enum class RYNEX_API FrameBufferImageSize : uint8_t
	{
		Nono = 0,
		MainViewPort,
		RelativToMainViewPort,
		StaticSize
	};



	struct RYNEX_API FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;

		// FramebufferTextureSpecification(FramebufferTextureSpecification&&) = default;
		FramebufferTextureSpecification(const FramebufferTextureSpecification&) = default;

		FramebufferTextureSpecification(TextureFormat format)
			: TextureFormat(format) { }

		FramebufferTextureSpecification(TextureFormat format, uint32_t samples)
			: TextureFormat(format), Samples(samples) { }

		FramebufferTextureSpecification(TextureFormat format, uint32_t samples, TextureWrappingSpecification wrapping)
			: TextureFormat(format), Samples(samples), TextureWrapping(wrapping) { }

		FramebufferTextureSpecification(TextureFormat format, uint32_t samples, TextureFilteringMode filtering)
			: TextureFormat(format), Samples(samples), TextureFiltering(filtering) { }

		FramebufferTextureSpecification(TextureFormat format, uint32_t samples, TextureWrappingSpecification wrapping, TextureFilteringMode filtering)
			: TextureFormat(format), Samples(samples), TextureWrapping(wrapping), TextureFiltering(filtering) { }

		FramebufferTextureSpecification(TextureFormat format, uint32_t samples, TextureWrappingSpecification wrapping, TextureFilteringMode filtering, TextureCompareModes compare)
			: TextureFormat(format), Samples(samples), TextureWrapping(wrapping), TextureFiltering(filtering), Compare(compare) { }
		

		TextureFormat TextureFormat = TextureFormat::RGBA8;
		
		uint32_t Samples = 1;
		TextureWrappingSpecification TextureWrapping = {
			TextureWrappingMode::ClampEdge,
			TextureWrappingMode::ClampEdge,
			TextureWrappingMode::ClampEdge
		};
		TextureFilteringMode TextureFiltering = TextureFilteringMode::Linear;
		TextureCompareModes Compare = TextureCompareModes::None;
		bool operator ==(FramebufferTextureSpecification& framebufferTextureSpecification)
		{
			return (framebufferTextureSpecification.TextureFormat == TextureFormat) && 
				(framebufferTextureSpecification.TextureFiltering == TextureFiltering) &&
				(framebufferTextureSpecification.TextureWrapping == TextureWrapping);
		}
	};

	struct RYNEX_API FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}
		FramebufferAttachmentSpecification(std::vector<FramebufferTextureSpecification>& attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification>::iterator begin()
		{
			return Attachments.begin();
		}

		std::vector<FramebufferTextureSpecification>::iterator end()
		{
			return Attachments.end();
		}

		std::vector<FramebufferTextureSpecification>::const_iterator begin() const
		{
			return Attachments.begin();
		}

		std::vector<FramebufferTextureSpecification>::const_iterator end() const
		{
			return Attachments.end();
		}

		FramebufferTextureSpecification& operator[](uint32_t index)
		{
			RY_CORE_ASSERT(index < Attachments.size());
			return Attachments[index];
		}

		const FramebufferTextureSpecification& operator[](uint32_t index) const
		{
			RY_CORE_ASSERT(index < Attachments.size());
			return Attachments[index];
		}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct RYNEX_API FramebufferSpecification
	{
		uint32_t Width, Height, Depth;
		FramebufferAttachmentSpecification Attachments;
		TextureTarget Target;
		uint32_t Samples;
		bool SwapChainTarget;

		FramebufferSpecification()
			: Width(1u), Height(1u), Depth(1u)
			, Attachments({}), Target(TextureTarget::Texture2D)
			, Samples(1u), SwapChainTarget(false)
		{
		}

		FramebufferSpecification(uint32_t width, uint32_t height, uint32_t depth
			, FramebufferAttachmentSpecification atchemnts = FramebufferAttachmentSpecification()
			, TextureTarget target = TextureTarget::Texture2D
			, uint32_t samples = 1u
			, bool swapChainTarget = false
			)
			: Width(width), Height(height), Depth(depth)
			, Attachments(atchemnts), Target(target)
			, Samples(samples), SwapChainTarget(swapChainTarget)
		{

		}

		FramebufferSpecification(uint32_t width, uint32_t height
			, FramebufferAttachmentSpecification atchemnts = FramebufferAttachmentSpecification()
			, uint32_t samples = 1u
			, bool swapChainTarget = false
		)
			: Width(width), Height(height), Depth(1u)
			, Attachments(atchemnts), Target(TextureTarget::Texture2D)
			, Samples(samples), SwapChainTarget(swapChainTarget)
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

		virtual bool SetTextureForDepthAttchment(const Ref<Texture>& texture) = 0;
		virtual bool SetTextureForColorAttchment(const Ref<Texture>& texture, uint32_t atchmentIndex) = 0;
#ifdef RY_TEXTURE_STORE_ARRAY
		virtual bool SetTextureForDepthAttchment(const Ref<StoreTextureArray>& texture, uint32_t texArrayindex) = 0;
		virtual bool SetTextureForColorAttchment(const Ref<StoreTextureArray>& texture, uint32_t texArrayindex, uint32_t atchmentIndex) = 0;
#endif
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDeathAttachmentRendererID() const = 0;

		virtual Ref<Texture> GetAttachmentTexture(uint32_t index = 0) const = 0;

		virtual const std::vector<Ref<Texture>>& GetAttachmentsTextures() const = 0;
		virtual const uint32_t GetAttachmentTexturesSize() const = 0;

		virtual Ref<Texture> GetDepthTexture() const = 0;

		virtual void Resize2D(uint32_t withe, uint32_t heigth) = 0;
		virtual int ReadPixel(uint32_t attachmentsIndex, int x, int y) = 0;
		virtual const glm::uvec3& GetFrambufferSize() = 0;

		virtual void Bind(float width = 0.0f, float height = 0.0f, float x = 0.0f, float y = 0.0f) = 0;
		virtual void UnBind() = 0;

		virtual void BindColorAttachment(uint32_t index = 0, uint32_t slot = 0) const = 0;
		virtual void BindDeathAttachment(uint32_t slot = 0) const = 0;

		virtual void BindColorAttachmentImage(Acces acces, uint32_t index = 0, uint32_t slot = 0) const = 0;
		virtual void BindDeathAttachmentImage(Acces acces, uint32_t slot = 0) const = 0;

		static AssetType GetStaticType() { return AssetType::Framebuffer; }
		AssetType GetType() const { return GetStaticType(); }
	};
}



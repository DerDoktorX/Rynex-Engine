#include "rypch.h"
#include "OpenGLFramebuffer.h"

#include <Platform/OpenGL/OpenGLTexture.h>
#include <Platform/OpenGL/OpenGLBase.h>




#define GL_COLOR_ATTACHMENT_INDEX(x) (GL_COLOR_ATTACHMENT0)
namespace Rynex {

	constexpr uint32_t s_MAX_FRAMEBUFFER_SIZE = 8192;

	namespace Utils {

		static GLenum AttachmentType(const TextureFormat attachmentType)
		{
			switch (attachmentType)
			{
			case TextureFormat::RED_INTEGER:
			case TextureFormat::R8:
			case TextureFormat::RG8:

			case TextureFormat::RGB8:
			case TextureFormat::S_RGB8:
			case TextureFormat::RGB16F:
			case TextureFormat::RGB32F:

			case TextureFormat::RGBA8:
			case TextureFormat::S_RGBA8:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
		
				return GL_COLOR_ATTACHMENT0;

			// case TextureFormat::DepthComp:
			case TextureFormat::DepthComp16:
			case TextureFormat::DepthComp24:
			case TextureFormat::DepthComp32:
			case TextureFormat::DepthComp32F:
				return GL_DEPTH_ATTACHMENT;

			case TextureFormat::Depth24Stencil8:
			case TextureFormat::Depth32FStencil8:
				return GL_DEPTH_STENCIL_ATTACHMENT;
			}
			RY_CORE_ASSERT(false, "Error: Utils::ImageFormatToGLDataFormat!");
			return GL_COLOR_ATTACHMENT0;
		}

		static void TexturDefaultTypesColor(FramebufferTextureSpecification* attachment)
		{
			TexFilter& filter = attachment->m_TextureFiltering;
			if (filter == TexFilter::Default)
				filter = TexFilter::Nearest;

			TexFrom& fromat = attachment->m_TextureFormat;
			if (fromat == TexFrom::Default)
				fromat = TexFrom::Depth24Stencil8;

			TextureWrappingMode& warpT = attachment->m_TextureWrapping.m_T;
			if (warpT == TexWarp::Default)
				warpT = TexWarp::Repeat;

			TextureWrappingMode& warpR = attachment->m_TextureWrapping.m_R;
			if (warpR == TexWarp::Default)
				warpR = TexWarp::None;

			TextureWrappingMode& warpS = attachment->m_TextureWrapping.m_S;
			if (warpS == TexWarp::Default)
				warpS = TexWarp::Repeat;
		}

		static bool IsDeathTex(TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::RED_INTEGER:
			case TextureFormat::R8:			
			case TextureFormat::RG8:

			case TextureFormat::RGB8:	
			case TextureFormat::S_RGB8:
			case TextureFormat::RGB16F:	
			case TextureFormat::RGB32F:

			case TextureFormat::RGBA8:
			case TextureFormat::S_RGBA8:
			case TextureFormat::RGBA16F:	
			case TextureFormat::RGBA32F:	
				return false;


			// case TextureFormat::DepthComp:
			case TextureFormat::DepthComp16:
			case TextureFormat::DepthComp24:
			case TextureFormat::DepthComp32:
			case TextureFormat::DepthComp32F:

			case TextureFormat::Depth24Stencil8:
			case TextureFormat::Depth32FStencil8:
				return true;

			case TextureFormat::Default:
			case TextureFormat::None:	
				RY_CORE_ASSERT(false, "None or devout Formats should not used!");
				break;
			default:
				break;
			}
			RY_CORE_ASSERT(false, "Not set or not known TextureFormat!");
			return false;
		}

		static void TextureDefaultTypesDepth(FramebufferTextureSpecification* attachment)
		{
			TexFilter& filter = attachment->m_TextureFiltering;
			if (filter==TexFilter::Default)
				filter = TexFilter::Nearest;

			TexFrom& format = attachment->m_TextureFormat;
			if (format==TexFrom::Default)
				format = TexFrom::Depth24Stencil8;

			TextureWrappingMode& warpT = attachment->m_TextureWrapping.m_T;
			if (TexWarp::Default==warpT)
				warpT = TexWarp::ClampEdge;

			TextureWrappingMode& warpR = attachment->m_TextureWrapping.m_R;
			if (TexWarp::Default==warpR)
				warpR = TexWarp::None;

			TextureWrappingMode& warpS = attachment->m_TextureWrapping.m_S;
			if (TexWarp::Default==warpS)
				warpS = TexWarp::ClampEdge;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		uint32_t& withe = m_Specification.m_Width;
		uint32_t& height = m_Specification.m_Height;
		uint32_t& depth = m_Specification.m_Depth;
		TextureTarget& target = m_Specification.m_Target;

		switch (target)
		{
			case TextureTarget::Texture2D:
			case TextureTarget::TextureCubeMap:
			{
				break;
			}
			case TextureTarget::Default:
			{
				RY_CORE_ERROR("We can't see if the target is valid so we defined Default as TextureTarget::Texture2D");
				target = TextureTarget::Texture2D;
				break;
			}
			case TextureTarget::Texture1D:
			case TextureTarget::Texture3D:
			case TextureTarget::TextureRectAngle:
			case TextureTarget::TextureBuffer:
			default:
			{
				RY_CORE_ASSERT(false, "No valid TextureTarget for a Framebuffer");
				break;
			}
		}
		

		m_Size = glm::uvec3{ withe, height, depth };
		constexpr uint32_t midmapsLevel = 0u;
		for (FramebufferTextureSpecification& attachment : m_Specification.m_Attachments)
		{
			bool depthFormat = Utils::IsDeathTex(attachment.m_TextureFormat);
			if (nullptr == m_DepthAttachment && depthFormat)
			{
				Utils::TextureDefaultTypesDepth(&attachment);
				TextureSpecification spec{
					withe, height, depth,
					target,
					attachment.m_TextureFormat,
					attachment.m_Samples,
					attachment.m_TextureFiltering,
					attachment.m_TextureWrapping,
					attachment.Compare,
					midmapsLevel
				};
				m_DepthAttachment = CreateRef<OpenGLTextureStorageModern>(spec);

				uint32_t countColorTex = m_ColorAttachmentsTex.size();
				uint32_t countAttachmentsCount = m_Specification.m_Attachments.m_Attachments.size();
				uint32_t expectColorCount = countAttachmentsCount - 1u;
				if (expectColorCount != countColorTex)
				{
					RY_CORE_WARN("We Expect the Depth Buffer to be the last Element! (Possible problems withe Attachment Index)");
				}
			}
			else if(!depthFormat)
			{
				Utils::TextureDefaultTypesColor(&attachment);
				TextureSpecification spec{
					withe, height, depth,
					target,
					attachment.m_TextureFormat,
					attachment.m_Samples,
					attachment.m_TextureFiltering,
					attachment.m_TextureWrapping,
					attachment.Compare,
					midmapsLevel
				};
				Ref<OpenGLTextureStorageModern> tex = CreateRef<OpenGLTextureStorageModern>(spec);
				m_ColorAttachmentsTex.push_back(tex);
			}	
			else
			{
				RY_CORE_ERROR("No Framebuffer can use more then one Depth Texture! (We use only the First found Depth Texture!)");
			}
		}
		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
		if(m_RendererID)
		{
			RY_GRAFIC_DELETE(m_RendererID, OpenGLFramebuffer);
			glDeleteFramebuffers(1,&m_RendererID);
			m_RendererID = 0;
		}


		for (Ref<Texture>& tex : m_ColorAttachmentsTex)
		{
			RY_DESTROY_REF(tex);
		}

		m_ColorAttachmentsTex.clear();
		m_ColorAttachmentsTex.shrink_to_fit();

		RY_DESTROY_REF(m_DepthAttachment);
	}

	void OpenGLFramebuffer::ClearAttachmentNull(uint32_t index)
	{

		const GLint drawBuffer = index;
		
		Ref<OpenGLTextureStorageModern> texture = GetAttechmentTextureFromIndex(index);
		const TextureSpecification& spec = texture->GetSpecification();
		TexFrom format = spec.m_Format;

		
		switch (format)
		{
		case TexFrom::R8:
		case TexFrom::RG8:

		case TexFrom::RGB8:
		case TexFrom::RGB16F:
		case TexFrom::RGB32F:
		case TexFrom::S_RGB8:

		case TexFrom::RGBA8:
		case TexFrom::RGBA16F:
		case TexFrom::RGBA32F:
		case TexFrom::S_RGBA8:
		{
			glm::vec4 color(0.0f, 0.0f, 0.0f, 0.0f);
			glClearNamedFramebufferfv(m_RendererID, GL_COLOR, drawBuffer, glm::value_ptr(color));
			OnFramebufferDataChangeAction(texture);
			break;
		}
			
		case TexFrom::RED_INTEGER:
		{
			glm::ivec4 color(0, 0, 0, 0);
			glClearNamedFramebufferiv(m_RendererID, GL_COLOR, drawBuffer, glm::value_ptr(color));
			OnFramebufferDataChangeAction(texture);
			break;
		}
			

		default:
			RY_CORE_ASSERT(false, "This format is not expected!");
			break;
		}
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, int value)
	{
		glm::ivec4 value4{
		    value, 0, 0, 0
		};
		ClearAttachment(index, value4);
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, const glm::ivec4& value)
	{
		GLint drawBuffer = index;
		glClearNamedFramebufferiv(m_RendererID, GL_COLOR, drawBuffer, glm::value_ptr(value));
		GL_CHECK_LOOP();
		OnFramebufferDataChangeAction(index);

	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, const glm::uvec4& value)
	{
		GLint drawBuffer = index;
		glClearNamedFramebufferuiv(m_RendererID, GL_COLOR, drawBuffer, glm::value_ptr(value));
		GL_CHECK_LOOP();

		OnFramebufferDataChangeAction(index);
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, const glm::vec3& value)
	{
		glm::vec4 value4 = glm::vec4(value, 0.0f);
		ClearAttachment(index, value4);
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, const glm::vec4& value)
	{
		GLint drawBuffer = index;
		glClearNamedFramebufferfv(m_RendererID, GL_COLOR, drawBuffer, glm::value_ptr(value));
		GL_CHECK_LOOP();

		OnFramebufferDataChangeAction(index);
	}

	bool OpenGLFramebuffer::SetTextureForDepthAttachment(const Ref<Texture>& texture)
	{
		if(nullptr == m_DepthAttachment || nullptr == texture || m_DepthAttachment->GetSpecification() != texture->GetSpecification())
		{
			RY_CORE_ERROR("Depth Texture change (SetTextureForDepthAttachment) Failed!");
			return false;
		}
		if(texture.get() == m_DepthAttachment.get())
		{
			RY_CORE_WARN("Depth Texture was already set by that texture (SetTextureForDepthAttachment)!");
			return true;
		}
	    const Ref<OpenGLTextureStorageModern> openglDepthAttachment = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(m_DepthAttachment);
		const Ref<OpenGLTextureStorageModern> openglTexture = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
		openglDepthAttachment->RemoveParent(this);
		RY_DESTROY_REF(m_DepthAttachment);

		m_DepthAttachment = texture;

		openglTexture->AddParent(this);
		Invalidate();
		return true;
	}

	bool OpenGLFramebuffer::SetTextureForColorAttachment(const Ref<Texture>& texture, uint32_t attachmentIndex)
	{

		if (m_ColorAttachmentsTex.size() <= attachmentIndex || nullptr == texture || m_ColorAttachmentsTex.at(attachmentIndex)->GetSpecification() != texture->GetSpecification())
		{
			RY_CORE_ERROR("Color Texture change on attachment[{}] (SetTextureForDepthAttachment) Failed!", attachmentIndex);
			return false;
		}
		Ref<Texture>& attachmentTexture = m_ColorAttachmentsTex.at(attachmentIndex);

		if (attachmentTexture.get() == m_DepthAttachment.get())
		{
			RY_CORE_WARN("Color Texture on attachment[{}] was already set by that texture (SetTextureForDepthAttachment)!", attachmentIndex);
			return true;
		}
		Ref<OpenGLTextureStorageModern> openglAttachmentTexture = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(attachmentTexture);
		const Ref<OpenGLTextureStorageModern> openglTexture = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
		openglAttachmentTexture->RemoveParent(this);
		RY_DESTROY_REF(attachmentTexture);
		RY_DESTROY_REF(openglAttachmentTexture);
		attachmentTexture = openglTexture;
		openglTexture->AddParent(this);
		Invalidate();
		return true;
	}

	void OpenGLFramebuffer::ClearDeathAttachment(float value)
	{
		RY_CORE_ASSERT(nullptr != m_DepthAttachment, "no Death Attachment!");
	    const Ref<OpenGLTextureStorageModern> openglDepthAttachment = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(m_DepthAttachment);
		uint32_t formate = openglDepthAttachment->GetOpenGLTextureDataFormate();

		switch (formate)
		{
		case GL_DEPTH_COMPONENT:
		{
			GLfloat depthValue = value;
			glClearBufferfv(GL_DEPTH, 0, &value);
			GL_CHECK_LOOP();

			OnFramebufferDataChangeAction(m_DepthAttachment);
			return;
		}
		case GL_DEPTH_STENCIL:
		{
			GLfloat depth = value;
			GLint stencil = 0;
			glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
			GL_CHECK_LOOP();

			OnFramebufferDataChangeAction(m_DepthAttachment);
			return;
		}
		default:
			break;
		}
		std::string formatStr = OpenGL::GetTextureFormateStr(formate);
		RY_CORE_ASSERT(false, "not Vaild OpenGL target Data! {}", formatStr);
		
	}

	const FramebufferSpecification& OpenGLFramebuffer::GetFramebufferSpecification() const
	{
		return m_Specification;
	}

	uint32_t OpenGLFramebuffer::GetColorAttachmentRendererID(uint32_t index) const
	{
		Ref<OpenGLTextureStorageModern> texture = GetAttechmentTextureFromIndex(index);
		uint32_t renderID = texture->GetRenderID();
		return renderID;
	}

	uint32_t OpenGLFramebuffer::GetDeathAttachmentRendererID() const
	{
		return m_DepthAttachment->GetRenderID();
	}

	void OpenGLFramebuffer::Bind(float width, float height, float x, float y)
	{
		OpenGLRenderCommand::BindFramebuffer(m_RendererID);
		GLsizei widthS = width == 0.0f ? m_Specification.m_Width : width;
		GLsizei heightS = height == 0.0f ? m_Specification.m_Height : height;
		glViewport(x, y, widthS, heightS);
	}

	void OpenGLFramebuffer::UnBind()
	{
		GLsizei widthS = m_Specification.m_Width;
		GLsizei heightS = m_Specification.m_Height;
		uint32_t defaultTexture = OpenGLRenderCommand::GetDefaultFrambufferRenderID();
		glm::uvec2 windowSize = OpenGL::GetMainWindowCurentSize();
		OpenGLRenderCommand::BindFramebuffer(defaultTexture);

		glViewport(0, 0, windowSize.x, windowSize.y);

	}

	void OpenGLFramebuffer::BindColorAttachment(uint32_t index, uint32_t slot) const
	{
		Ref<OpenGLTextureStorageModern> texture = GetAttechmentTextureFromIndex(index);
		texture->Bind(slot);
	}

	void OpenGLFramebuffer::BindDeathAttachment(uint32_t slot) const
	{
		m_DepthAttachment->Bind(slot);
	}

	void OpenGLFramebuffer::BindColorAttachmentImage(Access acces, uint32_t index, uint32_t slot) const
	{
		RY_CORE_ASSERT(index < m_ColorAttachmentsTex.size(), "Error: OpenGLFramebuffer::BindColorAttachmentImage!");
		m_ColorAttachmentsTex[index]->BindImage(acces, slot);
	}

	void OpenGLFramebuffer::BindDeathAttachmentImage(Access acces, uint32_t slot) const
	{
		m_DepthAttachment->BindImage(acces, slot);
	}



	Ref<OpenGLTextureStorageModern> OpenGLFramebuffer::GetAttechmentTextureFromIndex(uint32_t index) const
	{
		RY_CORE_ASSERT(index < m_ColorAttachmentsTex.size(), "Error: OpenGLFramebuffer::GetAttechmentTextureFromIndex!");
		const Ref<Texture>& texture = m_ColorAttachmentsTex.at(index);
		Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
		return texutureStorage;
	}

	void OpenGLFramebuffer::CreateID()
	{
		DestroyID();

		RY_CORE_ASSERT(0u == m_RendererID);

		RY_OPENGL_FRAME_BUFFER_ID_SCOPE_LOCK();
		glCreateFramebuffers(1u, &m_RendererID);
		RY_GRAFIC_CREATE(m_RendererID, OpenGLFramebuffer);

		GL_CHECK();
	}

	void OpenGLFramebuffer::DestroyID()
	{
		if (0u == m_RendererID)
			return;
		RY_OPENGL_FRAME_BUFFER_ID_SCOPE_LOCK();

		RY_GRAFIC_DELETE(m_RendererID, OpenGLFramebuffer);
		glDeleteFramebuffers(1u, &m_RendererID);
		m_RendererID = 0u;
		
		GL_CHECK();
	}

	void OpenGLFramebuffer::Invalidate()
	{
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLFramebuffer::Invalidate);

		CreateID();

		const uint32_t colorAttachmentCount = SetupTextures();
		SetFrameBufferStates(colorAttachmentCount);
	}

	void OpenGLFramebuffer::CreateAttachmentTexture(const  Ref<OpenGLTextureStorageModern>& texture, const uint32_t slot)
	{
		const uint32_t withe  = m_Size.x;
		const uint32_t height = m_Size.y;
		const uint32_t depth = m_Size.z;

		const TextureSpecification& attachment = texture->GetSpecification();

		texture->SetSpecfication({
				withe, height,depth,
				attachment.m_Target,
				attachment.m_Format,

				attachment.m_Samples,
				attachment.m_FilteringMode,
				attachment.WrappingSpec,
				attachment.m_Compare
		}, this);
		ConecetTextureToFramffbuffer(texture, slot);
	}


	void OpenGLFramebuffer::ConecetTextureToFramffbuffer(const Ref<OpenGLTextureStorageModern>& texture, uint32_t slot)
	{
		const TextureSpecification& specs = texture->GetSpecification();
		const TextureFormat formate = specs.m_Format;
		const uint32_t attachmentType = Utils::AttachmentType(formate); // Color, Depth, Stencil

		uint32_t targetFormat = texture->GetOpenGLTextureTarget();
		const uint32_t textureRenderID = texture->GetRenderID();
		const uint32_t attachmentTypeSLot = attachmentType + slot;

		glNamedFramebufferTexture(m_RendererID, attachmentTypeSLot, textureRenderID, 0);
	}


	void OpenGLFramebuffer::OnChildeSpecifcationChange(OpenGLTextureStorageModern* ptrTex)
	{
		RY_CORE_WARN("Children Texture of Framebuffer has specification changed this is not expected To happen!, maby Change of data is forced by Framebuffer!");
	}

	void OpenGLFramebuffer::OnChildeDataChange(OpenGLTextureStorageModern* ptrTex)
	{
		RY_CORE_WARN("Children Texture of Framebuffer has changed this is not expected To happen!, maby Change of data is forced by Framebuffer!");
	}

	void OpenGLFramebuffer::OnChildeDestroy(OpenGLTextureStorageModern* ptrTex)
	{
		RY_CORE_ASSERT(false, "This Funktion Should never be called!");
	}

	void OpenGLFramebuffer::AddTextureParentToTextures()
	{
		for (const Ref<Texture>& texture : m_ColorAttachmentsTex)
		{
			const Ref<OpenGLTextureStorageModern> textureObject = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
			textureObject->AddParent(this);
		}
		if (nullptr != m_DepthAttachment)
		{
		    const Ref<OpenGLTextureStorageModern> textureObject = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(m_DepthAttachment);
		    textureObject->AddParent(this);
		}
	}

	void OpenGLFramebuffer::SetFrameBufferStates(const uint32_t colorTextureCount)
	{
		if (0 != colorTextureCount)
		{
		    constexpr uint32_t MAX_TEXTURE_COUNT = 10u;
			RY_CORE_ASSERT(colorTextureCount <= MAX_TEXTURE_COUNT, "Error: OpenGLFramebuffer to many Color Attachments!");
			const GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
				GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
			};
			glNamedFramebufferDrawBuffers(m_RendererID, colorTextureCount, buffers);
		}
		else
		{
			glNamedFramebufferDrawBuffer(m_RendererID, GL_NONE);
			glNamedFramebufferReadBuffer(m_RendererID, GL_NONE);
		}
	    const GLenum state = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		RY_CORE_ASSERT(GL_FRAMEBUFFER_COMPLETE == state, "Framebuffer is not Complete!");

	}

	uint32_t OpenGLFramebuffer::SetColorAttachments(const std::vector<Ref<Texture>>& colorAttachments, uint32_t openGLTextureTarget)
	{
		const uint32_t texSize = colorAttachments.size();
		if (0u == texSize)
			return 0u;

		uint32_t i = 0;
		for (const Ref<Texture>& textureObject : colorAttachments)
		{
			Ref<OpenGLTextureStorageModern> textureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(textureObject);
			CreateAttachmentTexture(textureStorage, i);
			i++;
		}
		return i;
	}


	uint32_t OpenGLFramebuffer::SetupTextures()
	{
		uint32_t colorAttachmentsSize = 0;
		colorAttachmentsSize += SetColorAttachments(m_ColorAttachmentsTex);

		if(nullptr != m_DepthAttachment)
		{
		    Ref<OpenGLTextureStorageModern> openglDepthAttachment = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(m_DepthAttachment);
		    CreateAttachmentTexture(openglDepthAttachment, 0u);
		}
		return colorAttachmentsSize;
	}

	void OpenGLFramebuffer::OnFramebufferDataChangeAction()
	{
		for (Ref<Texture>& texture : m_ColorAttachmentsTex)
		{
			OnFramebufferDataChangeAction(texture);
		}
		if (nullptr != m_DepthAttachment)
		{
			OnFramebufferDataChangeAction(m_DepthAttachment);
		}
	}

	void OpenGLFramebuffer::OnFramebufferDataChangeAction(uint32_t index)
	{
		Ref<OpenGLTextureStorageModern> texture = GetAttechmentTextureFromIndex(index);
		OnFramebufferDataChangeAction(texture);
	}
	void OpenGLFramebuffer::OnFramebufferDataChangeAction(Ref<Texture>& texture)
	{
		Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
		OnFramebufferDataChangeAction(texutureStorage);
	}

	void OpenGLFramebuffer::OnFramebufferDataChangeAction(Ref<OpenGLTextureStorageModern>& texture)
	{
		texture->ChangedDataFromParent(this);
	}


	Ref<Texture> OpenGLFramebuffer::GetAttachmentTexture(uint32_t index) const
	{
		Ref<OpenGLTextureStorageModern> texture = GetAttechmentTextureFromIndex(index);


		Ref<Texture> attachmentTexCopy = static_cast<Ref<Texture>>(texture);
		return attachmentTexCopy;
	}

	const std::vector<Ref<Texture>>& OpenGLFramebuffer::GetAttachmentsTextures() const
	{
		return m_ColorAttachmentsTex;
	}

	const uint32_t OpenGLFramebuffer::GetAttachmentTexturesSize() const
	{
		return m_ColorAttachmentsTex.size();
	}

	Ref<Texture> OpenGLFramebuffer::GetDepthTexture() const
	{
		return m_DepthAttachment;
	}

	

	void OpenGLFramebuffer::Resize2D(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MAX_FRAMEBUFFER_SIZE || height > s_MAX_FRAMEBUFFER_SIZE)
		{
			RY_CORE_WARN("Faild Resize frambueffer to {0}, {1}", width, height);
			return;
		}
		m_Size = { width , height, m_Specification.m_Depth };
		m_Specification.m_Width = m_Size.x;
		m_Specification.m_Height = m_Size.y;
		m_Specification.m_Depth = m_Size.z;

		RY_CORE_TRACE("Resize frambueffer to {0}, {1}, {2}", m_Size.x, m_Size.y, m_Size.z);
		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentsIndex, int x, int y)
	{
		Ref<OpenGLTextureStorageModern> texture = GetAttechmentTextureFromIndex(attachmentsIndex);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentsIndex);
		int pixeldata;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixeldata);

		return pixeldata;
	}

	const glm::uvec3& OpenGLFramebuffer::GetFramebufferSize()
	{
		return m_Size;
	}

}

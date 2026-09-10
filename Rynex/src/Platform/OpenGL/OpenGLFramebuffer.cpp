#include "rypch.h"
#include "OpenGLFramebuffer.h"

#include <Platform/OpenGL/OpenGLTexture.h>
#include <Platform/OpenGL/OpenGLBase.h>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>



#define GL_COLOR_ATTACHMENT_INDEX(x) (GL_COLOR_ATTACHMENT0)
namespace Rynex {

	static const uint32_t s_MaxFrambufferSize = 8192;

	namespace Utils {

		static GLenum AtchemtType(TextureFormat attachmentType)
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
			TexFilter& filter = attachment->TextureFiltering;
			if (filter == TexFilter::Default)
				filter = TexFilter::Nearest;

			TexFrom& fromat = attachment->TextureFormat;
			if (fromat == TexFrom::Default)
				fromat = TexFrom::Depth24Stencil8;

			TextureWrappingMode& warpT = attachment->TextureWrapping.T;
			if (warpT == TexWarp::Default)
				warpT = TexWarp::Repeate;

			TextureWrappingMode& warpR = attachment->TextureWrapping.R;
			if (warpR == TexWarp::Default)
				warpR = TexWarp::None;

			TextureWrappingMode& warpS = attachment->TextureWrapping.S;
			if (warpS == TexWarp::Default)
				warpS = TexWarp::Repeate;
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
				RY_CORE_ASSERT(false, "None or defout Formats Shoudbe not used!");
				break;
			default:
				break;
			}
			RY_CORE_ASSERT(false, "Not set or not known TexureFormat!");
			return false;
		}

		static void TexturDefaultTypesDepth(FramebufferTextureSpecification* attachment)
		{
			TexFilter& filter = attachment->TextureFiltering;
			if (filter == TexFilter::Default)
				filter = TexFilter::Nearest;

			TexFrom& fromat = attachment->TextureFormat;
			if (fromat == TexFrom::Default)
				fromat = TexFrom::Depth24Stencil8;

			TextureWrappingMode& warpT = attachment->TextureWrapping.T;
			if (warpT == TexWarp::Default)
				warpT = TexWarp::ClampEdge;

			TextureWrappingMode& warpR = attachment->TextureWrapping.R;
			if (warpR == TexWarp::Default)
				warpR = TexWarp::None;

			TextureWrappingMode& warpS = attachment->TextureWrapping.S;
			if (warpS == TexWarp::Default)
				warpS = TexWarp::ClampEdge;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{

		
		uint32_t& withe = m_Specification.Width;
		uint32_t& height = m_Specification.Height;
		uint32_t& depth = m_Specification.Depth;
		TextureTarget& target = m_Specification.Target;

		switch (target)
		{
			case TextureTarget::Texture2D:
			case TextureTarget::TextureCubeMap:
			{
				break;
			}
			case TextureTarget::Default:
			{
				RY_CORE_ERROR("We can't see if the target is vaild so we defined Default as TextureTarget::Texture2D");
				target = TextureTarget::Texture2D;
				break;
			}
			case TextureTarget::Texture1D:
			case TextureTarget::Texture3D:
			case TextureTarget::TextureRectAngle:
			case TextureTarget::TextureBuffer:
			default:
			{
				RY_CORE_ASSERT(false, "No Vaild TextureTarget for a Frambuffer");
				break;
			}
		}
		

		m_Size = { withe , height, depth };
		constexpr uint32_t midmapsLevel = 0u;
		for (FramebufferTextureSpecification& attachment : m_Specification.Attachments)
		{
			bool depthFormat = Utils::IsDeathTex(attachment.TextureFormat);
			if (nullptr == m_DepthAttachment && depthFormat)
			{
				Utils::TexturDefaultTypesDepth(&attachment);
				TextureSpecification spec = {
					withe, height, depth,
					target,
					attachment.TextureFormat,
					attachment.Samples,
					attachment.TextureFiltering,
					attachment.TextureWrapping,
					attachment.Compare,
					midmapsLevel
				};
				m_DepthAttachment = CreateRef<OpenGLTextureStorageModern>(spec);

				uint32_t countColorTex = m_ColorAttachmentsTex.size();
				uint32_t countAttachmentsCount = m_Specification.Attachments.Attachments.size();
				uint32_t expexteColorCount = countAttachmentsCount - 1u;
				if (expexteColorCount != countColorTex)
				{
					RY_CORE_WARN("We Expext the Depth Buffer to be the last Ellement! (Posible problems withe Attachment Index)");
				}
			}
			else if(!depthFormat)
			{
				Utils::TexturDefaultTypesColor(&attachment);
				TextureSpecification spec = {
					withe, height, depth,
					target,
					attachment.TextureFormat,
					attachment.Samples,
					attachment.TextureFiltering,
					attachment.TextureWrapping,
					attachment.Compare,
					midmapsLevel

				};
				Ref<OpenGLTextureStorageModern> tex = CreateRef<OpenGLTextureStorageModern>(spec);
				m_ColorAttachmentsTex.push_back(tex);
			}	
			else
			{
				RY_CORE_ERROR("No Farmbuffer Can Use more then one Depth Texture! (We use only the First found Depth Texture!)");
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
		
		GLint drawBuffer = index;
		
		Ref<OpenGLTextureStorageModern> texture = GetAttechmentTextureFromIndex(index);
		const TextureSpecification& spec = texture->GetSpecification();
		TexFrom fromat = spec.Format;

		
		switch (fromat)
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
			RY_CORE_ASSERT(false, "This fomate is not expexted!");
			break;
		}
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t index, int value)
	{
		glm::ivec4 value4 = glm::ivec4(value, 0, 0, 0);
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

	bool OpenGLFramebuffer::SetTextureForDepthAttchment(const Ref<Texture>& texture)
	{
		if(nullptr == m_DepthAttachment || nullptr == texture || m_DepthAttachment->GetSpecification() != texture->GetSpecification())
		{
			RY_CORE_ERROR("Depth Texture change (SetTextureForDepthAttchment) Faild!");
			return false;
		}
		if(texture.get() == m_DepthAttachment.get())
		{
			RY_CORE_WARN("Depth Texture was alrady set by that texture (SetTextureForDepthAttchment)!");
			return true;
		}
		Ref<OpenGLTextureStorageModern> openglTexture = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
		m_DepthAttachment->RemoveParent(this);
		RY_DESTROY_REF(m_DepthAttachment);

		m_DepthAttachment = openglTexture;
		m_DepthAttachment->AddParent(this);
		Invalidate();
		return true;
	}

	bool OpenGLFramebuffer::SetTextureForColorAttchment(const Ref<Texture>& texture, uint32_t atchmentIndex)
	{

		if (m_ColorAttachmentsTex.size() <= atchmentIndex || nullptr == texture || m_ColorAttachmentsTex.at(atchmentIndex)->GetSpecification() != texture->GetSpecification())
		{
			RY_CORE_ERROR("Color Texture change on athcment[{}] (SetTextureForDepthAttchment) Faild!", atchmentIndex);
			return false;
		}
		Ref<Texture>& attchmentTexture = m_ColorAttachmentsTex.at(atchmentIndex);

		if (attchmentTexture.get() == m_DepthAttachment.get())
		{
			RY_CORE_WARN("Color Texture on atchment[{}] was alrady set by that texture (SetTextureForDepthAttchment)!", atchmentIndex);
			return true;
		}
		Ref<OpenGLTextureStorageModern> openglAttchmentTexture = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(attchmentTexture);
		Ref<OpenGLTextureStorageModern> openglTexture = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
		openglAttchmentTexture->RemoveParent(this);
		RY_DESTROY_REF(attchmentTexture);
		RY_DESTROY_REF(openglAttchmentTexture);

		attchmentTexture = openglTexture;
		openglTexture->AddParent(this);
		Invalidate();
		return true;
	}

	void OpenGLFramebuffer::ClearDeathAttachment(float value)
	{
		RY_CORE_ASSERT(nullptr != m_DepthAttachment, "no Death Attachment!");
		uint32_t formate = m_DepthAttachment->GetOpenGLTextureDataFormate();

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
		std::string formatStr = OpenGL::GetTextureFomateStr(formate);
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
		GLsizei widthS = width == 0.0f ? m_Specification.Width : width;
		GLsizei heightS = height == 0.0f ? m_Specification.Height : height;
		glViewport(x, y, widthS, heightS);
	}

	void OpenGLFramebuffer::UnBind()
	{
		GLsizei widthS = m_Specification.Width;
		GLsizei heightS = m_Specification.Height;
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

	void OpenGLFramebuffer::BindColorAttachmentImage(Acces acces, uint32_t index, uint32_t slot) const
	{
		RY_CORE_ASSERT(index < m_ColorAttachmentsTex.size(), "Error: OpenGLFramebuffer::BindColorAttachmentImage!");
		m_ColorAttachmentsTex[index]->BindImage(acces, slot);
	}

	void OpenGLFramebuffer::BindDeathAttachmentImage(Acces acces, uint32_t slot) const
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

		uint32_t colorAtchmentCount = SetupTextures();
		SetFrameBufferStates(colorAtchmentCount);
	}

	void OpenGLFramebuffer::CreateAttechmentTexture(Ref<OpenGLTextureStorageModern>& texture, uint32_t slot)
	{
		uint32_t withe  = m_Size.x;
		uint32_t height = m_Size.y;
		uint32_t depth = m_Size.z;

		const TextureSpecification& attachment = texture->GetSpecification();
		


		texture->SetSpecfication({
				withe, height,depth,
				attachment.Target,
				attachment.Format,

				attachment.Samples,
				attachment.FilteringMode,
				attachment.WrappingSpec,
				attachment.Compare
		}, this);
		ConecetTextureToFramffbuffer(texture, slot);
	}


	void OpenGLFramebuffer::ConecetTextureToFramffbuffer(const Ref<OpenGLTextureStorageModern>& texture, uint32_t slot)
	{
		const TextureSpecification& specs = texture->GetSpecification();
		TextureFormat formate = specs.Format;
		uint32_t attecmentType = Utils::AtchemtType(formate); // Color, Depth, Stencil

		uint32_t targetFromat = texture->GetOpenGLTextureTarget();
		uint32_t textureRenderID = texture->GetRenderID();
		uint32_t attachmentTypeSLot = attecmentType + slot;

		glNamedFramebufferTexture(m_RendererID, attachmentTypeSLot, textureRenderID, 0);
	}


	void OpenGLFramebuffer::OnChildeSpecifcationChange(OpenGLTextureStorageModern* ptrTex)
	{
		RY_CORE_WARN("Children Texture of Frambuffer has specifcation changed this is not expected To happen!, maby Change of data is forced by Frambuffer!");
	}

	void OpenGLFramebuffer::OnChildeDataChange(OpenGLTextureStorageModern* ptrTex)
	{
		RY_CORE_WARN("Children Texture of Frambuffer has changed this is not expected To happen!, maby Change of data is forced by Frambuffer!");
	}

	void OpenGLFramebuffer::OnChildeDestroy(OpenGLTextureStorageModern* ptrTex)
	{
		RY_CORE_ASSERT(false, "This Funktion Shoud never be called!");
	}

	void OpenGLFramebuffer::AddTextureParentToTextures()
	{
		for (const Ref<Texture>& texure : m_ColorAttachmentsTex)
		{
			Ref<OpenGLTextureStorageModern> texureObject = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texure);
			texureObject->AddParent(this);
		}
		if (nullptr != m_DepthAttachment)
		{
			m_DepthAttachment->AddParent(this);
		}
	}

	void OpenGLFramebuffer::SetFrameBufferStates(uint32_t countColorTex)
	{
		if (countColorTex)
		{
			RY_CORE_ASSERT(countColorTex <= 10, "Error: OpenGLFramebuffer to many Color Attachments!");
			GLenum buffers[10] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4,
				GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9,
			};
			glNamedFramebufferDrawBuffers(m_RendererID, countColorTex, buffers);
		}
		else if (countColorTex == 0)
		{
			glNamedFramebufferDrawBuffer(m_RendererID, GL_NONE);
			glNamedFramebufferReadBuffer(m_RendererID, GL_NONE);
		}
		else
		{
			RY_CORE_ASSERT(false, "Not expexted State!");
		}
		RY_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer is not Complet!");

	}

	uint32_t OpenGLFramebuffer::SetColoarAtchments(const std::vector<Ref<Texture>>& colorAttachments, uint32_t openglTexTarget)
	{
		uint32_t texSize = colorAttachments.size();
		if (texSize == 0u)
			return 0u;

		uint32_t i = 0;
		for (const Ref<Texture>& texureObject : colorAttachments)
		{
			Ref<OpenGLTextureStorageModern> texutureStorage = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texureObject);
			CreateAttechmentTexture(texutureStorage, i);
			i++;
		}
		return i;
	}


	uint32_t OpenGLFramebuffer::SetupTextures()
	{
		uint32_t colorAttachmentsSize = 0;
		colorAttachmentsSize += SetColoarAtchments(m_ColorAttachmentsTex);
		if(m_DepthAttachment != nullptr)
			CreateAttechmentTexture(m_DepthAttachment, 0u);
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
		if (width == 0 || height == 0 || width > s_MaxFrambufferSize || height > s_MaxFrambufferSize)
		{
			RY_CORE_WARN("Faild Resize frambueffer to {0}, {1}", width, height);
			return;
		}
		m_Size = { width , height, m_Specification.Depth };
		m_Specification.Width = m_Size.x;
		m_Specification.Height = m_Size.y;
		m_Specification.Depth = m_Size.z;

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

	const glm::uvec3& OpenGLFramebuffer::GetFrambufferSize()
	{
		return m_Size;
	}

}

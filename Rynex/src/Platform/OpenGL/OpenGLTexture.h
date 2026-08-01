#pragma once

#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Memory/WeakPtrSet.h>

#include <Platform/OpenGL/OpenGLBase.h>


namespace Rynex {

#define RY_USE_PRENT_LINKE


	class OpenGLTextureStorageModern : public Texture
	{
	public:
		OpenGLTextureStorageModern(const TextureSpecification& spec);
		OpenGLTextureStorageModern(const TextureSpecification& spec, void* dataPtr, uint32_t size);
		OpenGLTextureStorageModern(const TextureSpecification& spec, std::vector<unsigned char>&& data);
		virtual ~OpenGLTextureStorageModern();


		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Object.GetDimension().withe; }
		virtual uint32_t GetHeight() const override { return m_Object.GetDimension().heigth; }
		virtual uint32_t GetDepth() const override { return m_Object.GetDimension().depth; }

		virtual uint32_t GetRenderID() const override { return m_RendererIDTex; }

		virtual void BindLessTex() override;
		virtual void UnBindLessTex() override;
		virtual uint64_t GetBindlesHandle()const override  { return m_BindlesHandle; }
		virtual bool IsBindLessTexActiv() const override { return m_BindlesActive; }

		virtual void SetData(void* dataPtr, uint32_t size) override;

		virtual const std::vector<unsigned char> GetCurrentRenderData() override;
		virtual const std::vector<unsigned char> GetCurrentRenderData(TextureFormat format) override;
		virtual const uint32_t GetCurrentRenderDataSize() const override { return m_Data.size(); };

		virtual void FreeCurrentData() override;

		virtual bool IsTransferd() override;
		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImage(Acces acces, uint32_t slot = 0) const override;
		virtual void UnBindImage(Acces acces, uint32_t slot = 0) const override;
		virtual void UnBind(uint32_t slot = 0) const override;

		virtual bool operator==(const Texture& other)const override
		{
			return m_RendererIDTex == ((OpenGLTextureStorageModern&)other).m_RendererIDTex;
		};

		virtual void Resize1D(uint32_t width) override;
		virtual void Resize2D(uint32_t width, uint32_t height) override;
		virtual void Resize3D(uint32_t width, uint32_t height, uint32_t depth) override;

		virtual void ClearImage();
		virtual void ClearImage(int data);
		virtual void ClearImage(const glm::vec3& data);
		virtual void ClearImage(const glm::vec4& data);
		virtual void ClearImage(const glm::ivec4& data);
		virtual void ClearImage(const glm::uvec4& data);

		virtual void ClearDepth();


		void SetSpecfication(const TextureSpecification& spec, OpenGLFramebuffer* frambufferPtr);
		void AddParent(OpenGLBindlesTextureArray* openGLBindlesTextureArrayPtr);
		void RemoveParent(OpenGLBindlesTextureArray* openGLBindlesTextureArrayPtr);

		void AddParent(OpenGLLinkedTextureArray* openGlLinkedTextureArrayPtr);
		void RemoveParent(OpenGLLinkedTextureArray* openGlLinkedTextureArrayPtr);

		void AddParent(OpenGLFramebuffer* openGlFramebufferPtr);
		void RemoveParent(OpenGLFramebuffer* openGlFramebufferPtr);

		void ChangedDataFromParent(OpenGLFramebuffer* openGlFramebufferPtr);
		void ChangedSpecifcationFromParent(OpenGLFramebuffer* openGlFramebufferPtr);

		void OnSpecifcationChangeCall();
		void OnDataChangeCall();
		void OnDestroyCall();

	private:
		void CreateID(uint32_t target);
		void DestroyID();
		void Invalidate();
		
		void SetupFance();
		void ClearLocaleDataStore();

		uint32_t GetByteSize() const;
		void CopyTexData(uint8_t* data, uint32_t size, uint32_t offset = 0);
	
		void SetupeSpecficationVaribels();
		

	
		uint32_t GetOpenGLTextureTarget() const;
		uint32_t GetOpenGLTextureDataFormate() const;
		bool CopyFromTextureToTexture(const Ref<OpenGLTextureStorageModern>& textureDst);
		bool CopyFromTextureToTexture(const Ref<OpenGLTextureStorageModern>& textureDst, const glm::uvec3& size);
		bool CopyFromTextureToTexture(const Ref<OpenGLTextureStorageModern>& textureDst, const glm::uvec3& size, const glm::uvec3& offsetSrc, const glm::uvec3& offsetDst);

	private:		
		std::vector<uint8_t> m_Data;
#ifdef RY_USE_PRENT_LINKE
		Memory::WeakPtrSet<OpenGLBindlesTextureArray, OpenGLLinkedTextureArray, OpenGLFramebuffer> m_ParentVec;
#elif 1
		std::vector<std::variant<Weak<OpenGLBindlesTextureArray>, Weak<OpenGLLinkedTextureArray>, Weak<OpenGLFramebuffer>>> m_ParentVec;
#endif
		OpenGLFence m_FanceObject;
		uint64_t m_BindlesHandle = 0ull;

		TextureSpecification m_Specification;
		OpenGLTextureObject m_Object;
		OpenGLTextureSampler m_Sampler;

		uint32_t m_RendererIDTex;
		uint32_t m_RendererIDSam;

		bool m_BindlesActive = false;
	private:
		friend OpenGLFramebuffer;
		friend OpenGLBindlesTextureArray;
		friend OpenGLLinkedTextureArray;
	};

#if 1
	class OpenGLLinkedTextureArray : public LinkedTextureArray
	{
	public:
		OpenGLLinkedTextureArray(const TextureSpecification& spec);
		virtual ~OpenGLLinkedTextureArray();

		virtual const TextureSpecification& GetVaildTextureSpecification() const override { return m_Specification; }

		virtual uint32_t GetVaildTextureWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetVaildTextureHeight() const override { return m_Specification.Height; }
		virtual uint32_t GetVaildTextureDepth() const override { return m_Specification.Depth; }

		virtual uint32_t GetRenderID() const override { return m_RendererIDTex; }

		virtual bool IsTransferd() override;

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImage(Acces acces, uint32_t slot = 0) const override;
		virtual void UnBindImage(Acces acces, uint32_t slot = 0) const override;
		virtual void UnBind(uint32_t slot = 0) const override;


		virtual void SetTextureToArray(int index, const Ref<Texture>& texture, bool instandGPUploade = false) override;
		virtual void SetTextureToArray(int index, std::nullptr_t, bool instandGPUploade = false) override;

		virtual const Ref<Texture>& GetTextureToArray(int index) override;

		virtual void ResizeTextureArray(int size, bool instandGPUploade = false) override;

		virtual const std::vector<Weak<Texture>>& GetTextureVec() const override;
		virtual uint32_t GetTextureCount() const override;
		virtual void ClearTextures() override;

		virtual bool IsDataRaydyOnGPU() const override;
		virtual void UpdateDataGPU() override;

		virtual void ResizeDimensions1D(uint32_t width) override;
		virtual void ResizeDimensions2D(uint32_t width, uint32_t height) override;
		virtual void ResizeDimensions3D(uint32_t width, uint32_t height, uint32_t depth) override;
		virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

		virtual bool operator==(const LinkedTextureArray& other) const override
		{
			return m_RendererIDTex == ((OpenGLLinkedTextureArray&)other).m_RendererIDTex;
		};

		void OnChildeSpecifcationChange(OpenGLTextureStorageModern* ptrTex);
		void OnChildeDataChange(OpenGLTextureStorageModern* ptrTex);
		void OnChildeDestroy(OpenGLTextureStorageModern* ptrTex);

		uint32_t GetOpenGLTextureTarget() const;
		uint32_t GetOpenGLTextureDataFormate() const;
		uint32_t GetPerTextureByteSize() const;

	private:
		bool HasTextureMoreThenOnce(const Ref<Texture>& texture);

		void SetupeSpecficationVaribels();
		void ClearLocaleDataStore();

		void SetupFance();
		void Invalidate();
		void CreateID(uint32_t target);
		void DestroyID();


		void ChangeTextureIndex(int index);
		void RemovedTextureIndexFromChangeVec(int index);
	private:
		std::vector<Weak<Texture>> m_LinkedTexturesVec;
		std::vector<int> m_ChangedTextureInidicesVec;
		std::vector<uint8_t> m_Data;

		OpenGLFence m_FanceObject;
		

		OpenGLTextureObject m_Object;
		OpenGLTextureSampler m_Sampler;

		TextureSpecification m_Specification;

		uint32_t m_TextureIndexDimension;
		uint32_t m_RendererIDTex;
		uint32_t m_RendererIDSam;
		
		bool m_DataUpdatedGPU;
	};
#endif
}


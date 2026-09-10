#pragma once
#include <Rynex/Renderer/API/BindlesTextureArray.h>
#include <Platform/OpenGL/OpenGLShaderStorageBuffer.h>

namespace Rynex {	
	class OpenGLTextureStorageModern;

	class OpenGLBindlesTextureArray : public BindlesTextureArray
	{
	public:
		OpenGLBindlesTextureArray();
		OpenGLBindlesTextureArray(uint32_t initCount);
		OpenGLBindlesTextureArray(std::vector<Ref<Texture>>&& tex);
		
		
		~OpenGLBindlesTextureArray();
	
		virtual bool IsTransferd() override;
		virtual void Bind(uint32_t slot = 0) override;
		virtual void UnBind(uint32_t slot) override;
		virtual void AktivateTextures() override;
		virtual void DeactivateTextures() override;

		virtual int AddTextures(const Ref<Texture>& texture) override;
		virtual int SubmitTextures(const Ref<Texture>& texture) override;
		virtual void FlushTextures() override;

		virtual void ClearTextures() override;

		virtual void EraseTexture(const Ref<Texture>& texture) override;
		virtual void SwapTexture(const Ref<Texture>& fromtexture, const Ref<Texture>& toTexture) override;
		virtual const MapVectorRef<Texture>& GetTextures() const override { return m_TexturesMap; }


		virtual uint32_t GetTexturesCount() const override { return m_Count; }

		virtual int GetStoredTextureIndex(const Ref<Texture>& texture)const override;
		virtual bool HasTextureStored(const Ref<Texture>& texture) const override;
		virtual bool HasTextureStoredOn(const Ref<Texture>& texture, int slot) const override;
		virtual const Ref<Texture>& GetTexture(int slot)const override;


		virtual void SetData(const void* data, uint32_t byteSize) override { RY_CORE_ASSERT(false, "Storage Buffer Not Implemted OpenGLBindlesTextureArray"); }
		virtual void SetData(const void* data, uint32_t offset, uint32_t byteSize) override { RY_CORE_ASSERT(false, "Storage Buffer Not Implemted OpenGLBindlesTextureArray"); }

		virtual void ResizeBuffer(uint32_t byteSize) override { RY_CORE_ASSERT(false, "Storage Buffer Not Implemted OpenGLBindlesTextureArray"); }
		virtual void ResizeBuffer(const void* data, uint32_t byteSize) override { RY_CORE_ASSERT(false, "Storage Buffer Not Implemted OpenGLBindlesTextureArray"); }
	
		virtual uint32_t GetByteSize() const override { return m_ByteSize; }
		virtual uint32_t GetTextureDataByteSize()const override;

		void OnChildeSpecifcationChange(OpenGLTextureStorageModern* ptrTex);
		void OnChildeDataChange(OpenGLTextureStorageModern* ptrTex);
		void OnChildeDestroy(OpenGLTextureStorageModern* ptrTex);

	private:
		void CreateID();
		void DestroyID();

		void InvalideData();
		int64_t GetKey(const Ref<Texture>& texture)const;
		int64_t GetKey(OpenGLTextureStorageModern* texture) const;

		int AddTextureVectorData(const Ref<Texture>& texture);
		int SubmiteTextureVectorData(const Ref<Texture>& texture);

		void AktivateBindlesTextures();
		void AktivateBindlesTexturesSafe();

		void DeactivateBindlesTextures();
		void DeactivateBindlesTexturesSafe();
		void LoadeBindlesHandles();

	private:
		static constexpr const uint32_t s_BufferFlag = GL_DYNAMIC_STORAGE_BIT;
		static constexpr const uint32_t s_Target = GL_SHADER_STORAGE_BUFFER;
	private:
		MapVectorRef<Texture> m_TexturesMap;
		OpenGLShaderStorageBuffer m_Buffer;

		uint32_t m_Count = 0u;
		uint32_t m_ByteSize = 0u;
		int m_LastBindeSlot = -1;
	private:
		friend OpenGLTextureStorageModern;


	};
}

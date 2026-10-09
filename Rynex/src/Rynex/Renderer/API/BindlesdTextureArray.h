#pragma once
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Core/MapVector.h>

namespace Rynex {

	class RYNEX_API BindlesTextureArray : public StorageBuffer
	{
	public:
		virtual ~BindlesTextureArray() {}
		

		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void UnBind(uint32_t slot) = 0;
		virtual void AktivateTextures() = 0;
		virtual void DeactivateTextures() = 0;

		virtual void ClearTextures() = 0;

		// Add Texture to the buffer and Create or Uploade the buffer to GPU Memory
		// return stored index from ssbo
		virtual int AddTextures(const Ref<Texture>& texture) = 0;

		// Submit Texture to the Texture Array and dont Update The Buffer Automicliy! 
		// return stored index from ssbo
		virtual int SubmitTextures(const Ref<Texture>& texture) = 0;

		// Create or Update the buffer to GPU Memory.
		// using together withe SubmitTextures or for the case a Texture has Changed in any way.
		virtual void FlushTextures() = 0;

		virtual void EraseTexture(const Ref<Texture>& texture) = 0;
		virtual void SwapTexture(const Ref<Texture>& fromtexture, const Ref<Texture>& toTexture) = 0;

		virtual const MapVectorRef<Texture>& GetTextures() const = 0;

		
		virtual uint32_t GetTexturesCount() const = 0;
		virtual const Ref<Texture>& GetTexture(int index)const = 0;
		virtual uint32_t GetTextureDataByteSize()const = 0;
		virtual bool HasTextureStored(const Ref<Texture>& texture) const = 0;
		virtual int GetStoredTextureIndex(const Ref<Texture>& texture)const = 0;
		virtual bool HasTextureStoredOn(const Ref<Texture>& texture, int slot) const = 0;
		
		static AssetType GetStaticTypeBindlesTextureArray() { return AssetType::BindlesArray; }
		AssetType GetType() const override { return GetStaticTypeBindlesTextureArray(); }

		static Ref<BindlesTextureArray> CreateBindlesTextureArray();
		static Ref<BindlesTextureArray> CreateBindlesTextureArray(uint32_t initCount);
		static Ref<BindlesTextureArray> CreateBindlesTextureArray(std::vector<Ref<Texture>>&& tex);
		static Ref<BindlesTextureArray> CreateBindlesTextureArray(std::initializer_list<Ref<Texture>> tex);
	};
}



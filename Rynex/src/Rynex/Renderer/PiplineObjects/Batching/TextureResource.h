#pragma once

#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Core/Range.h>

namespace Rynex {
	enum BatechedResource
	{
		BatechedResource_NoBatching = 0,
		BatechedResource_BindingRange = BIT(0),
		BatechedResource_Array = BIT(1),
		
		BatechedResource_ArrayRanges = BIT(2), // Multyble Resource types in one Array like Texture[Albedo[0], Normale[0],Albedo[1], Normale[1], ..., Albedo[n], Normale[n] ]
		BatechedResource_BindingRangeArray = BatechedResource_BindingRange | BatechedResource_Array,
		
		BatechedResource_Texture = BIT(3),
		BatechedResource_TextureArray = BIT(4),
		BatechedResource_BindlesTextures = BIT(5)
	};


	struct TextureTypeSetup
	{
		Range bindingRange;
		Range arrayRange;
		std::vector<Ref<Texture>> textureVec;

		TextureTypeSetup() = default;
		TextureTypeSetup(int firstBindSlot, int bindSlotCount = 1, int firstArrayIndex = 0, int arrayCount = 1)
			: bindingRange(firstBindSlot, firstBindSlot + bindSlotCount)
			, arrayRange(firstArrayIndex, firstArrayIndex + arrayCount)
		{
			ResizeFromRanges();
		}

		uint32_t GetMaxTextureCount() const
		{
			int bindingRangeCountTexture = bindingRange.GetCount();
			int arrayCountTexture = arrayRange.GetCount();
			int typeTextureCount = bindingRangeCountTexture + arrayCountTexture;
			return typeTextureCount;
		}

		void ResizeFromRanges()
		{
			textureVec.clear();
			int typeTextureCount = GetMaxTextureCount();
			textureVec.resize(typeTextureCount);
		}

		void SetSingleTextureBindSlot(int bindSlot)
		{
			constexpr int bindingSlotCount = 1;
			bindingRange = { bindSlot , bindSlot + bindingSlotCount };
			
			constexpr int firstArrayIndex = 0;
			constexpr int lastArrayIndex = 1;
			arrayRange = { firstArrayIndex , lastArrayIndex };

			ResizeFromRanges();
		}

		void SetTextureRange(int bindSlot, int bindingSlotCount)
		{
			bindingRange = { bindSlot , bindSlot + bindingSlotCount };

			constexpr int firstArrayIndex = 0;
			constexpr int lastArrayIndex = 1;
			arrayRange = { firstArrayIndex , lastArrayIndex };

			ResizeFromRanges();
		}

		void SetTextureArray(int bindSlot, int arrayCount, int firstArrayIndex = 0)
		{
			constexpr int bindingSlotCount = 1;
			bindingRange = { bindSlot , bindSlot + bindingSlotCount };
			arrayRange = { firstArrayIndex , firstArrayIndex + arrayCount };
			ResizeFromRanges();
		}

		bool operator== (const TextureTypeSetup& rigthe) const
		{
			bool bindRangeResult = this->bindingRange == rigthe.bindingRange;
			bool arrayResult = this->arrayRange == rigthe.arrayRange;
			return bindRangeResult && arrayResult;
		}

		bool operator!= (const TextureTypeSetup& rigthe) const
		{
			bool bindRangeResult = this->bindingRange != rigthe.bindingRange;
			bool arrayResult = this->bindingRange != rigthe.arrayRange;
			return bindRangeResult || arrayResult;
		}

		bool HasColision(const TextureTypeSetup& rigthe) const
		{
			return (this->bindingRange == rigthe.bindingRange && this->arrayRange.IsColiding(rigthe.arrayRange))
					|| this->bindingRange.IsColiding(rigthe.bindingRange);
		}

		bool HasRangeNoColsion(const TextureTypeSetup& rigthe) const
		{
			return (this->bindingRange <= rigthe.bindingRange || this->bindingRange >= rigthe.bindingRange)
				&& (this->arrayRange <= rigthe.arrayRange || this->arrayRange >= rigthe.arrayRange);
		}

		int AddTextureToType(const Ref<Texture>& texture)
		{
			int index = textureVec.size();
			int typeTextureCount = GetMaxTextureCount();

			if(index < typeTextureCount)
				textureVec.emplace_back(texture);
			else
				index = -1;
			return index;
		}
	};
	
	class TextureResource
	{
	public:
		virtual ~TextureResource() { }

		
		// Add Textures list to the all Texture types it is expeted that all texture types have the same size
		// return the index count from start of all textureTypes it schoud be the same other wise we return -1 and befor that we have a error break
		virtual int AddTexture(const std::vector<Ref<Texture>>& textureVec) = 0;

		// Add Textures list to the all Texture types it is expeted that all texture types have the same size
		// return the index count from start of all textureTypes it schoud be the same other wise we return -1 and befor that we have a error break
		virtual int AddTexture(const std::initializer_list<Ref<Texture>>& texturelist) = 0;

		// rerturn how many texture other alle textureType we curently have
		virtual uint32_t GetTextureCount() const = 0;

		// return we have no texture other alle textureType
		virtual bool Empty() const = 0;


		// reset the textures so every thing shoud be empty from outside or stored in place and check,
		// when submiting the textures again if somting is chaging
		virtual void Reset() = 0;

		// clears everything push every elment complet new even it is the same
		virtual void Clear() = 0;


		// rerturn how many textureType we set
		virtual int GetTextureTypeCount() const = 0;

		// Add a Texture to the textureType set if we store more then one katigury this is importent
		// return the index count from start of type we push this
		virtual int AddTexture(const Ref<Texture>& texture, int textureTypeIndex = 0) = 0;

		// rerturn how many texture other textureType we curently have
		virtual uint32_t GetTextureCountFromType(int textureTypeIndex = 0) const = 0;

		// return we have no texture in textureType
		virtual bool EmptyFromType(int textureTypeIndex = 0) const = 0;


		

		// Bind the list of Textures
		virtual void Bind() = 0;

		// Unbind the list of Textures
		virtual void Unbind() = 0;

		// if it maters like on Texture Array, you can only bind texture withe same formate and size
		// this funtion checks all textureTypes
		virtual bool IsTextureSizeVaild(const glm::uvec2& textureSize) const = 0;

	};
}


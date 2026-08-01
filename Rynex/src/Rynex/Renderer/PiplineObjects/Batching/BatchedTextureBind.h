#pragma once
#include <Rynex/Renderer/PiplineObjects/Batching/TextureResource.h>


namespace Rynex {
	class TextureResourceBindRange : public TextureResource
	{
	public:
		using BiningTextureRange = std::tuple<Range, std::vector<Ref<Texture>>>;
	public:
		TextureResourceBindRange(const std::initializer_list<Range>& list);
		TextureResourceBindRange(const std::vector<Range>& textureTypeVec);

		~TextureResourceBindRange();

		// Add Textures list to the all Texture types it is expeted that all texture types have the same size
		// return the index count from start of all textureTypes it schoud be the same other wise we return -1 and befor that we have a error break
		virtual int AddTexture(const std::vector<Ref<Texture>>& textureVec) override;

		// Add Textures list to the all Texture types it is expeted that all texture types have the same size
		// return the index count from start of all textureTypes it schoud be the same other wise we return -1 and befor that we have a error break
		virtual int AddTexture(const std::initializer_list<Ref<Texture>>& texturelist) override;

		// rerturn how many texture other alle textureType we curently have
		virtual uint32_t GetTextureCount() const override;

		// return we have no texture other alle textureType
		virtual bool Empty() const override;


		// reset the textures so every thing shoud be empty from outside or stored in place and check,
		// when submiting the textures again if somting is chaging
		virtual void Reset() override;

		// clears everything push every elment complet new even it is the same
		virtual void Clear() override;


		// rerturn how many textureType we set
		virtual int GetTextureTypeCount() const override;

		// Add a Texture to the textureType set if we store more then one katigury this is importent
		// return the index count from start of type we push this
		virtual int AddTexture(const Ref<Texture>& texture, int textureTypeIndex = 0) override;

		// rerturn how many texture other textureType we curently have
		virtual uint32_t GetTextureCountFromType(int textureTypeIndex = 0) const override;

		// return we have no texture in textureType
		virtual bool EmptyFromType(int textureTypeIndex = 0) const override;




		// Bind the list of Textures
		virtual void Bind() override;

		// Unbind the list of Textures
		virtual void Unbind() override;

		// if it maters like on Texture Array, you can only bind texture withe same formate and size
		// this funtion checks all textureTypes
		virtual bool IsTextureSizeVaild(const glm::uvec2& textureSize) const override;

	private:
		std::vector<BiningTextureRange> m_TextureTypeVec;
	};

	class TextureResourceBindles : TextureResource
	{
	public:
		using BidlesTextures = std::tuple<int, Ref<BindlesTextureArray>>;

	public:
		TextureResourceBindles(const std::initializer_list<int>& list);
		TextureResourceBindles(const std::vector<int>& textureTypeVec);

		~TextureResourceBindles();

		// Add Textures list to the all Texture types it is expeted that all texture types have the same size
		// return the index count from start of all textureTypes it schoud be the same other wise we return -1 and befor that we have a error break
		virtual int AddTexture(const std::vector<Ref<Texture>>& textureVec) override;

		// Add Textures list to the all Texture types it is expeted that all texture types have the same size
		// return the index count from start of all textureTypes it schoud be the same other wise we return -1 and befor that we have a error break
		virtual int AddTexture(const std::initializer_list<Ref<Texture>>& texturelist) override;

		// rerturn how many texture other alle textureType we curently have
		virtual uint32_t GetTextureCount() const override;

		// return we have no texture other alle textureType
		virtual bool Empty() const override;


		// reset the textures so every thing shoud be empty from outside or stored in place and check,
		// when submiting the textures again if somting is chaging
		virtual void Reset() override;

		// clears everything push every elment complet new even it is the same
		virtual void Clear() override;


		// rerturn how many textureType we set
		virtual int GetTextureTypeCount() const override;

		// Add a Texture to the textureType set if we store more then one katigury this is importent
		// return the index count from start of type we push this
		virtual int AddTexture(const Ref<Texture>& texture, int textureTypeIndex = 0) override;

		// rerturn how many texture other textureType we curently have
		virtual uint32_t GetTextureCountFromType(int textureTypeIndex = 0) const override;

		// return we have no texture in textureType
		virtual bool EmptyFromType(int textureTypeIndex = 0) const override;




		// Bind the list of Textures
		virtual void Bind() override;

		// Unbind the list of Textures
		virtual void Unbind() override;

		// if it maters like on Texture Array, you can only bind texture withe same formate and size
		// this funtion checks all textureTypes
		virtual bool IsTextureSizeVaild(const glm::uvec2& textureSize) const override;

	private:
		std::vector<BidlesTextures> m_TextureTypeVec;
	};
}

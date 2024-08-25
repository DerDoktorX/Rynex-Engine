#pragma once

#include <string>

#include "Rynex/Asset/Base/Asset.h"
#include <Rynex/Asset/Base/Buffer.h>

namespace Rynex {

	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA32F,

		RED_INTEGER,
		DEPTH24STENCIL8,
		Depth = DEPTH24STENCIL8,
	};

	struct TextureSpecification
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
		uint32_t Samples = 1;
	};

	class Texture : public Asset
	{
	public:
		static Ref<Texture> CreateInstance(TextureSpecification spec, uint32_t id);
		virtual ~Texture() = default;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeigth() const = 0;
		virtual uint32_t GetRenderID() const = 0;

		virtual const std::string& GetPath() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture& other)const = 0;

		static void BindTex(uint32_t renderID, uint32_t slot = 0);

		static AssetType GetStaticType() { return AssetType::Texture; }
		AssetType GetType() const { return GetStaticType(); }
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t withe, uint32_t height);
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(TextureSpecification spec , void* data);

		static AssetType GetStaticType() { return AssetType::Texture2D; }
		AssetType GetType() const { return GetStaticType(); }
	};

	
}

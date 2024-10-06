#pragma once
#include "Rynex/Renderer/API/Framebuffer.h"
#include "Rynex/Asset/Base/AssetMetadata.h"

namespace Rynex {
	class FramebufferImporter
	{
	public:
		static Ref<Framebuffer> ImportFramebuffer(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Framebuffer> LoadFramebuffer(const std::filesystem::path& path);
		static void ReLoadingFramebuffer(AssetHandle handle, const std::filesystem::path& path);
	};
}


#pragma once
#include "Rynex/Renderer/API/Framebuffer.h"
#include "Rynex/Asset/Base/AssetMetadata.h"

namespace Rynex {
	class FramebufferImporter
	{
	public:
		static Ref<Framebuffer> ImportFramebuffer(AssetHandle handle, const AssetMetadata& metadata, bool async);
		static Ref<Framebuffer> LoadFramebuffer(const std::filesystem::path& path, bool async);
		static void ReLoadingFramebuffer(AssetHandle handle, const std::filesystem::path& path, bool async);
	};
}


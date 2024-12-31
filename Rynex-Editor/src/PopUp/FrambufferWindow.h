#pragma once

#include <Rynex/Asset/EditorAssetManager.h>
#include <Rynex/Renderer/API/Framebuffer.h>

namespace Rynex {

	class FrambufferWindow
	{
	public:
		FrambufferWindow();
		~FrambufferWindow();

		void OnAtache();
		void OnDettache();
		void OnUpdate();

		void OnOpen(const std::filesystem::path& path);
		void OnClose();

		void OnImGuiRender();

		bool IsWindowOpen() { return m_Open; }
	private:

	private:
		bool m_Open = false;
		FramebufferSpecification m_FBspec;
		std::filesystem::path m_Path;
		char m_Name[64] = "FrameBufferDefaulte";

#if RY_EDITOR_ASSETMANGER_THREADE
		Ref<EditorAssetManegerThreade> m_AssetManger;
#else
		Ref<EditorAssetManager> m_AssetManger;
#endif

	};
}

#pragma once
#include <Rynex/Scene/Entity.h>
#include <Rynex/Scene/Scene.h>
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/EditorAssetManager.h>
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/API/Buffer.h>


namespace Rynex {

	class MeshWindow
	{
	public:
		MeshWindow();
		~MeshWindow();

		void OnAtache();
		void OnDettache();
		void OnUpdate();
		void OnImGuiRender();
		void OnEvent(Event& e);

		void OnOpen(AssetHandle handle);
		void OnClose();
	private:
		Ref<VertexBuffer> m_VertexBuffer;
		BufferLayout m_Layoute;
		std::vector<unsigned char> m_VertexData;
		std::vector<uint32_t> m_IndexData;
		Ref<Scene> m_WorkingScene;
		Ref<EditorCamera> m_EditorCamera;

		Entity m_EntityWorking;
		Entity m_Ground;
		Entity m_Grid;
		Ref<Framebuffer> m_FrameBuffer;
		glm::uvec2 m_ScreenSize = { 0, 0 };
		bool m_Open = false;
		bool m_ObjectWireFrame = false;
	};

}



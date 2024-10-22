#pragma once
#include "Rynex/Renderer/Camera/OrthograficCamera.h"
#include "Rynex/Renderer/Camera/Camera.h"
#include "Rynex/Renderer/Camera/EditorCamera.h"
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Scene/Components.h"

namespace Rynex {
	class RYNEX_API Renderer2D
	{
	public:
		

		static void InitEditor();
		static void Init();
		static void Shutdown();

		static void BeginSceneQuade(const EditorCamera& camera);
		static void BeginSceneQuade(const OrthograficCamera& camera);	//TODO: Remove 
		static void BeginSceneQuade(const Camera& camera, const glm::mat4& transform);
		static void EndSceneQuade();
		static void FlushQuade();

		static void DrawSprite(const glm::mat4& tranform, SpriteRendererComponent& src, int entityID);
		static void DrawQuad(const glm::mat4& tranform, const glm::vec4& color, int entityID = -2);
		static void DrawQuad(const glm::mat4& tranform, const Ref<Texture> texture, int entityID = -2);



		static void BeginSceneIcon(const Camera& camera, const glm::mat4& transform, const glm::uvec2& viewPortSize);
		static void EndSceneIcon();
		static void FlushIcon();

		static void DrawLigthPointIcon(const glm::mat4& tranform, int entityID = -2);
		static void DrawLigthSpotIcon(const glm::mat4& tranform,  int entityID = -2);
		static void DrawLigthDirctionelIcon(const glm::mat4& tranform,  int entityID = -2);
		static void DrawCameraIcon(const glm::mat4& tranform, int entityID = -2);
		static void DrawIcon(const glm::mat4& tranform, int index = 0, int entityID = -2);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t ChachSize = 0;
			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		
		static Statistics GetQuadeStats();
		static Statistics GetIconStats();
		
		static void ResetQuadeStats();
		static void ResetIconStats();
	private:
		static void StartNewBatchQuade();
		static void StartNewBatchIcon();
	};
}



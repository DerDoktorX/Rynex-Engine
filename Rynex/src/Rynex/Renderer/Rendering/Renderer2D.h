#pragma once
#include "Rynex/Renderer/Camera/OrthograficCamera.h"
#include "Rynex/Renderer/Camera/Camera.h"
#include "Rynex/Renderer/Camera/EditorCamera.h"
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Scene/Components.h"
#include <Rynex/Renderer/Text/Font.h>

namespace Rynex {
	struct TextParams
	{
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};

	class RYNEX_API Renderer2D
	{
	public:
		
		static void Init();
		static void InitEditor();

		static void Shutdown();
		static void ShutdownEditor();
		
		

		static void BeginSceneQuade(const EditorCamera& camera);
		static void BeginSceneQuade(const OrthograficCamera& camera);	//TODO: Remove 
		static void BeginSceneQuade(const Camera& camera, const glm::mat4& transform);
		static void EndSceneQuade();
		static void FlushQuade();

		static void DrawSprite(const glm::mat4& tranform, SpriteRendererComponent& src, int entityID);
		static void DrawQuad(const glm::mat4& tranform, const glm::vec4& color, int entityID = -2);
		static void DrawQuad(const glm::mat4& tranform, const Ref<Texture> texture, int entityID = -2);

		static void DrawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID);
		static void DrawString( const glm::mat4& transform, const TextComponent& textC, int entityID);
		static void DrawParticle(const glm::mat4& tranform, const glm::vec3& position, const glm::vec4& color, float size, float rotation, const Ref<Texture>& texture, int entityID);


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



#pragma once
#include "Rynex/Renderer/Camera/OrthograficCamera.h"
#include "Rynex/Renderer/Camera/Camera.h"
#include "Rynex/Renderer/Camera/EditorCamera.h"
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Scene/Components.h"

namespace Rynex {
	class Renderer2D
	{
	public:
		

		static void Init();
		static void Shutdown();

		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthograficCamera& camera);	//TODO: Remove 
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();
		static void Flush();
		static void FlushAndReset();

		//Entity

		static void DrawSprite(const glm::mat4& tranform, SpriteRendererComponent& src, int entityID);


		//Primitv
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D> texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D> texture);


		static void DrawQuad(const glm::mat4& tranform, const glm::vec4& color, int entityID = -2);
		static void DrawQuad(const glm::mat4& tranform, const Ref<Texture2D> texture, int entityID = -2);

		static void DrawQuadSingle(const glm::mat4& tranform, const glm::vec4& color, int entityID = -2);
		static void DrawQuadSingle(const glm::mat4& tranform, const Ref<Texture2D> texture, int entityID = -2);
		
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D> texture);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D> texture);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t ChachSize = 0;
			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		static void ResetStats();
		static Statistics GetStats();
	private:
		static void StartNewBatch();
	};
}



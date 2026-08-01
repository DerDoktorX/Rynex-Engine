#pragma once
#include <Rynex/Scene/ScenePrototyps.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Text/Font.h>

namespace Rynex {

	struct TextParams
	{
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};

	

	class Renderer2D
	{
	public:
		static void Init();
		static void InitEditor();
		static void Shutdown();
		static void ShutdownEditor();

		static uint32_t SubmitPass(const ViewPassData& viewPass);

		static void SubmitSpriteSingle(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);
		static void SubmitQuadSingle(const glm::mat4& transform, const Ref<Texture>& texture, int entityID);

		
		static void SubmitSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);
		static void SubmitQuad(const glm::mat4& transform, const glm::vec4& color, int entityID);
		static void SubmitQuad(const glm::mat4& transform, const Ref<Texture>& texture, int entityID);

		static void SubmitString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID);
		static void SubmitStringCom(const glm::mat4& transform, TextComponent& textC, int entityID);

		static void SubmitStringSingle(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID);
		static void SubmitStringComSingle(const glm::mat4& transform, TextComponent& textC, int entityID);


		static void SubmitLigthPointIcon(const glm::mat4& transform, int entityID = -2);
		static void SubmitLigthSpotIcon(const glm::mat4& transform, int entityID = -2);
		static void SubmitLigthDirctionelIcon(const glm::mat4& transform, int entityID = -2);
		static void SubmitCameraIcon(const glm::mat4& transform, int entityID = -2);
		static void SubmitIcon(const glm::mat4& transform, int index = 0, int entityID = -2);


		static void SubmitRenderDrawListQuad(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer);
		static void SubmitRenderDrawListText(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer);
		static void SubmitRenderDrawListIcon(RenderTarget& target, const Ref<UniformBuffer>& camerbuffer, const Ref<UniformBuffer>& displaybuffer);

		static void SubmitRenderDrawList();

		static void SubmitRenderDrawListViewPortSelected();


		static void DrawQuads();
		static void RenderText();
		static void RenderIcons();

	};

}

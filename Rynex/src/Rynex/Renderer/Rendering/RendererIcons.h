#pragma once
#include "Rynex/Renderer/API/Texture.h"
#include "Rynex/Renderer/Camera/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {
	class RendererIcons
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform, const glm::uvec2& sreenSize);
		static void EndScene();

		static void Flush();
		static void StartNewBatch();

		static void DrawQuad(const glm::mat4& tranform, const Ref<Texture>& texture, int entityID = -2);

		static void DrawLigthPointIcon(const glm::mat4& tranform, int entityID = -2);
		static void DrawLigthSpotIcon(const glm::mat4& tranform, int entityID = -2);
		static void DrawLigthDirctionelIcon(const glm::mat4& tranform, int entityID = -2);
		static void DrawCameraIcon(const glm::mat4& tranform, int entityID = -2);


	};
}


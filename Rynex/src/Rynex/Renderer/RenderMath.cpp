// RenderMath created in Project Rynex-Rendering on 01/11/2026.
#include <rypch.h>
#include "RenderMath.h"

#include <array>
#include <glm/gtx/transform.hpp>

namespace Rynex {
    glm::mat4 computePSMMatrix(const glm::mat4& cameraView, const glm::mat4& cameraProjection, const glm::vec3& lightDirWorld)
    {
        // 1. Matrix for world -> post-perspectivistic room
        glm::mat4 cameraMatrix = cameraProjection * cameraView;

        // 2. Directional light in post-perspectivistic room
        //    Directional light: Choose a point far away from the light direction.

        glm::vec3 lightPosWorld = glm::vec3(0.0f) - lightDirWorld * 1000.0f; // or camPos - lightDir * large
        glm::vec4 lightPosPost = cameraMatrix * glm::vec4(lightPosWorld, 1.0f);
        //    In case w < 0 (behind the camera), Handle signs separately (simplified here)
        constexpr float wMinValue = 0.0000001f;
        if (lightPosPost.w <= 0.0f)
            lightPosPost.w = wMinValue;

        lightPosPost /= lightPosPost.w; // perspectival Division -> NDC-Room

        // 3. Look directional: center off the Unit cube.
        glm::vec3 centerPost{0.0f, 0.0f, 0.0f };
		glm::vec3 upPost{0.0f, 1.0f, 0.0f};

		glm::mat4 lightViewPost = glm::lookAt(glm::vec3(lightPosPost), centerPost, upPost);

		// 4. All 8 corners from the Camera-NDC-Cube im post-perspective. Light room
		std::array<glm::vec4, 8> corners{
			glm::vec4{ -1.0f,-1.0f,-1.0f,1.0f },
            glm::vec4{  1.0f,-1.0f,-1.0f,1.0f },
            glm::vec4{ -1.0f, 1.0f,-1.0f,1.0f },
            glm::vec4{  1.0f, 1.0f,-1.0f,1.0f },
			glm::vec4{ -1.0f,-1.0f, 1.0f,1.0f },
            glm::vec4{  1.0f,-1.0f, 1.0f,1.0f },
            glm::vec4{ -1.0f, 1.0f, 1.0f,1.0f },
            glm::vec4{  1.0f, 1.0f, 1.0f,1.0f }
		};
		glm::vec3 minCorner{ std::numeric_limits<float>::max() };
		glm::vec3 maxCorner{ -std::numeric_limits<float>::max() };
		for (auto& c : corners) {
			glm::vec4 p = lightViewPost * c;
			p /= p.w;
			minCorner = glm::min(minCorner, glm::vec3(p));
			maxCorner = glm::max(maxCorner, glm::vec3(p));
		}

		// 5. Perspective Light Projection, for cover the [minCorner, maxCorner] area.
        constexpr float nearMaxValue = 0.001f;
		float nearL = std::max(nearMaxValue, -maxCorner.z);
		float farL = -minCorner.z;
		float left = minCorner.x;
		float right = maxCorner.x;
		float bottom = minCorner.y;
		float top = maxCorner.y;

		glm::mat4 lightProjPost = glm::frustum(left, right, bottom, top, nearL, farL);

		// 6. PSM-Matrix
		return lightProjPost * lightViewPost * cameraMatrix;
	}

    glm::mat4 Math::CalculateNormaleMatrix(const glm::mat4& transform)
    {
        const glm::mat4 transpose = glm::transpose(transform);
        const glm::mat4 transposeInverse = glm::inverse(transpose);
        return transposeInverse;
    }
}

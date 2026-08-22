#pragma once
// ============================================================================
//  SceneRenderCamera.h  –  Rynex Engine
//
//  Plain-data view of a camera filled by the caller (Scene / Editor) before
//  BeginScene().  The renderer is decoupled from concrete Camera types.
//
//  Improvement over original:
//   -> Added orthographic parameters (OrthoLeft/Right/Bottom/Top) so the
//     struct is complete for both projection types, rather than silently
//     ignoring them when IsOrthographic == true.
//   -> Added MakeOrthographicCameraData() factory to match the perspective one.
// ============================================================================

#include <rypch.h>

namespace Rynex {

    // -- Camera data – POD, no virtual functions -----------------------------------------------------------------------------
    struct RYNEX_API SceneRenderCameraData
    {
        // Matrices – caller must keep these consistent.
        glm::mat4   view = glm::mat4(1.0f);
        glm::mat4   projection = glm::mat4(1.0f);
        glm::mat4   viewProjection = glm::mat4(1.0f);

        glm::mat4   inverseView = glm::mat4(1.0f);
        glm::mat4   inverseProjection = glm::mat4(1.0f);
        glm::mat4   inverseViewProjection = glm::mat4(1.0f);

        // World-space orientation
        glm::vec3   position = glm::vec3(0.0f);
        glm::vec3   forward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3   up = glm::vec3(0.0f, 1.0f, 0.0f);

        // Shared clip planes
        float       nearClip = 0.01f;
        float       farClip = 1000.0f;

        // Perspective-specific (ignored when IsOrthographic == true)
        float       fov = glm::radians(45.0f);  // radians
        float       aspectRatio = 16.0f / 9.0f;

        // Orthographic-specific (ignored when IsOrthographic == false)
        float       orthoSize = 10.0f;    // half-extent; convenience alias
        float       orthoLeft = -10.0f;
        float       orthoRight = 10.0f;
        float       orthoBottom = -10.0f;
        float       orthoTop = 10.0f;

        bool        isOrthographic = false;
    };


    // -- Factories – compute all derived fields from raw inputs -----------------------------------------------------------------------------

    // Perspective camera from matrices + scalar parameters.
    RYNEX_API SceneRenderCameraData MakeSceneRenderCameraData(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& position,
        float            nearClip,
        float            farClip,
        float            fovRadians = glm::radians(45.0f),
        float            aspectRatio = 16.0f / 9.0f
    );

    // Orthographic camera – explicit frustum edges.
    RYNEX_API SceneRenderCameraData MakeOrthographicCameraData(
        const glm::mat4& view,
        float            left,
        float            right,
        float            bottom,
        float            top,
        float            nearClip,
        float            farClip,
        const glm::vec3& position
    );

    // Light-space camera for directional shadows (convenience wrapper).
    // direction must be normalised.
    RYNEX_API SceneRenderCameraData MakeLightSpaceCameraData(
        const glm::vec3& center,
        const glm::vec3& direction,
        float            orthoHalfExtent,
        float            nearClip,
        float            farClip
    );

} // namespace Rynex
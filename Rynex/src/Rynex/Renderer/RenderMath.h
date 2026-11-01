#pragma once
#include <glm/glm.hpp>


namespace Rynex::Math {
    /**
     * compute Perspective Shadow Matrix (PSM)
     * @param cameraView need camera view from the camera
     * @param cameraProjection need camera view from the camera
     * @param lightDirWorld need from the light source world direction
     * @return PSM-Matrix
     */
    static glm::mat4 computePSMMatrix(const glm::mat4& cameraView, const glm::mat4& cameraProjection, const glm::vec3& lightDirWorld);

    /**
     * Calculate from transform/model matrix -> normale Matrix.
     * @param transform
     * @return normale matrix
     */
    static glm::mat4 CalculateNormaleMatrix(const glm::mat4& transform);
}

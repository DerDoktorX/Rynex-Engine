#include "rypch.h"
#include "FustremCulling.h"


namespace Rynex {

    FustremCulling::FustremCulling()
    {
    }

    FustremCulling::~FustremCulling()
    {
    }

    void FustremCulling::UpdateFrustum(const glm::mat4& viewProjection)
	{
        const glm::mat4 vp = glm::transpose(viewProjection);

        for (int i = 0; i < 6; ++i) {
            glm::vec4 plane;
            if (i % 2 == 0) {
                plane = vp[3] - vp[i / 2];
            }
            else {
                plane = vp[3] + vp[i / 2];
            }

            float length = glm::length(glm::vec3(plane));
            planes[i].Normal = glm::vec3(plane) / length;
            planes[i].Distance = plane.w / length;
        }
	}

    bool FustremCulling::SphereInFrustum(const Sphere& sphere) const
    {
        for (const auto& plane : planes) {
            if (glm::dot(plane.Normal, sphere.Center) + plane.Distance < -sphere.Radius) {
                return false;
            }
        }
        return true;
    }

    bool FustremCulling::AABBInFrustum(const AABB& aabb) const
    {
        for (const auto& plane : planes) {
            glm::vec3 p = aabb.Min;
            if (plane.Normal.x >= 0) p.x = aabb.Max.x;
            if (plane.Normal.y >= 0) p.y = aabb.Max.y;
            if (plane.Normal.z >= 0) p.z = aabb.Max.z;

            if (glm::dot(plane.Normal, p) + plane.Distance < 0) {
                return false;
            }
        }
        return true;
    }
}
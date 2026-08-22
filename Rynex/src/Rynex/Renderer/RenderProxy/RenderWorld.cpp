#include "rypch.h"
#include "RenderWorld.h"

#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderCommand.h>

namespace Rynex {


    void OuadTree2D::GenerateNode()
    {
        m_RootNode = nullptr;
        m_NodesTree.clear();

        
    }
#if 0
    void OuadTree2D::CreateNode(Node& node)
    {
        uint32_t count = node.childrens.size();
        if (4 < count)
        {
            
            AABB& aabb = node.boxVolume;
            glm::vec3 differc = aabb.Max - aabb.Min;
            glm::vec3 differcHalf = differc / 2.0f;
            glm::vec3 center = aabb.Max - differcHalf;

#if 0
            std::array<glm::vec3, 8> array = {
                glm::vec3{ aabb.Max.x - xDiffercHalf, aabb.Max.y - yDiffercHalf , aabb.Max.z - zDiffercHalf },
                glm::vec3{ aabb.Max.x - xDiffercHalf, aabb.Max.y - yDiffercHalf , aabb.Max.z + zDiffercHalf },
                glm::vec3{ aabb.Max.x - xDiffercHalf, aabb.Max.y + yDiffercHalf , aabb.Max.z - zDiffercHalf },
                glm::vec3{ aabb.Max.x - xDiffercHalf, aabb.Max.y + yDiffercHalf , aabb.Max.z + zDiffercHalf },
                glm::vec3{ aabb.Max.x + xDiffercHalf, aabb.Max.y - yDiffercHalf , aabb.Max.z - zDiffercHalf },
                glm::vec3{ aabb.Max.x + xDiffercHalf, aabb.Max.y - yDiffercHalf , aabb.Max.z + zDiffercHalf },
                glm::vec3{ aabb.Max.x + xDiffercHalf, aabb.Max.y + yDiffercHalf , aabb.Max.z - zDiffercHalf },
                glm::vec3{ aabb.Max.x + xDiffercHalf, aabb.Max.y + yDiffercHalf , aabb.Max.z + zDiffercHalf }
            };
#else
            std::array< std::array<glm::vec3, 2>, 4> diffArray = {
                std::array<glm::vec3, 2>{ glm::vec3{  differcHalf.x, 0.0f,           0.0f }, glm::vec3{           0.0f, 0.0f, -differcHalf.z} },
                std::array<glm::vec3, 2>{ glm::vec3{  differcHalf.x, 0.0f,  differcHalf.z }, glm::vec3{           0.0f, 0.0f,           0.0f} },
                std::array<glm::vec3, 2>{ glm::vec3{           0.0f, 0.0f,           0.0f }, glm::vec3{ -differcHalf.x, 0.0f, -differcHalf.z} },
                std::array<glm::vec3, 2>{ glm::vec3{           0.0f, 0.0f,  differcHalf.z }, glm::vec3{ -differcHalf.x, 0.0f,           0.0f} }
            };
           
#endif
            uint32_t i = 0;
            for (Node*& nodePtr : node.childrens)
            {
                std::array<glm::vec3,2> border = diffArray.at(i);
                nodePtr->boxVolume = AABB{ center + border[0], center + border[1] };

                for ()
                {
                }
                i++;
            }

        }
    }
#endif
   


}
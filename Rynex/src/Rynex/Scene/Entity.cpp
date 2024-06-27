#include "rypch.h"
#include "Entity.h"

namespace Rynex {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}


#include "rypch.h"
#include "Entity.h"

namespace Rynex {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	Entity Entity::AddChildrenEntity(const std::string& name)
	{
#if RY_REALTION_SCHIP_ID_COMP
		UUID idParent = GetComponent<RealtionShipComponent>().FirstID;
		Entity childe = m_Scene->CreateEntity(name);

		RealtionShipComponent& childeRealtionshipC = childe.GetComponent<RealtionShipComponent>();
		RealtionShipComponent& parentRealtionshipC = GetComponent<RealtionShipComponent>();
		if (!idParent)
		{
			parentRealtionshipC.FirstID = childe.GetUUID();
			childeRealtionshipC.ParentID = GetUUID();
		}
		else
		{
			Entity otherChildsEntity = m_Scene->GetEntitiyByUUID(idParent);
			UUID nextChildren = otherChildsEntity.GetComponent<RealtionShipComponent>().NextID;

			while (nextChildren)
			{
				otherChildsEntity = m_Scene->GetEntitiyByUUID(nextChildren);
				nextChildren = otherChildsEntity.GetComponent<RealtionShipComponent>().NextID;
			}

			otherChildsEntity.GetComponent<RealtionShipComponent>().NextID = childe.GetUUID();
			childe.GetComponent<RealtionShipComponent>().PreviusID = otherChildsEntity.GetUUID();
			childeRealtionshipC.ParentID = GetUUID();

		}
#elif RY_REALTION_SCHIP_ARRAY_COMP
		RealtionShipComponent& rlsC = GetComponent<RealtionShipComponent>();

#endif

		return childe;
	}


	// Destroy not From Scene Regestriy only prepar childrens
	void Entity::DestroyEntity()
	{
#if RY_REALTION_SCHIP_ID_COMP
		RealtionShipComponent& parentRealtionshipC = GetComponent<RealtionShipComponent>();
		UUID nextChilde = parentRealtionshipC.FirstID;
		UUID newParentEnitity = parentRealtionshipC.ParentID;
		while (nextChilde)
		{
			Entity childeEntity = m_Scene->GetEntitiyByUUID(nextChilde);
			RealtionShipComponent& childeRealtionshipC = childeEntity.GetComponent<RealtionShipComponent>();
			RY_CORE_ASSERT(childeRealtionshipC.ParentID == GetUUID(), "is not SameParent!");
			childeRealtionshipC.ParentID = newParentEnitity;
			nextChilde = childeRealtionshipC.NextID;
		}
		Entity parent = m_Scene->GetEntitiyByUUID(newParentEnitity);
		if (newParentEnitity)
		{
			UUID nextCilde = parentRealtionshipC.NextID ? parentRealtionshipC.NextID : 0ull;
			UUID previusCilde = parentRealtionshipC.PreviusID ? parentRealtionshipC.PreviusID : 0ull;
			if (nextCilde)
			{
				Entity enitytNext = m_Scene->GetEntitiyByUUID(parentRealtionshipC.NextID);
				enitytNext.GetComponent<RealtionShipComponent>().PreviusID = previusCilde;
			}
			if (previusCilde)
			{
				Entity enitytPrivus = m_Scene->GetEntitiyByUUID(parentRealtionshipC.PreviusID);
				enitytPrivus.GetComponent<RealtionShipComponent>().NextID = nextCilde;
			}
			else
			{
				Entity enitytPrente = m_Scene->GetEntitiyByUUID(parentRealtionshipC.ParentID);
				enitytPrente.GetComponent<RealtionShipComponent>().FirstID = nextCilde;
			}
		}
#elif RY_REALTION_SCHIP_ARRAY_COMP
		RealtionShipComponent& rlsC = GetComponent<RealtionShipComponent>();
		for ( : rlsC.)
		{

		}
#endif
	}

	void Entity::DestroyEntityChildrens()
	{
#if RY_REALTION_SCHIP_ID_COMP
		RealtionShipComponent& parentRealtionshipC = GetComponent<RealtionShipComponent>();
		UUID nextChilde = parentRealtionshipC.FirstID;
		while (nextChilde)
		{
			Entity childeEntity = m_Scene->GetEntitiyByUUID(nextChilde);
			RealtionShipComponent& childeRealtionshipC = childeEntity.GetComponent<RealtionShipComponent>();
			RY_CORE_ASSERT(childeRealtionshipC.ParentID == GetUUID(), "is not SameParent!");

			UUID first = childeRealtionshipC.FirstID;
			if (first)
			{
				Entity firstChilde = m_Scene->GetEntitiyByUUID(nextChilde);
				firstChilde.DestroyEntityChildrens();
			}
			nextChilde = childeRealtionshipC.NextID;
			m_Scene->DestroyEntity(childeEntity);
		}
#elif RY_REALTION_SCHIP_ARRAY_COMP
		RealtionShipComponent& rlsC = GetComponent<RealtionShipComponent>();
#endif

	}

	void Entity::UpdateMatrix()
	{
#if RY_REALTION_SCHIP_ID_COMP

		TransformComponent& entityTransformC = GetComponent<TransformComponent>();
		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		RealtionShipComponent& parentRealtionshipC = GetComponent<RealtionShipComponent>();
		UUID nextChilde = parentRealtionshipC.FirstID;

		entityMatrix4x4C.Locale = entityTransformC.GetTransform();
		UUID parentEnitity = parentRealtionshipC.ParentID;
		if(parentEnitity)
		{
			Entity parent = m_Scene->GetEntitiyByUUID(parentEnitity);
			glm::mat4& parentMat =  parent.GetComponent<ModelMatrixComponent>().Globle;
			entityMatrix4x4C.Globle = parentMat * entityMatrix4x4C.Locale;
		}
		else
		{
			entityMatrix4x4C.Globle = entityMatrix4x4C.Locale;
		}

		if (HasComponent<ViewMatrixComponent>())
		{
			ViewMatrixComponent& view = GetComponent<ViewMatrixComponent>();
			view.Locale = glm::inverse(entityMatrix4x4C.Locale);
			view.Globle = glm::inverse(entityMatrix4x4C.Globle);
		}
		if (HasComponent<StaticMeshComponent>())
		{
			StaticMeshComponent& staticMesh = GetComponent<StaticMeshComponent>();
			
			for (auto& mesh : staticMesh.Meshes)
			{
				mesh.Globle = entityMatrix4x4C.Globle * mesh.Locale;
			}
		}
		while (nextChilde)
		{
			Entity childeEntity = m_Scene->GetEntitiyByUUID(nextChilde);
			RealtionShipComponent& childeRealtionshipC = childeEntity.GetComponent<RealtionShipComponent>();
			RY_CORE_ASSERT(childeRealtionshipC.ParentID == GetUUID(), "is not SameParent!");
			childeEntity.UpdateMatrix();
			UUID first = childeRealtionshipC.FirstID;
			if (first)
			{
				Entity firstChilde = m_Scene->GetEntitiyByUUID(nextChilde);
				firstChilde.UpdateMatrix();
			}
			nextChilde = childeRealtionshipC.NextID;
		}
#elif RY_REALTION_SCHIP_ARRAY_COMP
		RealtionShipComponent& rlsC = GetComponent<RealtionShipComponent>();
#endif
	}

}


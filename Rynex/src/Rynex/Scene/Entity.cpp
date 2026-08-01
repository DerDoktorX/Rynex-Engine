#include "rypch.h"
#include "Entity.h"

namespace Rynex {

	namespace Utils{
		static int ExtractIntegerWords(const std::string& str, uint32_t begin = 0)
		{
			std::stringstream sourche;
			std::string temp;
			int found = 0;
			
			while (!sourche.eof()) 
			{
				sourche >> temp;

				/* Checking the given word is integer or not */
				if (std::stringstream(temp) >> found)
					return found;

				/* To save from space at the end of string */
				temp = "";
			}
			return found;
		}
	}

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle)
	{
		if (nullptr != scene)
			m_Scene = Scene::GetRefInPlace(scene);
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
		Ref<Scene> scene = m_Scene.lock();


		Entity childe = scene->CreateEntity(name);
		UUID childID = childe.GetUUID();
		RealtionShipUUIDComponent& rlsCchild = childe.GetComponent<RealtionShipUUIDComponent>() ;
		rlsCchild.parent = GetUUID();
		std::vector<UUID>& childrens = GetChildrens();
		childrens.push_back(childID);

#endif
		
		return childe;
	}

	Entity Entity::CopyEntity()
	{
		Ref<Scene> scene = m_Scene.lock();

		std::string tagCopy = GetTagName();
		uint32_t index = tagCopy.find(" Copy");
		if (index < tagCopy.size())
		{
			std::string fromCopy = tagCopy.substr(index);
			int copyCount = Utils::ExtractIntegerWords(fromCopy);
			copyCount++;
			tagCopy = tagCopy.substr(0, index + 6);
			tagCopy += ' ' + std::to_string(copyCount);
		}
		else
		{
			tagCopy += " Copy";
		}
		Entity copy = scene->CreateEntity(tagCopy);
		copy.GetUUID();
		Scene::CopyComponentToEntity(copy, Entity(m_EntityHandle, scene.get()));
		RealtionShipUUIDComponent& realtionshipC = copy.GetComponent<RealtionShipUUIDComponent>();

		// copy.CopyChildres(Entity(m_EntityHandle, m_Scene));

		realtionshipC = RealtionShipUUIDComponent();
		return copy;
	}


	// Destroy not From Scene Regestriy only prepar childrens
	void Entity::DestroyEntity()
	{
		Ref<Scene> scene = m_Scene.lock();

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

		std::string name = GetTagName();
		Entity parent = GetParentEntity();
		UUID idPrarent = GetParentID();
		if (parent.IsVaild())
		{
			UUID id  = GetUUID();
			uint32_t index = 0;
			std::vector<UUID>& cildres = parent.GetChildrens();
			for (UUID& childe : cildres)
			{
				if (id == childe)
					break;
				index++;
			}
			auto it = cildres.begin();
			cildres.erase(it + index, it + index + 1);
		}

		std::vector<UUID>& childrens = GetChildrens();
		for (UUID idChild : childrens)
		{
			Entity childEntity = scene->GetEntitiyByUUID(idChild);
			RY_CORE_ASSERT(childEntity.IsVaild());
			childEntity.GetComponent<RealtionShipUUIDComponent>().parent = idPrarent;
		}
		
		if (HasComponent<ModelMangerComponent>())
			RemoveComponent<ModelMangerComponent>();
#endif
	}

	void Entity::DestroyEntityChildrens()
	{
		Ref<Scene> scene = m_Scene.lock();

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
		std::vector<UUID> cildresCopy = GetChildrens();
		for (auto& childe : cildresCopy)
		{
			Entity childeEntity = scene->GetEntitiyByUUID(childe);
			childeEntity.DestroyEntityChildrens();
			scene->DestroyEntity(childeEntity);

		}
		std::vector<UUID>& cildres = GetChildrens();
		uint32_t size = cildres.size();
		RY_CORE_ASSERT(size == 0);
		for (auto& childe : cildresCopy)
		{
			if(Entity childeEntity = scene->GetEntitiyByUUID(childe))
				std::string name = childeEntity.GetTagName();
		}
		
#endif

	}

	

	void Entity::CopyChildres(Entity e)
	{
		
		UUID parentID = GetUUID();
		
		std::vector<UUID> childrenCopy = e.GetChildrens();
		Ref<Scene> scene = m_Scene.lock();

		for (UUID childCopyID : childrenCopy)
		{
			Entity childCopyE = scene->GetEntitiyByUUID(childCopyID);
			if(childCopyE)
			{
				std::string nameCopy = childCopyE.GetTagName()+ " C";
				Entity childE = scene->CreateEntity(nameCopy);
				
				UUID childID = childE.GetUUID();
				Scene::CopyComponentToEntity(childE, childCopyE);

				RealtionShipUUIDComponent& rlsh_C = childE.GetComponent<RealtionShipUUIDComponent>();
				rlsh_C.childrens.clear();
				rlsh_C.parent = parentID;

				childE.CopyChildres(childCopyE);

				std::vector<UUID>& children = GetChildrens();
				children.emplace_back<UUID>(UUID{ childID });
				
				{
					std::vector<UUID> childrenChild = childE.GetChildrens();
					std::string nameChild = childE.GetTagName();
						for (auto& childe : childrenChild)
						{
							Entity childeEntity = scene->GetEntitiyByUUID(childe);
							std::string name = childeEntity.GetTagName();
						}
				}

			}
		}
		
	}

	

	void Entity::UpdateAutoMatrix()
	{
#if RY_REALTION_SCHIP_ID_COMP

		TransformComponent& entityTransformC = GetComponent<TransformComponent>();
		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		RealtionShipComponent& parentRealtionshipC = GetComponent<RealtionShipComponent>();
		UUID nextChilde = parentRealtionshipC.FirstID;

		
		UUID parentEnitity = parentRealtionshipC.ParentID;
		if (parentEnitity)
		{
			Entity parent = m_Scene->GetEntitiyByUUID(parentEnitity);
			glm::mat4& parentMat = parent.GetComponent<ModelMatrixComponent>().Globle;
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
		if (HasComponent<ModelMangerComponent>())
		{
			ModelMangerComponent& staticMesh = GetComponent<ModelMangerComponent>();
			staticMesh.UpdateMatrix(entityMatrix4x4C.Globle);

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
		UpadteTransformFromMatrix();

#ifdef COMPONENT_SINGLE_MODEL
		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		if (Entity parent = GetParentEntity())
		{
			glm::mat4& parentMat = parent.GetComponent<ModelMatrixComponent>().Globle;
			entityMatrix4x4C.Globle = parentMat * entityMatrix4x4C.Locale;
			UpdateComponent(entityMatrix4x4C);
		}
		else
		{
			entityMatrix4x4C.Globle = entityMatrix4x4C.Locale;
			UpdateComponent(entityMatrix4x4C);
		}


		if (HasComponent<ViewMatrixComponent>())
		{
			ViewMatrixComponent& view = GetComponent<ViewMatrixComponent>();
			view.Locale = glm::inverse(entityMatrix4x4C.Locale);
			view.Globle = glm::inverse(entityMatrix4x4C.Globle);
			UpdateComponent(view);
		}
#else
		ModelMatrixLocaleComponent& entityModelMatrixLocaleC = GetComponent<ModelMatrixLocaleComponent>();
		ModelMatrixGlobleComponent& entityModelMatrixGlobleC = GetComponent<ModelMatrixGlobleComponent>();
		if (Entity parent = GetParentEntity())
		{
			ModelMatrixGlobleComponent& parentModelMatrixGlobleC = parent.GetComponent<ModelMatrixGlobleComponent>();
			
			entityModelMatrixGlobleC.Set(parentModelMatrixGlobleC, entityModelMatrixLocaleC);
			UpdateComponent(entityModelMatrixGlobleC);
		}
		else
		{
			entityModelMatrixGlobleC.Set(entityModelMatrixLocaleC);
			UpdateComponent(entityModelMatrixGlobleC);
		}


		if (HasComponent<ViewMatrixComponent>())
		{
			ViewMatrixComponent& view = GetComponent<ViewMatrixComponent>();
			view.Locale = glm::inverse(entityModelMatrixLocaleC.Matrix);
			view.Globle = glm::inverse(entityModelMatrixGlobleC.Matrix);
			UpdateComponent(view);
		}
#endif

		if (HasComponent<ModelMangerComponent>())
		{
			ModelMangerComponent& staticMesh = GetComponent<ModelMangerComponent>();
			

#if 0
			ModelMangerComponent& staticMesh = GetComponent<ModelMangerComponent>();
			if (staticMesh.ModelR)
			{
				staticMesh.UpdateMatrix(entityMatrix4x4C.Globle);
				UpdateComponent(staticMesh);
			}
#endif

		}

		Ref<Scene> scene = m_Scene.lock();
		for (UUID& childID : GetChildrens())
		{
			Entity child = scene->GetEntitiyByUUID(childID);
			RY_CORE_ASSERT(child);

			child.UpdateAutoMatrix();
		}
#endif
	}

	

	void Entity::UpadteTransformFromMatrix()
	{
		TransformComponent& entityTransformC = GetComponent<TransformComponent>();
#ifdef COMPONENT_SINGLE_MODEL
		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		const glm::mat4& matrix = entityMatrix4x4C.Locale;
#else
		ModelMatrixLocaleComponent& entityModelMatrixLocaleC = GetComponent<ModelMatrixLocaleComponent>();
		const glm::mat4& matrix = entityModelMatrixLocaleC.Matrix;
#endif
		entityTransformC.SetTransform(matrix);

		UpdateComponent(entityTransformC);
	}

	void Entity::UpdateMatrixFromTransform()
	{
		TransformComponent& entityTransformC = GetComponent<TransformComponent>();
#ifdef COMPONENT_SINGLE_MODEL
		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		entityMatrix4x4C.Locale = entityTransformC.GetTransform();
		UpdateComponent(entityMatrix4x4C);
#else
		ModelMatrixLocaleComponent& entityModelMatrixLocaleC = GetComponent<ModelMatrixLocaleComponent>();
		entityModelMatrixLocaleC.Set(entityTransformC);
		UpdateComponent(entityModelMatrixLocaleC);
#endif

		
	}

	bool Entity::RemoveFromChildrens(Entity e)
	{
#ifdef RY_COMPONENT_RELATION_SHIPS_BASED_UUID
		UUID removeId = e.GetUUID();
		uint32_t i = 0u;

		std::vector<UUID>& childrens = GetChildrens();
		for (UUID& id : childrens)
		{
			if (id == removeId)
				break;
			i++;
		}
		if (i == childrens.size())
		{
			RY_CORE_WARN("Children not Found No Action!");
			return false;
		}
		std::vector<UUID>::iterator pos = childrens.begin() + i;
		childrens.erase(pos);
		RealtionShipUUIDComponent& realtionShipComp = e.GetComponent<RealtionShipUUIDComponent>();
		realtionShipComp.parent = UUID(0ull);
		return true;
#endif
	}

	bool Entity::AddToChildrens(Entity e)
	{
#ifdef RY_COMPONENT_RELATION_SHIPS_BASED_UUID
		UUID addId = e.GetUUID();
		uint32_t i = 0u;

		std::vector<UUID>& childrens = GetChildrens();
		for (UUID& id : childrens)
		{
			if (id == addId)
				break;
			i++;
		}
		if (i != childrens.size())
		{
			RY_CORE_WARN("Children alrady insigth! No Action");
			return false;
		}
		childrens.emplace_back(addId);

		RealtionShipUUIDComponent& realtionShipComp = e.GetComponent<RealtionShipUUIDComponent>();
		realtionShipComp.parent = this->GetUUID();
		return true;
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
		if (HasComponent<ModelMangerComponent>())
		{
			ModelMangerComponent& staticMesh = GetComponent<ModelMangerComponent>();
			if(staticMesh.ModelR)
				staticMesh.UpdateMatrix(entityMatrix4x4C.Globle);
			
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
		UpdateMatrixFromTransform();

#ifdef COMPONENT_SINGLE_MODEL

		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		
		if (Entity parent = GetParentEntity())
		{
			glm::mat4& parentMat = parent.GetComponent<ModelMatrixComponent>().Globle;
			entityMatrix4x4C.Globle = parentMat * entityMatrix4x4C.Locale;
			UpdateComponent(entityMatrix4x4C);
		}
		else
		{
			entityMatrix4x4C.Globle = entityMatrix4x4C.Locale;
			UpdateComponent(entityMatrix4x4C);
		}
#else

		ModelMatrixLocaleComponent& entityModelMatrixLocaleC = GetComponent<ModelMatrixLocaleComponent>();
		ModelMatrixGlobleComponent& entityModelMatrixGlobleC = GetComponent<ModelMatrixGlobleComponent>();

		if (Entity parent = GetParentEntity())
		{
			ModelMatrixGlobleComponent& parentModelMatrixGlobleC = parent.GetComponent<ModelMatrixGlobleComponent>();
			entityModelMatrixGlobleC.Set(parentModelMatrixGlobleC, entityModelMatrixLocaleC);
			UpdateComponent(entityModelMatrixGlobleC);
		}
		else
		{
			entityModelMatrixGlobleC.Set(entityModelMatrixLocaleC);
			UpdateComponent(entityModelMatrixGlobleC);
		}
#endif

		
		


		if (HasComponent<ViewMatrixComponent>())
		{
			ViewMatrixComponent& viewC = GetComponent<ViewMatrixComponent>();
#ifdef COMPONENT_SINGLE_MODEL
			viewC.Locale = glm::inverse(entityMatrix4x4C.Locale);
			viewC.Globle = glm::inverse(entityMatrix4x4C.Globle);
			UpdateComponent(viewC);
#else
			viewC.Locale = glm::inverse(entityModelMatrixLocaleC.Matrix);
			viewC.Globle = glm::inverse(entityModelMatrixGlobleC.Matrix);
			UpdateComponent(viewC);
#endif
			

		}
#if 0
		if (HasComponent<ModelMangerComponent>())
		{
			ModelMangerComponent& staticMeshC = GetComponent<ModelMangerComponent>();

			if (staticMesh.ModelR)
				staticMesh.UpdateMatrix(entityMatrix4x4C.Globle);
			UpdateComponent(staticMeshC);



		}
#endif

		Ref<Scene> scene = m_Scene.lock();
		for (UUID& childID : GetChildrens())
		{
			Entity child = scene->GetEntitiyByUUID(childID);
			RY_CORE_ASSERT(child);
			
			child.UpdateMatrix();
		}

#endif
	}

	void Entity::ExecuteOnChildrens(const std::function<void(Entity e)>& func)
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
			func(childeEntity);
			childeEntity.ExecuteOnChildrens(func);
		}
#elif RY_REALTION_SCHIP_ARRAY_COMP
		Ref<Scene> scene = m_Scene.lock();
		for (UUID& childID : GetChildrens())
		{
			Entity child = scene->GetEntitiyByUUID(childID);
			RY_CORE_ASSERT(child);
			func(child);
			child.ExecuteOnChildrens(func);
		}
#endif
	}

	const Scene *const Entity::GetScenePtr() const
	{	
		Ref<Scene> scene = m_Scene.lock();
		return scene.get();
	}

	Ref<Scene> Entity::GetScene() const
	{
		Ref<Scene> scene = m_Scene.lock();
		return scene;
	}

	
}


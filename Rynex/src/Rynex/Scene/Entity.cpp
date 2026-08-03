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
		Ref<Scene> scene = m_Scene.lock();


		Entity childe = scene->CreateEntity(name);
		UUID childID = childe.GetUUID();
		RealtionShipUUIDComponent& rlsCchild = childe.GetComponent<RealtionShipUUIDComponent>() ;
		rlsCchild.parent = GetUUID();
		std::vector<UUID>& childrens = GetChildrens();
		childrens.push_back(childID);
		
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


		realtionshipC = RealtionShipUUIDComponent();
		return copy;
	}


	// Destroy not From Scene Regestriy only prepar childrens
	void Entity::DestroyEntity()
	{
		Ref<Scene> scene = m_Scene.lock();

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
	}

	void Entity::DestroyEntityChildrens()
	{
		Ref<Scene> scene = m_Scene.lock();

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

		UpadteTransformFromMatrix();

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


		if (HasComponent<ModelMangerComponent>())
			ModelMangerComponent& staticMesh = GetComponent<ModelMangerComponent>();

		Ref<Scene> scene = m_Scene.lock();
		for (UUID& childID : GetChildrens())
		{
			Entity child = scene->GetEntitiyByUUID(childID);
			RY_CORE_ASSERT(child);

			child.UpdateAutoMatrix();
		}
	}

	

	void Entity::UpadteTransformFromMatrix()
	{
		TransformComponent& entityTransformC = GetComponent<TransformComponent>();
		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		const glm::mat4& matrix = entityMatrix4x4C.Locale;

		entityTransformC.SetTransform(matrix);

		UpdateComponent(entityTransformC);
	}

	void Entity::UpdateMatrixFromTransform()
	{
		TransformComponent& entityTransformC = GetComponent<TransformComponent>();
		ModelMatrixComponent& entityMatrix4x4C = GetComponent<ModelMatrixComponent>();
		entityMatrix4x4C.Locale = entityTransformC.GetTransform();
		UpdateComponent(entityMatrix4x4C);


		
	}

	bool Entity::RemoveFromChildrens(Entity e)
	{
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

	}

	bool Entity::AddToChildrens(Entity e)
	{
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
	}

	void Entity::UpdateMatrix()
	{
		UpdateMatrixFromTransform();
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
			ViewMatrixComponent& viewC = GetComponent<ViewMatrixComponent>();
			viewC.Locale = glm::inverse(entityMatrix4x4C.Locale);
			viewC.Globle = glm::inverse(entityMatrix4x4C.Globle);
			UpdateComponent(viewC);			

		}


		Ref<Scene> scene = m_Scene.lock();
		for (UUID& childID : GetChildrens())
		{
			Entity child = scene->GetEntitiyByUUID(childID);
			RY_CORE_ASSERT(child);
			
			child.UpdateMatrix();
		}

	}

	void Entity::ExecuteOnChildrens(const std::function<void(Entity e)>& func)
	{
		Ref<Scene> scene = m_Scene.lock();
		for (UUID& childID : GetChildrens())
		{
			Entity child = scene->GetEntitiyByUUID(childID);
			RY_CORE_ASSERT(child);
			func(child);
			child.ExecuteOnChildrens(func);
		}
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


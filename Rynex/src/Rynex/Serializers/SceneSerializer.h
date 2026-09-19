#pragma once


namespace Rynex {
	class Scene;
	class Entity;

	class RYNEX_API SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const FileSystem::Path& path);
		void SerializeRuntime(const FileSystem::Path& path);

		bool Deserialize(const FileSystem::Path& path);
		
		bool DeserializeRuntime(const FileSystem::Path& path);
	
	private:
		Ref<Scene> m_Scene;
	};

}



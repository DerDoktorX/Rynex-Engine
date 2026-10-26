#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>

#include <Rynex/Core/Application.h>
#include <Rynex/Core/LodePromis.h>
#include <Rynex/Project/Project.h>

namespace Rynex {


	struct AssetFindInfo
	{
		AssetHandle m_Handle;
		FileSystem::Path m_Path;
	    FileSystem::Path m_MarkedPath;
	    
		AssetFindInfo()
			: m_Handle(0ull)
			, m_Path()
	        , m_MarkedPath()
		{
		}

		AssetFindInfo(AssetHandle handle, const FileSystem::Path& path, const FileSystem::Path& markedPath)
			: m_Handle(handle)
			, m_Path(path)
		    , m_MarkedPath(markedPath)
		{
		}

        explicit operator bool() const
		{
			const bool result = AssetHandle::Zero() != m_Handle && !m_Path.IsEmpty();
			return result;
		}

	};
	class AssetManager
	{
	public:

		template<typename T>
		static Ref<T> GetAsset(const AssetHandle handle)
		{
		    const Ref<Project> project = Project::GetActive();
		    const Ref<AssetManagerBase> assetManger = project->GetAssetManger();
		    const Ref<Asset> asset = assetManger->GetAsset(handle);

			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static void GetAssetAsync(AssetHandle handle, Ref<T>* asset)
		{
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncProcess<T, AssetHandle, Ref<T>>, handle, asset);
			RY_SUBMIT_TASK_TO_ASYNC_THREED("get Asset " + std::to_string(handle), processFunc);

		}

		template<typename T>
		static void GetAssetAsync(AssetHandle handle, Weak<T>* asset)
		{
			const std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncProcess<T, AssetHandle, Weak<T>>, handle, asset);
			RY_SUBMIT_TASK_TO_ASYNC_THREED("get Asset " + std::to_string(handle), processFunc);
		}
		
		template<typename T, typename ...PromisArgs>
		static void GetAssetAsyncPromis(AssetHandle assetHandle, Ref<LodePromisType<T, PromisArgs ...>> lodePromis)
		{
			const std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncPromisProcessRef<T, AssetHandle, LodePromisType<T, PromisArgs ...>>, assetHandle, lodePromis);
			RY_SUBMIT_TASK_TO_ASYNC_THREED("get Asset " + std::to_string(assetHandle), processFunc);
		}


		

		template<typename T>
		static Ref<T> GetAsset(const FileSystem::Path& path)
		{
			
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(path);
			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static void GetAssetAsync(const FileSystem::Path& path, Ref<T>* asset)
		{
			*asset = nullptr;
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncProcess<T, FileSystem::Path, Ref<T>>, path, asset);
			RY_SUBMIT_TASK_TO_ASYNC_THREED("get Asset from Path " + path.GetPathString(), processFunc);
		}		
		
		template<typename T, typename ...PromisArgs>
		static void GetAssetAsyncPromis(const FileSystem::Path& path, Ref<LodePromisType<T, PromisArgs ...>> lodePromis)
		{
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncPromisProcessRef<T, FileSystem::Path, LodePromisType<T, PromisArgs ...>>, path, lodePromis);
			RY_SUBMIT_TASK_TO_ASYNC_THREED("get Asset from Path: " + path.GetPathString(), processFunc);
		}

		static std::filesystem::path GetMarkedAssetPath(AssetHandle handle)
		{
			Ref<Project> project = Project::GetActive();
			Ref<AssetManagerBase> assetManger = project->GetAssetManger();

			const AssetMetadata metaData = assetManger->GetMetadata(handle);
			if (!metaData.GetDisc())
				RY_CORE_WARN("Asset is Internal and has no File Path!");

			return metaData.m_PathMarker;
		}

		template<typename T>
		static std::filesystem::path GetMarkedAssetPath(const Ref<T>& asset)
		{
			AssetHandle handle = asset->m_Handle;
			std::filesystem::path markedtPath = GetMarkedAssetPath(handle);
			return markedtPath;
		}


		static AssetHandle GetAssetHandleMarkedPath(const std::string& pathMarkedStr)
		{
			const FileSystem::Path path = FileSystem::Path(pathMarkedStr);
			const Ref<AssetManagerBase> assetManger = GetAssetManagerBase();

			AssetHandle handle = assetManger->GetAssetHandle(path);
			return handle;
		}



		
		template<typename T>
		static Ref<T> FindAsset(const AssetFindInfo& info)
		{
			
			RY_CORE_ASSERT(info, "not enough information for 100% ID Asset!");


			Ref<AssetManagerBase> assetManger = GetAssetManagerBase();

			const AssetHandle handle = info.m_Handle;
			const AssetHandle handlePath = assetManger->GetAssetHandle(info.m_MarkedPath);

			Ref<T> asset = nullptr;


			if (assetManger->IsAssetHandleValid(handle) && handle == handlePath)
			{
				asset = AssetManager::GetAsset<T>(handle);
			}
			else if (assetManger->IsAssetHandleValid(handlePath))
			{
				asset = AssetManager::GetAsset<T>(handlePath);
			}
			else
			{
				RY_CORE_ASSERT(false, "The to many not valid states to Limited to safely ID The Asset!")
			}
			return asset;
		}


		static void FindAssetAsync(const AssetFindInfo& info, const std::function<void(AssetHandle)>& func)
		{
			
			RY_CORE_ASSERT(info, "not enough information for 100% ID Asset!");

		    const Ref<AssetManagerBase> assetManger = GetAssetManagerBase();

			const AssetHandle handle = info.m_Handle;
			const AssetHandle handlePath = assetManger->GetAssetHandle(info.m_Path);
			
			

			if (assetManger->IsAssetHandleValid(handle) && handle == handlePath)
			{
				func(handle);
			}
			else if (assetManger->IsAssetHandleValid(handlePath))
			{
				func(handlePath);
			}
			else
			{
				RY_CORE_ASSERT(false, "The to many not valid states to Limited to safely ID The Asset!");
			}
		}


		template<typename T>
		static AssetHandle CreatLocaleAsset(Ref<T> asset)
		{
			AssetMetadata metadata = AssetMetadata();
			metadata.m_Type = asset->GetType();
			Ref<Project> project = Project::GetActive();
			Ref<AssetManagerBase> assetManger = project->GetAssetManger();
			AssetHandle handle = Project::GetActive()->GetAssetManger()->CreatLocaleAsset(asset, metadata);
			return handle;
		}

		template<typename T>
		static Ref<T> GetLocaleAsset(AssetHandle handle)
		{
			RY_CORE_ASSERT(false, "This funktion is not any longer Sepoted! Use the funktion GetAsset");
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetLocaleAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static void DeleteLocaleAsset(AssetHandle handle)
		{
			Project::GetActive()->GetAssetManger()->DeleteLocaleAsset(handle);
		}

		static void Shutdown()
		{

		}
	private:
		template<typename T, typename N, typename PtrObject>
		static void GetAssetAsyncProcess(N identifier, PtrObject* assetLem)
		{
			const Ref<Asset> assetThread = Project::GetActive()->GetAssetManger()->GetAssetAsync(identifier);
			Ref<T> assetThreadRef = std::static_pointer_cast<T>(assetThread);
			*assetLem = assetThreadRef;
		}

		template<typename T, typename N, typename PromisLode>
		static void GetAssetAsyncPromisProcessRef(N identifier, Ref<PromisLode> lodePromis)
		{
			Ref<Project> project = Project::GetActive();
			Ref<AssetManagerBase> assetManger = project->GetAssetManger();

			Ref<Asset> assetThread = assetManger->GetAssetAsync(identifier);
			Ref<T> assetThreadRef = std::static_pointer_cast<T>(assetThread);
			lodePromis->OnLodingFinisht(assetThreadRef);
		}

		template<typename T, typename N, typename PromisLode>
		static void GetAssetAsyncPromisProcessWeak(N identifier, Weak<PromisLode> lodePromis)
		{

			Ref<Project> project = Project::GetActive();
			Ref<AssetManagerBase> assetManger = project->GetAssetManger();

			Ref<Asset> assetThread = assetManger->GetAssetAsync(identifier);
			Ref<T> assetThreadRef = std::static_pointer_cast<T>(assetThread);

			if(Ref<PromisLode> lodePromisRef = lodePromis.lock())
			{
				lodePromisRef->OnLodingFinisht(assetThreadRef);
			}

		}

	    static Ref<AssetManagerBase> GetAssetManagerBase()
		{
		    const Ref<Project> project = Project::GetActive();
		    Ref<AssetManagerBase> assetManger = project->GetAssetManger();
		    return assetManger;
		}

	};
	

}



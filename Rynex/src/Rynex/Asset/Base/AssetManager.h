#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Project/Project.h>
#include <Rynex/Core/LodePromis.h>

namespace Rynex {


	struct AssetFindeInfo
	{
		AssetHandle Handle;
		std::filesystem::path MarkedPath;
		std::filesystem::path Path;

		AssetFindeInfo()
			: Handle(0ull)
			, MarkedPath("")
			, Path("")
		{
		}

		AssetFindeInfo(AssetHandle handle, const std::filesystem::path& markedPath, const std::filesystem::path& path = "")
			: Handle(handle)
			, MarkedPath(markedPath)
			, Path(path)
		{
		}

		operator bool() const
		{
			bool result = Handle != 0ull && (MarkedPath != "" || Path != "");
			return result;
		}

	};
	class AssetManager
	{
	public:

		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			// Ref<Project> project = Project::GetActive();
			// auto assetManger = project->GetAssetManger();
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static void GetAssetAsync(AssetHandle handle, Ref<T>* asset)
		{
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncProcess<T, AssetHandle, Ref<T>>, handle, asset);
			RY_SUBMITE_TASK_TO_ASYNC_THREED("get Asset " + std::to_string(handle), processFunc);

		}

		template<typename T>
		static void GetAssetAsync(AssetHandle handle, Weak<T>* asset)
		{
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncProcess<T, AssetHandle, Weak<T>>, handle, asset);
			RY_SUBMITE_TASK_TO_ASYNC_THREED("get Asset " + std::to_string(handle), processFunc);
		}
		
		template<typename T, typename ...PromisArgs>
		static void GetAssetAsyncPromis(AssetHandle assetHandle, Ref<LodePromisType<T, PromisArgs ...>> lodePromis)
		{
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncPromisProcessRef<T, AssetHandle, LodePromisType<T, PromisArgs ...>>, assetHandle, lodePromis);
			RY_SUBMITE_TASK_TO_ASYNC_THREED("get Asset " + std::to_string(assetHandle), processFunc);
		}


		

		template<typename T>
		static Ref<T> GetAsset(const std::filesystem::path& path)
		{
			
#if RY_AKTIVATE_INTERAL_PATH
			RY_CORE_ASSERT(Project::HasSomeMarker(path))
#endif
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(path);
			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static void GetAssetAsync(const std::filesystem::path& path, Ref<T>* asset)
		{
			*asset = nullptr;
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncProcess<T, std::filesystem::path, Ref<T>>, path, asset);
			RY_SUBMITE_TASK_TO_ASYNC_THREED("get Asset from Path " + path.string(), processFunc);
		}		
		
		template<typename T, typename ...PromisArgs>
		static void GetAssetAsyncPromis(const std::filesystem::path& path, Ref<LodePromisType<T, PromisArgs ...>> lodePromis)
		{
			std::function<void()> processFunc = std::bind(&AssetManager::GetAssetAsyncPromisProcessRef<T, std::filesystem::path, LodePromisType<T, PromisArgs ...>>, path, lodePromis);
			RY_SUBMITE_TASK_TO_ASYNC_THREED("get Asset from Path " + path.string(), processFunc);
		}

		static std::filesystem::path GetMarkedAssetPath(AssetHandle handle)
		{
			Ref<Project> project = Project::GetActive();
			Ref<AssetManagerBase> assetManger = project->GetAssetManger();

			const AssetMetadata metaData = assetManger->GetMetadata(handle);
			if (!metaData.GetDisc())
				RY_CORE_WARN("Asset is Interal and has no File Path!");

			return metaData.PathMarker;
		}

		template<typename T>
		static std::filesystem::path GetMarkedAssetPath(const Ref<T>& asset)
		{
			AssetHandle handle = asset->Handle;
			std::filesystem::path markedtPath = GetMarkedAssetPath(handle);
			return metaData.PathMarker;
		}


		static AssetHandle GetAssetHandleMarkedPath(const std::filesystem::path& pathMarked)
		{
			std::string marker = Project::ExtraxtMarker(pathMarked);
			std::filesystem::path path = Project::RemoveMarker(pathMarked, marker);

			Ref<Project> project = Project::GetActive();
			Ref<AssetManagerBase> assetManger = project->GetAssetManger();

			AssetHandle handle = assetManger->GetAssetHandle(path);
			return handle;
		}

		template<typename T>
		static Ref<T> GetAssetMarkedPath(const std::filesystem::path& pathMarked)
		{
			AssetHandle handle = GetAssetHandleMarkedPath(path);
			Ref<T> asset = AssetManager::GetAsset<T>(handle);
			return asset;
		}

		
		template<typename T>
		static Ref<T> FindeAsset(const AssetFindeInfo& info)
		{
			
			RY_CORE_ASSERT(info, "not enough information for 100% ID Asset!");

			Ref<Project> project = Project::GetActive();
			Ref<EditorAssetManegerThreade> editorAssetManger = project->GetEditorAssetManger();

			AssetHandle handle = info.Handle;
			AssetHandle handlePath = editorAssetManger->GetAssetHandle(info.Path);
			AssetHandle handleMarkedPath = GetAssetHandleMarkedPath(info.MarkedPath);
			Ref<T> asset = nullptr;


			if (editorAssetManger->IsAssetHandleValid(handle) && (handle == handleMarkedPath || handle == handlePath))
			{
				asset = AssetManager::GetAsset<T>(handle);
			}
			else if (editorAssetManger->IsAssetHandleValid(handleMarkedPath))
			{
				asset = AssetManager::GetAsset<T>(handleMarkedPath);
			}
			else if (editorAssetManger->IsAssetHandleValid(handlePath))
			{
				asset = AssetManager::GetAsset<T>(handlePath);
			}
			else
			{
				RY_CORE_ASSERT(false,"The to many not vaild states to Limted to safely ID The Asset!")
			}
			return asset;
		}


		static void FindeAssetAsync(const AssetFindeInfo& info, std::function<void(AssetHandle)> func)
		{
			
			RY_CORE_ASSERT(info, "not enough information for 100% ID Asset!");

			Ref<Project> project = Project::GetActive();
			Ref<EditorAssetManegerThreade> editorAssetManger = project->GetEditorAssetManger();

			AssetHandle handle = info.Handle;
			AssetHandle handlePath = editorAssetManger->GetAssetHandle(info.Path);
			AssetHandle handleMarkedPath = GetAssetHandleMarkedPath(info.MarkedPath);
			
			

			if (editorAssetManger->IsAssetHandleValid(handle) && (handle == handleMarkedPath || handle == handlePath))
			{
				func(handle);
			}
			else if (editorAssetManger->IsAssetHandleValid(handleMarkedPath))
			{
				func(handleMarkedPath);
			}
			else if (editorAssetManger->IsAssetHandleValid(handlePath))
			{
				func(handlePath);
			}
			else
			{
				RY_CORE_ASSERT(false, "The to many not vaild states to Limted to safely ID The Asset!");
			}
		}


		template<typename T>
		static AssetHandle CreatLocaleAsset(Ref<T> asset)
		{
			AssetMetadata metadata = AssetMetadata();
			metadata.Type = asset->GetType();
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
			std::lock_guard<std::mutex> lock(s_WorkingThreadMutex);
			for (std::future<void>& thread : s_WorkingThread)
			{
				thread.wait();
			}
			s_WorkingThread.clear();
		}
	private:
		template<typename T, typename N, typename PtrObject>
		static void GetAssetAsyncProcess(N identifier, PtrObject* assetLem)
		{
			Ref<Asset> assetThread = Project::GetActive()->GetAssetManger()->GetAssetAsync(identifier);
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
	private:
		inline static std::mutex						s_WorkingThreadMutex;
		inline static std::vector<std::future<void>>	s_WorkingThread;
	};
	

}



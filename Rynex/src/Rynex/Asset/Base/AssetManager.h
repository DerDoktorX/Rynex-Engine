#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

// #include "Rynex/Core/Application.h"
#include "Rynex/Project/Project.h"

namespace Rynex {

	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			// Ref<Project> project = Project::GetActive();
			// auto assetManger = project->GetAssetManger();
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle, false);
			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static Ref<T> GetAsset(const std::filesystem::path& path)
		{
			// Ref<Project> project = Project::GetActive();
			// auto assetManger = project->GetAssetManger();
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(path);
			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static void GetAssetAsync(AssetHandle handle, Ref<T>* asset)
		{
#if 1
			std::lock_guard<std::mutex> lock(s_WorkingThreadMutex);
			// Ref<ThreadContext> threadContect = Application::Get().GetWindow().CreateThreadeContext();
			// s_WorkingThread.emplace_back(std::async(std::launch::async, [](AssetHandle handleLem, Ref<T>* assetLem, Ref<ThreadContext> threadContectLem)
			s_WorkingThread.emplace_back(std::async(std::launch::async, [](AssetHandle handleLem, Ref<T>* assetLem)
			{
				// threadContectLem->Init();
				Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handleLem, true);
				Ref<T> assetR = std::static_pointer_cast<T>(asset);
				// threadContectLem->Destroy();
				*assetLem = assetR;
			// }, handle, asset, threadContect));
			}, handle, asset));
#else
			std::promise<int> dataPromis;
			std::future<int> dataFuter = dataPromis.get_future();
			Ref<ThreadContext> threadContect = Application::Get().GetWindow().CreateThreadeContext();
			std::thread lodingThreade([](AssetHandle handleLem, Ref<T>* assetLem, Ref<ThreadContext> threadContectLem, std::promise<int>&& dataPromisLem)
				{
					threadContectLem->Init();
					Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handleLem);
					Ref<T> assetR = std::static_pointer_cast<T>(asset);
					threadContectLem->Destroy();
					*assetLem = assetR;
					dataPromisLem.set_value(1);
				}, handle, asset, threadContect, std::move(dataPromis));

			dataFuter.get();
			lodingThreade.join();
			

#endif
		}

		template<typename T>
		static void GetAssetAsync(AssetHandle handle, Weak<T>* asset)
		{
#if 1
			std::lock_guard<std::mutex> lock(s_WorkingThreadMutex);
			// Ref<ThreadContext> threadContect = Application::Get().GetWindow().CreateThreadeContext();
			// s_WorkingThread.emplace_back(std::async(std::launch::async, [](AssetHandle handleLem, Ref<T>* assetLem, Ref<ThreadContext> threadContectLem)
			s_WorkingThread.emplace_back(std::async(std::launch::async, [](AssetHandle handleLem, Weak<T>* assetLem)
				{
					// threadContectLem->Init();
					Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handleLem, true);
					Ref<T> assetR = std::static_pointer_cast<T>(asset);
					// threadContectLem->Destroy();
					*assetLem = assetR;
					// }, handle, asset, threadContect));
				}, handle, asset));
#else
			std::promise<int> dataPromis;
			std::future<int> dataFuter = dataPromis.get_future();
			Ref<ThreadContext> threadContect = Application::Get().GetWindow().CreateThreadeContext();
			std::thread lodingThreade([](AssetHandle handleLem, Ref<T>* assetLem, Ref<ThreadContext> threadContectLem, std::promise<int>&& dataPromisLem)
				{
					threadContectLem->Init();
					Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handleLem);
					Ref<T> assetR = std::static_pointer_cast<T>(asset);
					threadContectLem->Destroy();
					*assetLem = assetR;
					dataPromisLem.set_value(1);
				}, handle, asset, threadContect, std::move(dataPromis));

			dataFuter.get();
			lodingThreade.join();


#endif
		}

		template<typename T>
		static void GetAssetAsync(const std::filesystem::path& path, Ref<T>* asset)
		{
			std::lock_guard<std::mutex> lock(s_WorkingThreadMutex);
			// Ref<ThreadContext> threadContect = Application::Get().GetWindow().CreateThreadeContext();
			// s_WorkingThread.emplace_back(std::async(std::launch::async, [](const std::filesystem::path path, Ref<T>* assetLem, Ref<ThreadContext> threadContectLem)
			*asset = nullptr;
			s_WorkingThread.emplace_back(std::async(std::launch::async, [](const std::filesystem::path path, Ref<T>* assetLem)
			{
				// threadContectLem->Init();
				Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAssetAsync(path);
				Ref<T> assetR = std::static_pointer_cast<T>(asset);
				// threadContectLem->Destroy();
				*assetLem = assetR;
			}, path, asset));

			// }, path, asset, threadContect));
		}

		template<typename T>
		static AssetHandle CreatLocaleAsset(Ref<T> asset)
		{
			AssetMetadata metadata = AssetMetadata();
			metadata.Type = asset->GetType();
			Ref<Project> project = Project::GetActive();
			Ref<AssetManagerBase> assetManger = project->GetAssetManger();
			AssetHandle handle = Project::GetActive()->GetAssetManger()->CreatLocaleAsset(asset, metadata);
			// AssetHandle handle = assetManger->CreatLocaleAsset(asset, metadata);
			return handle;
		}

		template<typename T>
		static Ref<T> GetLocaleAsset(AssetHandle handle)
		{
			RY_CORE_ASSERT(false, "This funktion is not any longer Sepoted! Use the funktion GetAsset");
			// Ref<Project> project = Project::GetActive();
			// auto assetManger = project->GetAssetManger();
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetLocaleAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static void DeleteLocaleAsset(AssetHandle handle)
		{
			Project::GetActive()->GetAssetManger()->DeleteLocaleAsset(handle);
			// assetManger = project->;
			// assetManger;
		}

	private:
		inline static std::mutex						s_WorkingThreadMutex;
		inline static std::vector<std::future<void>>	s_WorkingThread;
	};
	

}



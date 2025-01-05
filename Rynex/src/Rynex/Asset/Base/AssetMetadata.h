#pragma once
#include "Asset.h"

namespace Rynex {

	enum class AssetState
	{
		None = 0,
		Loading,
		LostConection,	// Contion to File
		Updateing,
		Ready,			// Raedy to use
		Error,			// Every Error
		Uploading,		// Loading New Data on Disc
		NotLoaded,		// Not Needed
	};

	

	struct AssetMetadata
	{
		AssetType Type = AssetType::None;
		std::filesystem::path FilePath = "Not knowing Path";
		AssetState State = AssetState::None;
		std::string Name = std::string("Unkowne");				//FilenameDefault
		std::chrono::time_point<std::chrono::steady_clock> LoadingInTime = std::chrono::time_point<std::chrono::steady_clock>::min();
		std::vector<AssetHandle> ChildrenAssets;
		int Flags = 0;
		

		AssetMetadata() = default;
		AssetMetadata(const AssetMetadata&) = default;
	

		AssetMetadata(const AssetMetadata& metadat, AssetState state)
		{
			Type = metadat.Type;
			FilePath = metadat.FilePath;
			Name = metadat.Name;
			LoadingInTime = metadat.LoadingInTime;
			SetState(state);
		}	

		void SetChildeHandleMaxIndex(uint32_t maxIndex)
		{
			ChildrenAssets.reserve(maxIndex);
		}

		void AddChildeHandle(const AssetHandle handle)
		{
			ChildrenAssets.emplace_back(handle);
		}

		const AssetHandle GetChildeHandle(uint32_t index) const
		{
			return ChildrenAssets[index];
		}
		

		void SetState(AssetState state) 
		{
			State = state;
		}
		

		void SetActive(bool v)
		{
			if (v)
				Flags |= BIT(0);
			else
				Flags &= ~BIT(0);
		}
		bool GetActive() const
		{
			return BIT_EQUAL(Flags, BIT(0));
		}

		void SetIntern(bool v)
		{
			if (v)
				Flags |= BIT(1);
			else
				Flags &= ~BIT(1);
		}
		bool GetIntern() const
		{
			return BIT_EQUAL(Flags, BIT(1));
		}

		void SetDisc(bool v)
		{
			if (v)
				Flags |= BIT(2);
			else
				Flags &= ~BIT(2);
		}
		bool GetDisc() const
		{
			return BIT_EQUAL(Flags, BIT(2));
		}
		
		void SetParent(bool v)
		{
			if (v)
				Flags |= BIT(3);
			else
				Flags &= ~BIT(3);
		}
		bool GetParent() const
		{
			return BIT_EQUAL(Flags, BIT(3));
		}

		void SetChildren(bool v)
		{
			if (v)
				Flags |= BIT(4);
			else
				Flags &= ~BIT(4);
		}
		bool GetChildren() const
		{
			return BIT_EQUAL(Flags, BIT(4));
		}

		void SetScript(bool v)
		{
			if (v)
				Flags |= BIT(5);
			else
				Flags &= ~BIT(5);
		}
		bool GetScript() const
		{
			return BIT_EQUAL(Flags, BIT(5));
		}

		operator bool() const { return Type != AssetType::None && GetActive(); }

		
	};
	

}
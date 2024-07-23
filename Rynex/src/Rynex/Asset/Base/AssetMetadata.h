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
		std::filesystem::path FilePath;
		AssetState State = AssetState::None;
		std::string Name = std::string("");				//FilenameDefault
		std::filesystem::file_time_type LoadingTime;

		operator bool() const { return Type != AssetType::None; }

		AssetMetadata() = default;

		AssetMetadata(const AssetMetadata&) = default;

		AssetMetadata(const AssetMetadata& metadat, AssetState state)
		{
			Type = metadat.Type;
			FilePath = metadat.FilePath;
			Name = metadat.Name;
			LoadingTime = metadat.LoadingTime;
			SetState(state);
		}

		void SetState(AssetState state) 
		{
			State = state;
		}
	};
	

}
#pragma once
#include <Rynex/Asset/Base/Asset.h>


namespace Rynex {

	enum class AssetState
	{
		None = 0,
		Loading,
		LostConnection,	// Contention to File
		Updating,       // Updating
		Ready,			// Ready to use
		Error,			// Every Error
		Uploading,		// Loading New Data on Disc
		NotLoaded,		// Not Needed
	};

	

	struct AssetMetadata
	{
		AssetType m_Type = AssetType::None;
		std::filesystem::path m_FilePath = "Not knowing Path";
		AssetState m_State = AssetState::None;
		std::string m_Name = std::string("Unknown");				// FilenameDefault

		std::chrono::time_point<std::chrono::steady_clock> m_LoadingInTime = std::chrono::time_point<std::chrono::steady_clock>::min();
		std::vector<AssetHandle> m_ChildrenAssets;
		int m_Flags = 0;
		std::string m_ChangeTime = "0000-00-00 00:00:00";

		std::filesystem::path m_RelativePath = "Not knowing Relative Path";
		std::filesystem::path m_AbsolutePath = "Not knowing Absolute Path";
		std::string m_PathMarker = RY_PATH_NO_VALID_MARKER_STR "/Not knowing Path";
		FileSystem::Path m_Path;

		AssetMetadata() = default;
		AssetMetadata(const AssetMetadata&) = default;
	

		AssetMetadata(const AssetMetadata& metadat, AssetState state)
		{
			m_Type = metadat.m_Type;
			m_FilePath = metadat.m_FilePath;
			m_Name = metadat.m_Name;
			m_LoadingInTime = metadat.m_LoadingInTime;
			m_PathMarker = metadat.m_PathMarker;
			m_AbsolutePath = metadat.m_AbsolutePath;
			m_RelativePath = metadat.m_RelativePath;

			SetState(state);
		}	

		void SetFilePath(const std::filesystem::path& path);
		void SetFilePath(const FileSystem::Path& path);
		void SetMarkedFilePath(const std::string& markedPathStr);
		void SetMarkedFilePath(const std::string& markedPathStr, const std::filesystem::path& path);
		

		void SetChildeHandleMaxIndex(uint32_t maxIndex)
		{
			m_ChildrenAssets.reserve(maxIndex);
		}

		void AddChildeHandle(const AssetHandle handle)
		{
			m_ChildrenAssets.emplace_back(handle);
		}

		const AssetHandle GetChildeHandle(uint32_t index) const
		{
			return m_ChildrenAssets[index];
		}
		

		void SetState(AssetState state) 
		{

		    RY_CORE_TRACE("AssetMetadata state change from Asset ({}): {} -> {}", m_Name ,m_State, state);
			m_State = state;
		}

		void SetActive(bool v)
		{
			if (v)
				m_Flags |= BIT(0);
			else
				m_Flags &= ~BIT(0);
		}

		bool GetActive() const
		{
			return BIT_EQUAL(m_Flags, BIT(0));
		}

		void SetIntern(bool v)
		{
			if (v)
				m_Flags |= BIT(1);
			else
				m_Flags &= ~BIT(1);
		}

		bool GetIntern() const
		{
			return BIT_EQUAL(m_Flags, BIT(1));
		}

		void SetDisc(bool v)
		{
			if (v)
				m_Flags |= BIT(2);
			else
				m_Flags &= ~BIT(2);
		}

		bool GetDisc() const
		{
			return BIT_EQUAL(m_Flags, BIT(2));
		}
		
		void SetParent(bool v)
		{
			if (v)
				m_Flags |= BIT(3);
			else
				m_Flags &= ~BIT(3);
		}

		bool GetParent() const
		{
			return BIT_EQUAL(m_Flags, BIT(3));
		}

		void SetChildren(bool v)
		{
			if (v)
				m_Flags |= BIT(4);
			else
				m_Flags &= ~BIT(4);
		}

		bool GetChildren() const
		{
			return BIT_EQUAL(m_Flags, BIT(4));
		}

		void SetScript(bool v)
		{
			if (v)
				m_Flags |= BIT(5);
			else
				m_Flags &= ~BIT(5);
		}

		bool GetScript() const
		{
			return BIT_EQUAL(m_Flags, BIT(5));
		}

		operator bool() const 
		{ 
			bool isType = m_Type != AssetType::None;
			bool isActive = GetActive();
			bool isChangeTime = m_ChangeTime != "0000-00-00 00:00:00";
			return isType && isActive && isChangeTime;
		}

		
	};
	

}

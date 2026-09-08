#pragma once


#define RY_PATH_PROJECT_MARKER_STR "Project#!#"
#define RY_PATH_ENGINE_MARKER_STR "Engine#!#"
#define RY_PATH_NO_VALID_MARKER_STR "NotVaild!#"
#define RY_PATH_EXPECT_ENGINE_RELATIVE_START_FOlDER_STR "Editor-Assets"
#define RY_PATH_EXPECT_ENGINE_RELATIVE_START_STR "../Rynex-Editor/" RY_PATH_EXPECT_ENGINE_RELATIVE_START_FOlDER_STR

#define RY_PATH_PROJECT_MARKER_WSTR L"Project#!#"
#define RY_PATH_ENGINE_MARKER_WSTR L"Engine#!#"
#define RY_PATH_NO_VALID_MARKER_WSTR L"NotVaild!#"
#define RY_PATH_EXPECT_ENGINE_RELATIVE_START_WSTR L"../Rynex-Editor/Editor-Assets"


namespace Rynex::FileSystem {

	class Path
	{
	public:
		inline static constexpr int8_t PATH_MARKER_COUNT = 4; // path marker count
		inline static constexpr int8_t PATH_MARKER_INDEX = 2; // path valid count marker

		inline static constexpr const char* const PATH_MARKER_STR[PATH_MARKER_COUNT]={
			"",								// no marker at all
			RY_PATH_NO_VALID_MARKER_STR,	// not vaild marker 
			RY_PATH_ENGINE_MARKER_STR,		// engine marker 
			RY_PATH_PROJECT_MARKER_STR		// project marker 
		};
	// --- public enum class --------------------------------------------------------------------------------------------------
		enum class Origin : int8_t
		{
			None = 0,		// no marker at all
			Unknown = 1,	// not valid marker
			Engine = 2,		// engine marker 
			Project = 3		// project marker 
		};

		enum State : int16_t
		{
			None = 0,		// no known State
			Absolute = BIT(0),
			Relative = BIT(1),
			MarkedState = BIT(2),
			MarkedInline = BIT(3),
			MarkedUnknown = BIT(4),
			MarkedEngine = BIT(5),
			MarkedProject = BIT(6),
			Existing = BIT(7),
			Directory = BIT(8),
			File = BIT(9),
			Extension = BIT(10),
			AssetFileExtension = BIT(11),
		};
	// --- public methodes ----------------------------------------------------------------------------------------------------
		Path();
		Path(const Path& file);
		Path(const std::string& pathStr, Origin origne = Origin::None);
		Path(const std::string_view& pathView, Origin origne = Origin::None);
		Path(const std::filesystem::path& path, Origin origne = Origin::None);
		Path(const char* path, Origin origne = Origin::None);	

		~Path();

		bool IsEmpty() const;
		bool IsMarked() const;
		bool IsRelative() const;
		bool IsAbsolute() const;
		bool IsExisting() const;
		bool IsDirectory() const;
		bool IsFile() const;
		bool IsAssetExtension() const;
		uint64_t GetHash() const;

		int16_t GetState() const;

		Origin GetOrigin() const;
		

		const std::filesystem::path& GetPath() const; // get proteced acces to Orignal data
		[[nodiscard]] std::filesystem::path GetAbsolutePath() const;
		[[nodiscard]] std::filesystem::path GetRelativePath() const;
		[[nodiscard]] std::filesystem::path GetRelativePathFromOriginBase(Origin origin) const;
		[[nodiscard]] std::filesystem::path GetAbsoulteBasePath() const;
		[[nodiscard]] std::filesystem::path GetNamePath() const;
		[[nodiscard]] std::filesystem::path GetExtensionPath() const;

		[[nodiscard]] std::string GetAbsolutePathString() const;
		[[nodiscard]] std::string GetRelativePathString() const;
		[[nodiscard]] std::string GetMarkedPathString() const;
		[[nodiscard]] std::string GetRelativePathFromOriginBaseString(Origin origin) const;
		[[nodiscard]] std::string GetAbsoulteBasePathString() const;
		[[nodiscard]] std::string GetNamePathString() const;
		[[nodiscard]] std::string GetExtensionPathString() const;
		
		AssetType GetAssetFileType() const;
		

		void SetMarker(Origin origin);


		inline bool operator==(const Path& path) const
		{
			bool resultOrigin = m_Origin == path.m_Origin;
			bool resultPath = m_Path == path.m_Path;
			return resultPath && resultOrigin;
		}
		
		inline bool operator==(const std::filesystem::path& path) const
		{
			bool resultPath = m_Path == path;
			return resultPath;
		}

		inline bool operator==(const std::string& pathStr) const
		{
			bool resultPath = m_Path == pathStr;
			return resultPath;
		}

		inline bool operator==(const char* pathCharPtr) const
		{
			bool resultPath = m_Path == pathCharPtr;
			return resultPath;
		}


		inline bool operator!=(const Path& path) const
		{
			bool resultOrigin = m_Origin != path.m_Origin;
			bool resultPath = m_Path != path.m_Path;
			return resultPath || resultOrigin;
		}

		inline bool operator!=(const std::filesystem::path& path) const
		{
			bool resultPath = m_Path != path;
			return resultPath;
		}

		inline bool operator!=(const std::string& pathStr) const
		{
			bool resultPath = m_Path != pathStr;
			return resultPath;
		}

		inline bool operator!=(const char* pathCharPtr) const
		{
			bool resultPath = m_Path != pathCharPtr;
			return resultPath;
		}



		inline Path& operator=(const Path& path) 
		{
			m_Path = path.m_Path;
			return *this;
		}

		inline Path& operator=(const std::string& pathStr) 
		{
			auto [path, origin] = ConvertInternalPath(pathStr, m_Origin);
			m_Path = path;
			m_Origin = origin;
			return *this;
		}

		inline Path& operator=(const std::filesystem::path& pathStr) 
		{
			m_Path = pathStr;
			ConvertInternalPath();
			return *this;
		}

		inline Path& operator=(const char* pathChar)
		{
			std::string pathStr = pathChar;
			auto [path, origin] = ConvertInternalPath(pathChar, m_Origin);
			m_Path = path;
			m_Origin = origin;
			return *this;
		}
		
		inline std::filesystem::path& operator->()
		{
			return m_Path;
		}

		inline const std::filesystem::path& operator->() const
		{
			return m_Path;
		}

		inline Path& operator/=(const std::filesystem::path& path) 
		{
			m_Path /= path;
			ConvertInternalPath();
			return *this;
		}

		

		inline Path& operator/=(const std::string& path) 
		{
			m_Path /= path;
			ConvertInternalPath();
			return *this;
		}

		inline Path& operator/=(const char* path) 
		{
			m_Path /= path;
			ConvertInternalPath();
			return *this;
		}
	// --- public static methodes ---------------------------------------------------------------------------------------------
		static std::filesystem::path GetProjectDirectory();
		static std::filesystem::path GetEngineDirectory();
		static std::filesystem::path GetWorkingDirectory();
	
	protected:
		static void LogPathState(int16_t state);
		
		static int16_t FindMarkedPathInlineState(const std::string& pathStr, Origin origin);
		static int16_t FindFilesystemState(const std::filesystem::path& noInlineMarkedPath, Origin origin);

		static std::string GetResolveRelativePathToAbsoluteFromOrigin(const std::filesystem::path& path, Origin origin);
		static std::string GetResolveAbsoluteToMarkedPath(const std::filesystem::path& path, Origin origin);

		static inline int8_t GetOriginIndex(Origin origin) { return static_cast<int8_t>(origin); }
		static uint32_t GetMarkerCharacterCount(Origin origin);
		static bool IsPathMarked(const std::string& markedPathStr);
		static bool IsOriginPathMarked(Origin origin);

		static std::filesystem::path RemovePathMarker(const std::string& markedPathStr);
		static std::filesystem::path RemovePathMarker(const std::string& markedPathStr, Origin origin);

		static std::filesystem::path GetRelativePath(const std::string& markedPathStr);
		static std::filesystem::path GetRelativePath(const std::string& pathStr, const std::filesystem::path& pathBase);
		

		static std::filesystem::path GetPathAbsoluteOrigin(Origin origin);
		static Origin GetMarkedPathOrigin(const std::string& markedPathStr);
		static Origin GetPathOriginFromMarkerPath(const std::string& markedPathStr);
		static Origin GetExpectedOriginFromRelativePath(const std::filesystem::path& path);
		static Origin GetExpectedOriginFromAbsolutePath(const std::filesystem::path& path);

		static void ConvertAbsolutePath(std::filesystem::path& path, Origin origin);
		static std::tuple<std::filesystem::path, Origin> ConvertPathNoMarker(const std::string& pathStr, Origin origin);
		static void ConvertUniversalPath(std::filesystem::path& path);
		static void ConvertRealtivePathFromAbsolutePath(std::filesystem::path& relativePath, Origin origin);

		static std::tuple<std::filesystem::path, Origin> ConvertInternalPath(const std::string& pathStr, Origin origin);
	// --- private constexpr static methodes ----------------------------------------------------------------------------------
		inline static constexpr uint32_t GetMarkerLength(const char* const marker)
		{
			uint32_t count = 0;
			constexpr char stringEnd = 0;
			while (stringEnd != marker[count])
			{
				count++;
			}
			return count;
		}

		
		static size_t GetPathChartersCount(const char* pathPtr);
		static size_t GetPathChartersCount(const wchar_t* pathPtr);
		
	private:
		void ConvertInternalPath();


		std::filesystem::path GetResolveMarkerPath(Origin origin) const;
		std::filesystem::path GetResolveAbsoluteToMarkedPath(Origin origin) const;
		std::filesystem::path GetRelativePathFromAbsolutePath() const;

	// --- private varibles ---------------------------------------------------------------------------------------------------
		std::filesystem::path m_Path;
		Origin m_Origin;
	// --- private frinds -----------------------------------------------------------------------------------------------------
		friend bool operator==(const Path& a, const Path& b);
		friend bool operator!=(const Path& a, const Path& b);
		friend Path operator/(const Path& a, const Path& b);
	};

	

#pragma region NonMemberPathOperators

	inline bool operator == (const Path& a, const Path& b)
	{
		bool result = a.m_Path == b.m_Path;
		return result;
	}
	
	inline bool operator != (const Path& a, const Path& b) 
	{
		bool result = a.m_Path != b.m_Path;
		return result;
	}
	
	inline Path operator / (const Path& a, const Path& b) 
	{
		std::filesystem::path path = a.m_Path / b.m_Path;
		return Path(path);
	}

#pragma endregion

}

namespace robin_hood {


	template<>
	struct hash<Rynex::FileSystem::Path>
	{
		std::size_t operator()(const Rynex::FileSystem::Path& path) const
		{
			std::size_t hash = path.GetHash();
			return hash;
		}
	};
}

namespace std {
	template<>
	struct hash<Rynex::FileSystem::Path>
	{
		std::size_t operator()(const Rynex::FileSystem::Path& path) const
		{
			std::size_t hash = path.GetHash();
			return hash;
		}
	};
}

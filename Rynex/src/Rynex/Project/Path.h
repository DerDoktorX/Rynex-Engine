#pragma once


#define RY_PATH_PROJECT_MARKER_STR "Project#!#"
#define RY_PATH_ENGINE_MARKER_STR "Engine#!#"
#define RY_PATH_NO_VALID_MARKER_STR "NotVaild!#"
#define RY_PATH_EXPECT_ENGINE_RELATIVE_START_FOlDER_STR "Engine-Resources/"
#define RY_PATH_EXPECT_ENGINE_RELATIVE_START_STR "Engine-Resources/"

#define RY_PATH_PROJECT_MARKER_WSTR L"Project#!#"
#define RY_PATH_ENGINE_MARKER_WSTR L"Engine#!#"
#define RY_PATH_NO_VALID_MARKER_WSTR L"NotVaild!#"
#define RY_PATH_EXPECT_ENGINE_RELATIVE_START_WSTR L"Engine-Resources/Editor-Assets"


namespace Rynex::FileSystem {

    class Path
    {
    public:
        inline static constexpr int8_t PATH_MARKER_COUNT = 4; // path marker count
        inline static constexpr int8_t PATH_MARKER_INDEX = 2; // path valid count marker

        inline static constexpr const char* const PATH_MARKER_STR[PATH_MARKER_COUNT]={
            "",								// no marker at all
            RY_PATH_NO_VALID_MARKER_STR,	// not valid marker
            RY_PATH_ENGINE_MARKER_STR,		// engine marker
            RY_PATH_PROJECT_MARKER_STR		// project marker
        };
        // --- public enum class ----------------------------------------------------------------------------------------------
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
        // --- public methods -------------------------------------------------------------------------------------------------
        Path();
        Path(const Path& file);
        explicit Path(const std::string& pathStr, Origin origin = Origin::None);
        explicit Path(const std::string_view& pathView, Origin origin = Origin::None);
        explicit Path(const std::filesystem::path& path, Origin origin = Origin::None);
        explicit Path(const char* path, Origin origin = Origin::None);

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

        const std::filesystem::path& GetPath() const; // get protected access to Original data
        [[nodiscard]] Path GetParent() const;
        [[nodiscard]] std::filesystem::path GetAbsolutePath() const;
        [[nodiscard]] std::filesystem::path GetRelativePath() const;
        [[nodiscard]] std::filesystem::path GetRelativePathFromOriginBase(Origin origin) const;
        [[nodiscard]] std::filesystem::path GetAbsoluteBasePath() const;
        [[nodiscard]] std::filesystem::path GetNamePath() const;
        [[nodiscard]] std::filesystem::path GetExtensionPath() const;
        [[nodiscard]] std::filesystem::path GetParentPath() const;

        [[nodiscard]] std::string GetPathString() const;
        [[nodiscard]] std::string GetAbsolutePathString() const;
        [[nodiscard]] std::string GetRelativePathString() const;
        [[nodiscard]] std::string GetMarkedPathString() const;
        [[nodiscard]] std::string GetRelativePathFromOriginBaseString(Origin origin) const;
        [[nodiscard]] std::string GetAbsoluteBasePathString() const;
        [[nodiscard]] std::string GetNamePathString() const;
        [[nodiscard]] std::string GetExtensionPathString() const;
        [[nodiscard]] std::string GetParentPathString() const;
        AssetType GetAssetFileType() const;


        void SetMarker(Origin origin);
        void Clear();

        inline bool operator==(const Path& path) const
        {
            const bool resultOrigin = m_Origin == path.m_Origin;
            const bool resultPath = m_Path == path.m_Path;
            return resultPath && resultOrigin;
        }

        inline bool operator==(const std::filesystem::path& path) const
        {
            const bool resultPath = m_Path == path;
            return resultPath;
        }

        inline bool operator==(const std::string& pathStr) const
        {
            const bool resultPath = m_Path == pathStr;
            return resultPath;
        }

        inline bool operator==(const char* pathCharPtr) const
        {
            const bool resultPath = m_Path == pathCharPtr;
            return resultPath;
        }


        inline bool operator!=(const Path& path) const
        {
            const bool resultOrigin = m_Origin != path.m_Origin;
            const bool resultPath = m_Path != path.m_Path;
            return resultPath || resultOrigin;
        }

        inline bool operator!=(const std::filesystem::path& path) const
        {
            const bool resultPath = m_Path != path;
            return resultPath;
        }

        inline bool operator!=(const std::string& pathStr) const
        {
            const bool resultPath = m_Path != pathStr;
            return resultPath;
        }

        inline bool operator!=(const char* pathCharPtr) const
        {
            const bool resultPath = m_Path != pathCharPtr;
            return resultPath;
        }



        inline Path& operator=(const Path& path)
        {
            m_Path = path.m_Path;
            m_Origin = path.m_Origin;
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

        inline Path operator / (const std::filesystem::path& right) const
        {
            const std::filesystem::path path = m_Path / right;
            Path systemPath(path);
            return systemPath;
        }



        inline Path operator/(const std::string& right) const
        {
            const std::filesystem::path path = m_Path / right;
            Path systemPath(path);
            return systemPath;
        }

        inline Path operator/(const char* right) const
        {
            const std::filesystem::path path = m_Path / right;
            Path systemPath(path);
            return systemPath;
        }

        inline Path operator/(const Path& right) const
        {
            const std::filesystem::path path = m_Path / right.m_Path;
            Path systemPath(path);
            return systemPath;
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

        inline bool operator<(const Path& path) const
        {
            return m_Path < path.m_Path;
        }
        // --- public static methods ------------------------------------------------------------------------------------------
        static std::filesystem::path GetProjectDirectory();
        static std::filesystem::path GetEngineDirectory();
        static std::filesystem::path GetWorkingDirectory();

    private:
        static void LogPathState(int16_t state);

        static int16_t FindMarkedPathInlineState(const std::string& pathStr, Origin origin);
        static int16_t FindFilesystemState(const std::filesystem::path& noInlineMarkedPath, Origin origin);

        static std::string GetResolveRelativePathToAbsoluteFromOrigin(const std::filesystem::path& path, Origin origin);
        static std::string GetResolveAbsoluteToMarkedPath(const std::filesystem::path& path, Origin origin);

        static inline int8_t GetOriginIndex(Origin origin) { return static_cast<int8_t>(origin); }
        static uint32_t GetMarkerCharacterCount(Origin origin);
        static bool IsPathMarked(const std::string& markedPathStr);
        static bool IsOriginPathMarked(Origin origin);
        static bool IsStringInPath(const std::filesystem::path& path, const char* searchPtr);


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
        static void ConvertRelativePathFromAbsolutePath(std::filesystem::path& relativePath, Origin origin);

        static std::tuple<std::filesystem::path, Origin> ConvertInternalPath(const std::string& pathStr, Origin origin);
        // --- private constexpr static methods -------------------------------------------------------------------------------
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

        // --- private variables --------------------------------------------------------------------------------------------------
        std::filesystem::path m_Path;
        Origin m_Origin;
    };

}

namespace robin_hood {


	template<>
	struct hash<Rynex::FileSystem::Path>
	{
		[[nodiscard]] static std::size_t operator()(const Rynex::FileSystem::Path& path) noexcept
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
		[[nodiscard]] static std::size_t operator()(const Rynex::FileSystem::Path& path) noexcept
		{
			std::size_t hash = path.GetHash();
			return hash;
		}
	};
}

namespace fmt {
    template<>
    struct formatter<Rynex::FileSystem::Path> : formatter<std::filesystem::path>
    {
        context::iterator format(const Rynex::FileSystem::Path& value, format_context& ctx) const
        {
            const std::filesystem::path& path = value.GetPath();
            return formatter<std::filesystem::path>::format(path, ctx);
        }
    };
}
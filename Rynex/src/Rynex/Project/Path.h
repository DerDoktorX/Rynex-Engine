#pragma once


#define RY_PATH_PROJECT_MARKER_STR "Project#!#"
#define RY_PATH_ENGINE_MARKER_STR "Engine#!#"
#define RY_PATH_NO_VAILD_MARKER_STR "NotVaild!#"
#define RY_PATH_EXPEXT_ENGINE_RELATIV_START_STR "../Rynex-Editor/Editor-Assets"

#define RY_PATH_PROJECT_MARKER_WSTR L"Project#!#"
#define RY_PATH_ENGINE_MARKER_WSTR L"Engine#!#"
#define RY_PATH_NO_VAILD_MARKER_WSTR L"NotVaild!#"
#define RY_PATH_EXPEXT_ENGINE_RELATIV_START_WSTR L"../Rynex-Editor/Editor-Assets"


namespace Rynex::FileSystem {

	class Path
	{
	public:
		inline static constexpr int8_t PATH_MARKER_COUNT = 4; // path marker count
		inline static constexpr int8_t PATH_MARKER_INDEX = 2; // path valid count marker

		inline static constexpr const char* const PATH_MARKER_STR[PATH_MARKER_COUNT]={
			"",								// no marker at all
			RY_PATH_NO_VAILD_MARKER_STR,	// not vaild marker 
			RY_PATH_ENGINE_MARKER_STR,		// engine marker 
			RY_PATH_PROJECT_MARKER_STR		// project marker 
		};
	// --- public enum class --------------------------------------------------------------------------------------------------
		enum class Origne : int8_t
		{
			None = 0,		// no marker at all
			Unknown = 1,	// not vaild marker 
			Engine = 2,		// engine marker 
			Project = 3		// project marker 
		};

		enum State : int16_t
		{
			None = 0,		// no known State
			Absoulte = BIT(0),
			Realtive = BIT(1),
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

		Path(const Path& file);
		Path(const std::string& pathStr, Origne origne = Origne::None);
		Path(const std::string_view& pathView, Origne origne = Origne::None);
		Path(const std::filesystem::path& path, Origne origne = Origne::None);
		Path(const char* path, Origne origne = Origne::None);	

		~Path();

		bool IsMarked() const;
		bool IsRealtive() const;
		bool IsAbsoulte() const;
		bool IsExisting() const;
		bool IsDirectory() const;
		bool IsFile() const;
		bool IsAssetExtension() const;
		uint64_t GetHash() const;

		int16_t GetState() const;

		Origne GetMarkerOrigine() const;
		

		[[nodiscard]] const std::filesystem::path& GetPath() const; // get proteced acces to Orignal data
		[[nodiscard]] std::filesystem::path GetAbsolutePath() const;
		[[nodiscard]] std::filesystem::path GetRelativePath() const;
		[[nodiscard]] std::filesystem::path GetMarkedPath() const;

		[[nodiscard]]std::string GetAbsolutePathString() const;
		[[nodiscard]] std::string GetRelativePathString() const;
		[[nodiscard]] std::string GetMarkedPathString() const;

		void SetMarker(Origne origne);


		inline Path& operator=(const Path& path) 
		{
			m_Path = path.m_Path;
			return *this;
		}

		inline Path& operator=(const std::string& pathStr) 
		{
			m_Path = pathStr;
			ConvertInternalPath();
			return *this;
		}

		inline Path& operator=(const std::filesystem::path& pathStr) 
		{
			m_Path = pathStr;
			ConvertInternalPath();
			return *this;
		}

		inline Path& operator=(const char* pathCharcter) 
		{
			m_Path = pathCharcter;
			ConvertInternalPath();
			return *this;
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
		static std::filesystem::path GetProjectDiretory();
		static std::filesystem::path GetEngineDiretory();
		static std::filesystem::path GetWorkingDiretory();
	
	protected:
		static int16_t FindeMarkedPathInlineState(const std::filesystem::path& path, Origne origne);
		static int16_t FindeFilesystemStateState(const std::filesystem::path& noInlineMarkedPath, Origne origne);

		static std::filesystem::path GetResolveReltivePathToAbsoluteFromOrigne(const std::filesystem::path& path, Origne origne);
		static std::filesystem::path GetResolveAbsoluteToMarkedPath(const std::filesystem::path& path, Origne origne);

		static inline int8_t GetOrigneIndex(Origne origne) { return static_cast<int8_t>(origne); }
		static uint32_t GetMarkerCarkterCount(Origne origne);
		static bool IsPathMarked(const std::filesystem::path& markedPath);
		static bool IsOrignePathMarked(Origne origne);

		static std::filesystem::path RemovePathMarker(const std::filesystem::path& markedPath);
		static std::filesystem::path RemovePathMarker(const std::filesystem::path& markedPath, Origne origne);

		static std::filesystem::path GetReltivePath(const std::filesystem::path& path);
		static std::filesystem::path GetReltivePath(const std::filesystem::path& path, const std::filesystem::path& origene);

		static std::filesystem::path GetPathAbsoluteMarker(Origne origne);
		static Origne GetMarkedPathOrigine(const std::filesystem::path& markedPath);
		static Origne GetPathOrignFromMarkerPath(const std::filesystem::path& markedPath);
		static Origne GetExpextedOrigineFromRelativePath(const std::filesystem::path& path);
		static Origne GetExpextedOrigineFromAbsoultePath(const std::filesystem::path& path);


		static void ConvertUniverselPath(std::filesystem::path& path);
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
	private:
		void ConvertPathNoMarker();
		void ConvertAboultePath();

		void ConvertUniverselPath();
		void ConvertInternalPath();


		std::filesystem::path GetResolveMarkerPath(Origne origne) const;
		std::filesystem::path GetResolveAbsoluteToMarkedPath(Origne origne) const;
		std::filesystem::path ClearOringenMarker(Origne origne) const;
		std::filesystem::path GetRelativePathFromAbsoultePath() const;
	// --- private varibles ---------------------------------------------------------------------------------------------------
		std::filesystem::path m_Path;
		Origne m_Origne;
	// --- private frinds -----------------------------------------------------------------------------------------------------
		friend bool operator==(const Path& a, const Path& b);
		friend bool operator!=(const Path& a, const Path& b);
		friend Path operator/(const Path& a, const Path& b);
	};


#pragma region NoneMemberPathOperator

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

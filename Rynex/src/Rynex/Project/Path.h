#pragma once


#define RY_PATH_PROJECT_MARKER_STR "Project#!#"
#define RY_PATH_ENGINE_MARKER_STR "Engine#!#"
#define RY_PATH_NO_VAILD_MARKER_STR "#//#NotVaild#//#"
#define RY_PATH_EXPEXT_ENGINE_RELATIV_START_STR "../Rynex-Editor/Editor-Assets"

#define RY_PATH_PROJECT_MARKER_WSTR L"Project#!#"
#define RY_PATH_ENGINE_MARKER_WSTR L"Engine#!#"
#define RY_PATH_NO_VAILD_MARKER_WSTR L"#//#NotVaildMarker#//#"
#define RY_PATH_EXPEXT_ENGINE_RELATIV_START_WSTR L"../Rynex-Editor/Editor-Assets"


namespace Rynex {
	class Path
	{
	public:
		inline static constexpr int8_t PATH_MARKER_COUNT = 4; // path marker count
		inline static constexpr int8_t PATH_MARKER_INDEX = 2; // path valid count marker

		inline static constexpr const char* const Path::PATH_MARKER_STR[PATH_MARKER_COUNT]={
			"",								// no marker at all
			RY_PATH_NO_VAILD_MARKER_STR,	// not vaild marker 
			RY_PATH_ENGINE_MARKER_STR		// project marker 
			RY_PATH_PROJECT_MARKER_STR,		// engine marker 
		};
	// --- public enum class --------------------------------------------------------------------------------------------------

		enum class Origne : int8_t
		{
			None = 0,		// no marker at all
			Unknown = 1,	// not vaild marker 
			Engine = 2,		// engine marker 
			Project = 3		// project marker 
		};
	// --- public methodes ----------------------------------------------------------------------------------------------------

		Path(const Path& file);
		Path(const std::string& path);
		Path(const std::filesystem::path& path);
		Path(const char* path);		
		~Path();

		bool IsMarked() const;
		bool IsRealtive() const;
		bool IsAbsoulte() const;
		bool IsExisting() const;

		Origne GetMarkerOrigine() const;
		Origne GetExpextedMarkerOrigne() const;

		const std::filesystem::path& GetPath() const; // get proteced acces to Orignal data
		std::filesystem::path GetAbsolutePath() const;
		std::filesystem::path GetRelativePath() const;
		std::filesystem::path GetMarkedPath() const;

		std::filesystem::path GetRelativePathFromOrigne(Origne origne) const;

		void SetMarker(Origne origne);
		void GenertaMarker();
		void ClearOringenMarker();


		inline Path& operator=(const Path& path) {
			m_Path = path.m_Path;
			return *this;
		}

		inline Path& operator=(const std::string& pathStr) {
			m_Path = pathStr;
			ConvertUniverselPath();
			return *this;
		}

		inline Path& operator=(const std::filesystem::path& pathStr) {
			m_Path = pathStr;
			ConvertUniverselPath();
			return *this;
		}

		inline Path& operator=(const char* pathCharcter) {
			m_Path = pathCharcter;
			ConvertUniverselPath();
			return *this;
		}

		inline Path& operator/=(const std::filesystem::path& path) {
			m_Path /= path;
			ConvertUniverselPath();
			return *this;
		}

		inline Path& operator/=(const std::string& path) {
			m_Path /= path;
			ConvertUniverselPath();
			return *this;
		}

		inline Path& operator/=(const char* path) {
			m_Path /= path;
			ConvertUniverselPath();
			return *this;
		}
	// --- public static methodes ---------------------------------------------------------------------------------------------
		static std::filesystem::path GetProjectDiretory();
		static std::filesystem::path GetEngineDiretory();
		static std::filesystem::path GetWorkingDiretory();
	private:
		void ConvertUniverselPath();
		std::filesystem::path GetResolveMarkerPath(Origne origne) const;
		std::filesystem::path GetResolveAbsoluteToMarkedPath(Origne origne) const;
		std::filesystem::path ClearOringenMarker(Origne origne) const;

	// --- private static methodes --------------------------------------------------------------------------------------------
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
	// --- private varibles ---------------------------------------------------------------------------------------------------
		std::filesystem::path m_Path;
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

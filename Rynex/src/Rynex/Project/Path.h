#pragma once

namespace Rynex {
	class Path
	{
	public:
		enum class Origne
		{
			None = 0,
			Unknown = 1,
			Engine = 2,
			Project = 3,

		};
		// --- public methodes -------------------------------------------------------------------------------------------------------

		Path(const Path& file);
		Path(const std::string& path);
		Path(const std::filesystem::path& path);
		Path(const char* path);		~Path();

		bool IsMarker() const;
		bool IsRealtive() const;
		bool IsAbsoulte() const;
		bool IsExisting() const;

		Origne GetMarkerOrigine() const;
		Origne GetMarkerExpextedPath() const;

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
			return *this;
		}

	private:
		void ConvertUniverselPath();
		// --- private varibles ------------------------------------------------------------------------------------------------------
		std::filesystem::path m_Path;
		// --- private frinds -----------------------------------------------------------------------------------------------------
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(Path, bool, == , m_Path);
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(Path, bool, != , m_Path);

		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(Path, bool, < , m_Path);
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(Path, bool, <= , m_Path);

		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(Path, bool, > , m_Path);
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(Path, bool, >= , m_Path);

		friend Path operator/(const Path& a, const Path& b);
		friend Path operator/(const Path& a, const std::filesystem::path& b);
		friend Path operator/(const Path& a, const std::string& b);
		friend Path operator/(const Path& a, const char* b);
	};


	RY_NONE_MEBER_OPERATOR_BOOL(Path, == , &&, m_Path);
	RY_NONE_MEBER_OPERATOR_BOOL(Path, != , &&, m_Path);
	RY_NONE_MEBER_OPERATOR_BOOL(Path, < , &&, m_Path);
	RY_NONE_MEBER_OPERATOR_BOOL(Path, <= , &&, m_Path);
	RY_NONE_MEBER_OPERATOR_BOOL(Path, > , &&, m_Path);
	RY_NONE_MEBER_OPERATOR_BOOL(Path, >= , &&, m_Path);

	inline Path operator / (const Path& a, const Path& b) {
		std::filesystem::path path = a.m_Path / b.m_Path;
		return Path(path);
	}

	inline Path operator / (const Path& a, const std::filesystem::path& b) {
		std::filesystem::path path = a.m_Path / b;
		return Path(path);
	}

	inline Path operator / (const Path& a, const std::string& b) {
		std::filesystem::path path = a.m_Path / std::filesystem::path(b);
		return Path(path);
	}

	inline Path operator / (const Path& a, const char* b) {
		std::filesystem::path path = a.m_Path / std::filesystem::path(b);
		return Path(path);
	}

}

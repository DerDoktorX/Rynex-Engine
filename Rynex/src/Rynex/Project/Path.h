#pragma once

namespace Rynex {
	class Path
	{
	public:
		Path(const Path& file);
		Path(const std::string& path);
		Path(const std::filesystem::path& path);
		Path(const char* path);

		~Path();

		void SetMarker(const std::string& marker)
		{
			std::string pathStr = m_Path.string() + marker;
		}


	private:
		
		std::filesystem::path m_Path;
	};
}

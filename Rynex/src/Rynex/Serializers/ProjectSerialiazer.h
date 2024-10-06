#pragma once


namespace Rynex {
	class Project;

	class RYNEX_API ProjectSerialiazer
	{
	public:
		ProjectSerialiazer(Ref<Project> project);

		bool Serlize(const std::filesystem::path& filepath);
		bool Deserlize(const std::filesystem::path& filepath);
	
	private:
		Ref<Project> m_Project;
	};

}



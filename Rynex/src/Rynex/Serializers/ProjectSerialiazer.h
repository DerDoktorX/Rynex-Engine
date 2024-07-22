#pragma once

#include "Rynex/Project/Project.h"


namespace Rynex {

	class ProjectSerialiazer
	{
	public:
		ProjectSerialiazer(Ref<Project> project);

		bool Serlize(const std::filesystem::path& filepath);
		bool Deserlize(const std::filesystem::path& filepath);
	
	private:
		Ref<Project> m_Project;
	};

}



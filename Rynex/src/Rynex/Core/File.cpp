#include "rypch.h"
#include "File.h"
namespace Rynex {

	void File::ReadFile(std::string path, std::string& temp)
	{
		FILE* m_Stream;
		char buffer[255];
		int err;
		err = fopen_s(&m_Stream, path.c_str(), "r");

		if (m_Stream != NULL) 
		{

			while (fgets(buffer, 255, m_Stream) != NULL)
			{
				temp += buffer;
			}

			RY_CORE_INFO("Complet File Loade");
		}
		RY_CORE_ASSERT(m_Stream, "Coud not open File");
		err = fclose(m_Stream);
		if (err != 0) {
			int numclosed = _fcloseall();
			RY_CORE_WARN("Problem with close files, {0} files was still open!", numclosed);
		}
	}

	std::string File::ReadFile(std::string path)
	{
		return std::string();
	}

	void File::CheckState()
	{

	}
}

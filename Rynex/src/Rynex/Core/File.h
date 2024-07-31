#pragma once
#include <string>
#include "Rynex/Core/Base.h"
#include "Rynex/Renderer/API/Shader.h"

namespace Rynex {
	class File
	{
	public:
		static void ReadFile(std::string path, std::string& temp);
		static std::string ReadFile(std::string path);

		static void CheckState();

	private:
		uint32_t m_count = 0;
		std::unordered_map<std::string, std::string, Ref<Shader>> m_files;
	};
}
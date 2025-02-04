#pragma once

namespace Rynex {


	class TexturesImport
	{
	public:
		TexturesImport(uint32_t size);
		~TexturesImport();
		
		void Push_Back_Specular(const std::filesystem::path& specularPath);
		void Push_Back_Defuse(const std::filesystem::path& defusePath);

		void Emplace_Back_Specular(std::filesystem::path&& specularPath);
		void Emplace_Back_Defuse(std::filesystem::path&& defusePath);

		uint32_t Size();
		uint32_t AlecoeSize();
	private:
		std::filesystem::path* m_Base = nullptr;
		std::filesystem::path* m_Begin = nullptr;
		std::filesystem::path* m_Ende = nullptr;

		std::filesystem::path* m_Ptr = nullptr;

		std::filesystem::path* m_SpecularPtr = nullptr;
		std::filesystem::path* m_DefusePtr = nullptr;
		uint32_t m_AlecoeSize = 0;
		uint32_t m_Capacity = 0;
	};

}
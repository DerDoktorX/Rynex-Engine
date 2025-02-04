#include "rypch.h"
#include "TexturesImportData.h"

namespace Rynex {

	TexturesImport::TexturesImport(uint32_t size)
		: m_AlecoeSize(size)
	{

		m_Base = new std::filesystem::path[size];
		m_Begin = m_Base;
		m_Ende = m_Base;
		m_Ptr = m_Base;
		
	}

	TexturesImport::~TexturesImport()
	{
		delete[] m_Base;
	}

	void TexturesImport::Push_Back_Specular(const std::filesystem::path& specularPath)
	{
		if ((m_Capacity + 1) <= m_AlecoeSize)
		{

			m_Capacity++;
			m_Ende++;
			*m_Ende = specularPath;
			m_DefusePtr = m_Ende == nullptr ? m_Ende : m_DefusePtr;
		}
		

	}

	void TexturesImport::Push_Back_Defuse(const std::filesystem::path& defusePath)
	{
		if ((m_Capacity + 1) <= m_AlecoeSize)
		{
			
			m_Capacity++;
			m_Ende++;
			*m_Ende = defusePath;
			m_SpecularPtr = m_Ende == nullptr ? m_Ende : m_SpecularPtr;
		}
	}

	void TexturesImport::Emplace_Back_Specular(std::filesystem::path&& specularPath)
	{
		if ((m_Capacity + 1) <= m_AlecoeSize)
		{

			m_Capacity++;
			m_Ende++;
			*m_Ende = std::move(specularPath);
			m_DefusePtr = m_Ende == nullptr ? m_Ende : m_DefusePtr;
		}
	}

	void TexturesImport::Emplace_Back_Defuse(std::filesystem::path&& defusePath)
	{
		if ((m_Capacity + 1) <= m_AlecoeSize)
		{

			m_Capacity++;
			m_Ende++;
			*m_Ende = std::move(defusePath);
			m_SpecularPtr = m_Ende == nullptr ? m_Ende : m_SpecularPtr;
		}
	}

	uint32_t TexturesImport::Size()
	{
		return m_Capacity;
	}

	uint32_t TexturesImport::AlecoeSize() 
	{
		return m_AlecoeSize;
	}
}
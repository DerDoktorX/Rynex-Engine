#include "rypch.h"
#include "BatchedTextureBind.h"

#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/BindlesTextureArray.h>

namespace Rynex {

#pragma region TextureResourceBindRange

	TextureResourceBindRange::TextureResourceBindRange(const std::initializer_list<Range>& list)
		: m_TextureTypeVec()
	{
		m_TextureTypeVec.reserve(list.size());
		for (const Range& range : list)
		{
			m_TextureTypeVec.emplace_back(range, std::vector<Ref<Texture>>());
		}
	}

	TextureResourceBindRange::TextureResourceBindRange(const std::vector<Range>& textureTypeVec)
		: m_TextureTypeVec()
	{
		m_TextureTypeVec.reserve(textureTypeVec.size());
		for (const Range& range : textureTypeVec)
		{
			m_TextureTypeVec.emplace_back(range, std::vector<Ref<Texture>>());
		}
	}

	TextureResourceBindRange::~TextureResourceBindRange()
	{
		Clear();
	}

	int TextureResourceBindRange::AddTexture(const std::vector<Ref<Texture>>& textureVec)
	{
		RY_CORE_ASSERT(textureVec.size() == m_TextureTypeVec.size(), "Error not expexted textureType!");
		int indexLast = -2;
		int i = 0;
		for (const Ref<Texture>& texture : textureVec)
		{
			int index = AddTexture(texture, i);
			RY_CORE_ASSERT(index == indexLast || indexLast == -2, "The index types have difernzes in Count");
			i++;
		}
		return indexLast;
	}

	int TextureResourceBindRange::AddTexture(const std::initializer_list<Ref<Texture>>& texturelist)
	{
		return AddTexture(texturelist);
	}

	uint32_t TextureResourceBindRange::GetTextureCount() const
	{
		return m_TextureTypeVec.size();
	}

	bool TextureResourceBindRange::Empty() const
	{
		return m_TextureTypeVec.empty();
	}

	void TextureResourceBindRange::Reset()
	{
		for (auto& [range, textureVec] : m_TextureTypeVec)
		{
			textureVec.clear();
		}
	}

	void TextureResourceBindRange::Clear()
	{
		m_TextureTypeVec.clear();
	}

	int TextureResourceBindRange::GetTextureTypeCount() const
	{
		return m_TextureTypeVec.size();
	}

	int TextureResourceBindRange::AddTexture(const Ref<Texture>& texture, int textureTypeIndex) 
	{
		RY_CORE_ASSERT(textureTypeIndex < m_TextureTypeVec.size(), "vector overflow!");
		auto& [range, textureVec] = m_TextureTypeVec.at(textureTypeIndex);
		int index = textureVec.size();
		int typeTextureCount = range.GetCount();
		if(index < typeTextureCount)
		{
			textureVec.emplace_back(texture);
		}
		else
		{
			RY_CORE_ASSERT(false, "Coud not Add Texture, out side range!");
			index -= 1;
		}
		return index;
	}

	uint32_t TextureResourceBindRange::GetTextureCountFromType(int textureTypeIndex) const
	{
		RY_CORE_ASSERT(textureTypeIndex < m_TextureTypeVec.size(), "vector overflow!");
		const auto& [range, textureVec] = m_TextureTypeVec.at(textureTypeIndex);
		return range.GetCount();
	}

	bool TextureResourceBindRange::EmptyFromType(int textureTypeIndex) const
	{
		RY_CORE_ASSERT(textureTypeIndex < m_TextureTypeVec.size(), "vector overflow!");
		const auto& [range, textureVec] = m_TextureTypeVec.at(textureTypeIndex);
		return textureVec.empty();
	}

	void TextureResourceBindRange::Bind()
	{
		for (auto& [range, textureVec] : m_TextureTypeVec)
		{
			uint32_t bindingIndex = static_cast<uint32_t>(range.First);
			for (Ref<Texture>& texture : textureVec)
			{
				RY_CORE_ASSERT(texture != nullptr, "not expexted nullptr Ref!");
				RY_CORE_ASSERT(range.Last != bindingIndex, "out side defined binding Range!");

				texture->Bind(bindingIndex);
				bindingIndex++;
			}
		}
	}
	void TextureResourceBindRange::Unbind()
	{
		for (auto&[range, textureVec]  : m_TextureTypeVec)
		{
			uint32_t bindingIndex = static_cast<uint32_t>(range.First);
			for (Ref<Texture>& texture : textureVec)
			{
				RY_CORE_ASSERT(texture != nullptr, "not expexted nullptr Ref!");
				RY_CORE_ASSERT(range.Last != bindingIndex, "out side defined binding Range!");

				texture->UnBind(bindingIndex);
				bindingIndex++;
			}
		}
	}

	bool TextureResourceBindRange::IsTextureSizeVaild(const glm::uvec2& textureSize) const
	{
		RY_CORE_NOT_IMPL();
		RY_REMBER_FUNC_CHANGE("To decied to remove the funtion or to kep them!");
		return false;
	}


#pragma endregion

#pragma region TextureResourceBindles


	TextureResourceBindles::TextureResourceBindles(const std::initializer_list<int>& list)
		: m_TextureTypeVec()
	{
		m_TextureTypeVec.reserve(list.size());
		for (int binidniIndex : list)
		{
			m_TextureTypeVec.emplace_back(binidniIndex, Ref<BindlesTextureArray>(nullptr));
		}
	}

	TextureResourceBindles::TextureResourceBindles(const std::vector<int>& textureTypeVec)
		: m_TextureTypeVec()
	{
		m_TextureTypeVec.reserve(textureTypeVec.size());
		for (const int binidniIndex : textureTypeVec)
		{
			m_TextureTypeVec.emplace_back( binidniIndex, Ref<BindlesTextureArray>(nullptr));
		}

	}

	TextureResourceBindles::~TextureResourceBindles()
	{
		Clear();
	}

	int TextureResourceBindles::AddTexture(const std::vector<Ref<Texture>>& textureVec)
	{
		RY_CORE_ASSERT(textureVec.size() == m_TextureTypeVec.size(), "Error not expexted textureType!");
		int indexLast = -2;
		int i = 0;
		for (const Ref<Texture>& texture : textureVec)
		{
			int index = AddTexture(texture, i);
			RY_CORE_ASSERT(index == indexLast || indexLast == -2, "The index types have difernzes in Count");
			i++;
		}
		return indexLast;
	}

	int TextureResourceBindles::AddTexture(const std::initializer_list<Ref<Texture>>& texturelist)
	{
		return AddTexture(texturelist);
	}

	uint32_t TextureResourceBindles::GetTextureCount() const
	{
		return m_TextureTypeVec.size();
	}

	bool TextureResourceBindles::Empty() const
	{
		return m_TextureTypeVec.empty();
	}

	void TextureResourceBindles::Reset()
	{
		for (auto& [bindingIndex, bindlesTextureArray] : m_TextureTypeVec)
		{
			if (bindlesTextureArray != nullptr)
			{
				bindlesTextureArray->ClearTextures();
			}
		}
	}

	void TextureResourceBindles::Clear()
	{
		m_TextureTypeVec.clear();
	}

	int TextureResourceBindles::GetTextureTypeCount() const
	{
		return m_TextureTypeVec.size();
	}

	int TextureResourceBindles::AddTexture(const Ref<Texture>& texture, int textureTypeIndex)
	{
		RY_CORE_ASSERT(textureTypeIndex < m_TextureTypeVec.size(), "vector overflow!");
		auto& [bindingIndex, bindlesTextureArray] = m_TextureTypeVec.at(textureTypeIndex);
		int index = -1;
		if (bindlesTextureArray == nullptr)
		{
			bindlesTextureArray = BindlesTextureArray::CreateBindlesTextureArray({ texture });
			index = bindlesTextureArray->GetStoredTextureIndex(texture);
		}
		else
		{
			RY_REMBER_FUNC_CHANGE("Maybe change funktion to have use SubmiteTexture instead of AddTextures!");
			index = bindlesTextureArray->AddTextures(texture);
		}
		return index;
	}

	uint32_t TextureResourceBindles::GetTextureCountFromType(int textureTypeIndex) const
	{
		RY_CORE_ASSERT(textureTypeIndex < m_TextureTypeVec.size(), "vector overflow!");
		const auto& [bindingIndex, bindlesTextureArray] = m_TextureTypeVec.at(textureTypeIndex);
		uint32_t count = 0u;
		if (bindlesTextureArray != nullptr)
		{
			count = bindlesTextureArray->GetTexturesCount();
		}

		return count;
	}

	bool TextureResourceBindles::EmptyFromType(int textureTypeIndex) const
	{
		uint32_t count = GetTextureCountFromType();
		return count == 0u;
	}

	void TextureResourceBindles::Bind()
	{
		for (auto& [bindingIndex, bindlesTextureArray] : m_TextureTypeVec)
		{
			RY_CORE_ASSERT(bindlesTextureArray != nullptr, "Not expexted nullptr Ref!");
			bindlesTextureArray->Bind(bindingIndex);
		}
	}
	void TextureResourceBindles::Unbind()
	{
		for (auto& [bindingIndex, bindlesTextureArray] : m_TextureTypeVec)
		{
			RY_CORE_ASSERT(bindlesTextureArray != nullptr, "Not expexted nullptr Ref!");
			bindlesTextureArray->UnBind(bindingIndex);
		}
	}

	bool TextureResourceBindles::IsTextureSizeVaild(const glm::uvec2& textureSize) const
	{
		RY_CORE_NOT_IMPL();
		RY_REMBER_FUNC_CHANGE("To decied to remove the funtion or to kep them!");
		return false;
	}

#pragma endregion

}
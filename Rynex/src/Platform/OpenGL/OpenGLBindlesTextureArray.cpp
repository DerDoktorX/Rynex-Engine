#include "rypch.h"
#include "OpenGLBindlesTextureArray.h"

#include <Platform/OpenGL/OpenGLBase.h>
#include <Platform/OpenGL/OpenGLTexture.h>

namespace Rynex {

#pragma region OpenGLBindlesTextureArray

	OpenGLBindlesTextureArray::OpenGLBindlesTextureArray()
		: m_Buffer(s_Target, nullptr, 0u, s_BufferFlag)
		, m_ByteSize(0u)
		, m_Count(0u)
		, m_LastBindeSlot(-1)
		, m_TexturesMap()
	{
		InvalideData();
	}

	OpenGLBindlesTextureArray::OpenGLBindlesTextureArray(uint32_t initCount)
		: m_Buffer(s_Target, nullptr, initCount * sizeof(uint64_t), s_BufferFlag)
		, m_ByteSize(initCount * sizeof(uint64_t))
		, m_Count(initCount)
		, m_LastBindeSlot(-1)
		, m_TexturesMap()
	{
		
		InvalideData();
	}

	OpenGLBindlesTextureArray::OpenGLBindlesTextureArray(std::vector<Ref<Texture>>&& texVec)
		: m_Buffer(s_Target, nullptr, texVec.size() * sizeof(uint64_t), s_BufferFlag)
		, m_ByteSize(texVec.size() * sizeof(uint64_t))
		, m_Count(texVec.size())
		, m_LastBindeSlot(-1)
	{
		std::vector<Ref<Texture>> texureVec = std::move(texVec);
		for (const Ref<Texture>& texure : texureVec)
		{
			SubmiteTextureVectorData(texure);
		}
		InvalideData();
	}

	OpenGLBindlesTextureArray::~OpenGLBindlesTextureArray()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());

		DestroyID();
	}

	bool OpenGLBindlesTextureArray::IsTransferd()
	{
		return m_Buffer.IsTransfered();
	}

	void OpenGLBindlesTextureArray::Bind(uint32_t slot)
	{		
		AktivateBindlesTextures();
		m_Buffer.BindSlot(s_Target, slot);
	}

	void OpenGLBindlesTextureArray::UnBind(uint32_t slot)
	{
		DeactivateBindlesTextures();
		m_Buffer.UnBindSlot(s_Target, slot);
	}

	void OpenGLBindlesTextureArray::AktivateTextures()
	{
		AktivateBindlesTexturesSafe();
	}

	void OpenGLBindlesTextureArray::DeactivateTextures()
	{
		DeactivateBindlesTexturesSafe();
	}

	

	int OpenGLBindlesTextureArray::AddTextures(const Ref<Texture>& texture)
	{
		int index = AddTextureVectorData(texture);
		return index;
	}

	int OpenGLBindlesTextureArray::SubmitTextures(const Ref<Texture>& texture)
	{
		int index = SubmiteTextureVectorData(texture);

		return index;
	}

	void OpenGLBindlesTextureArray::FlushTextures()
	{
		LoadeBindlesHandles();
	}

	void OpenGLBindlesTextureArray::ClearTextures()
	{
		m_TexturesMap.Clear();
		RY_CORE_WARN("Posible Crash! of OpengGL becouse Buffer wite size of zero!");
		m_Buffer.ResizeData(s_Target, nullptr, 0u, s_BufferFlag);
	}

	void OpenGLBindlesTextureArray::EraseTexture(const Ref<Texture>& texture)
	{
		if (m_TexturesMap.HasObject(texture))
		{
			Ref<OpenGLTextureStorageModern> texRef = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
			texRef->RemoveParent(this);
			m_TexturesMap.EraseFromObject(texture);

			LoadeBindlesHandles();
		}
	}

	void OpenGLBindlesTextureArray::SwapTexture(const Ref<Texture>& fromTexture, const Ref<Texture>& toTexture)
	{
		Ref<OpenGLTextureStorageModern> fromTexRef = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(fromTexture);
		fromTexRef->RemoveParent(this);

		m_TexturesMap.ChangeKeyObject(fromTexture, toTexture);

		Ref<OpenGLTextureStorageModern> toTexRef = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(toTexture);
		toTexRef->AddParent(this);
	}

	int OpenGLBindlesTextureArray::GetStoredTextureIndex(const Ref<Texture>& texture) const
	{
		int index = -1;

		if(m_TexturesMap.HasObject(texture))
			index = m_TexturesMap.GetObjectIndex(texture);

		return index;
	}

	bool OpenGLBindlesTextureArray::HasTextureStored(const Ref<Texture>& texture) const
	{
		Texture* texSearchPtr = texture.get();
		for (const Ref<Texture>& tex : m_TexturesMap)
		{
			Texture* texElementPtr = tex.get();

			if(texElementPtr == texSearchPtr)
				return true;
		}
		return false;
	}

	bool OpenGLBindlesTextureArray::HasTextureStoredOn(const Ref<Texture>& texture, int slot) const
	{
		if (slot >= m_TexturesMap.Size())
			return false;


		const Texture* texSearchPtr = texture.get();
		
		const Ref<Texture>& texElementRef = m_TexturesMap.GetDataFromIndex(slot);
		const Texture* texElementPtr = texElementRef.get();

		return texSearchPtr == texElementPtr;
	}

	const Ref<Texture>& OpenGLBindlesTextureArray::GetTexture(int slot) const
	{
		const Ref<Texture>& texElement = m_TexturesMap.GetDataFromIndex(slot);
		return texElement;
	}



	uint32_t OpenGLBindlesTextureArray::GetTextureDataByteSize() const
	{
		uint32_t byteSize = 0u;
		for (const Ref<Texture>& tex : m_TexturesMap)
		{
			byteSize += tex->GetCurrentRenderDataSize();
		}
		return byteSize;
	}
	void OpenGLBindlesTextureArray::CreateID()
	{
		
	}


	void OpenGLBindlesTextureArray::DestroyID()
	{
		DeactivateBindlesTexturesSafe();

		m_TexturesMap.Clear();
	}

	void OpenGLBindlesTextureArray::InvalideData()
	{
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLBindlesTextureArray::InvalideData);
		LoadeBindlesHandles();
	}

	int64_t OpenGLBindlesTextureArray::GetKey(const Ref<Texture>& texture) const
	{
		Texture* ptrTexture = texture.get();
		int64_t key = (int64_t)ptrTexture;
		return key;
	}

	int64_t OpenGLBindlesTextureArray::GetKey(OpenGLTextureStorageModern* ptrTexture) const
	{
		int64_t key = (int64_t)ptrTexture;
		return key;
	}

	int OpenGLBindlesTextureArray::AddTextureVectorData(const Ref<Texture>& texture)
	{
		// int64_t key = GetKey(texture);
		if(m_TexturesMap.HasObject(texture))
		{
			int index = m_TexturesMap.GetObjectIndex(texture);
			return index;
		}
		else
		{
			int index = m_TexturesMap.AddObjectIndex( texture);			
			Ref<OpenGLTextureStorageModern> texRef = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
			texRef->AddParent(this);
			LoadeBindlesHandles();
			return index;
		}
	}

	int OpenGLBindlesTextureArray::SubmiteTextureVectorData(const Ref<Texture>& texture)
	{
		if (m_TexturesMap.HasObject(texture))
		{
			int index = m_TexturesMap.GetObjectIndex(texture);
			return index;
		}
		else
		{
			int index = m_TexturesMap.AddObjectIndex(texture);

			Ref<OpenGLTextureStorageModern> texRef = std::static_pointer_cast<OpenGLTextureStorageModern, Texture>(texture);
			texRef->AddParent(this);


			return index;
		}
	}

	void OpenGLBindlesTextureArray::AktivateBindlesTextures()
	{
		for (Ref<Texture>& tex : m_TexturesMap)
		{
			tex->BindLessTex();
		}
	}

	void OpenGLBindlesTextureArray::AktivateBindlesTexturesSafe()
	{
		for (Ref<Texture>& tex : m_TexturesMap)
		{
			if (!tex->IsBindLessTexActiv())
				tex->BindLessTex();
		}
	}

	void OpenGLBindlesTextureArray::DeactivateBindlesTextures()
	{
		for (Ref<Texture>& tex : m_TexturesMap)
		{
			tex->UnBindLessTex();
		}
	}

	void OpenGLBindlesTextureArray::DeactivateBindlesTexturesSafe()
	{
		for (Ref<Texture>& tex : m_TexturesMap)
		{
			if (tex->IsBindLessTexActiv())
				tex->UnBindLessTex();
		}
	}

	void OpenGLBindlesTextureArray::LoadeBindlesHandles()
	{
		uint32_t countTex = m_TexturesMap.Size();
		uint32_t oldCount = m_Count;
		

		m_Count = m_TexturesMap.Size();
		m_ByteSize = m_Count * sizeof(uint64_t);
		
		std::vector<uint64_t> bindlesTextureHandleVec;
		bindlesTextureHandleVec.reserve(m_Count);

		for (const Ref<Texture>& tex : m_TexturesMap)
		{
			uint64_t bindlesTexHandle = tex->GetBindlesHandle();
			bindlesTextureHandleVec.emplace_back(bindlesTexHandle);
		}
		const uint8_t* bindlesTexHandlesDataPtr = reinterpret_cast<const uint8_t*>(bindlesTextureHandleVec.data());
		if (oldCount == countTex)
			m_Buffer.SetData(s_Target, bindlesTexHandlesDataPtr, 0, m_ByteSize);
		else
			m_Buffer.ResizeData(s_Target, bindlesTexHandlesDataPtr, m_ByteSize, s_BufferFlag);
		

	}

	

	void OpenGLBindlesTextureArray::OnChildeSpecifcationChange(OpenGLTextureStorageModern* ptrTex)
	{
		int64_t key = GetKey(ptrTex);
		if (m_TexturesMap.HasKey(key))
		{
			uint32_t index = m_TexturesMap.GetIndex(key);
			uint32_t offsetBytes = index * sizeof(uint64_t);
			uint32_t sizeBytes = sizeof(uint64_t);

			uint64_t bindlesTexHandle = ptrTex->GetBindlesHandle();
			const uint64_t* bindlesTexHandlesDataPtr = &bindlesTexHandle;

			const uint8_t* bindlesTexHandlesByteDataPtr = reinterpret_cast<const uint8_t*>(bindlesTexHandlesDataPtr);

			m_Buffer.SetData(s_Target, bindlesTexHandlesByteDataPtr, offsetBytes, sizeBytes);
		}
	}

	void OpenGLBindlesTextureArray::OnChildeDataChange(OpenGLTextureStorageModern* ptrTex)
	{
		// don't care
	}

	

	void OpenGLBindlesTextureArray::OnChildeDestroy(OpenGLTextureStorageModern* ptrTex)
	{
		int64_t key = GetKey(ptrTex);
		if (m_TexturesMap.HasKey(key))
		{
			m_TexturesMap.EraseFromKey(key);
			LoadeBindlesHandles();
		}

	}

#pragma endregion
	


	

}
#include "rypch.h"
#include "DefaultMaterial.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>

#include <glm/gtc/type_ptr.hpp>

namespace Rynex {
#define DEFAULT_PATH_DEFAULT_MATERIAL "D:/dev/Rynex-Test-Projects/Test-Project-System/Assets/Shaders/Fetures/Cube3DShadow.glsl";
#define CHEKEBORD_DEFAULT_TEXTURE_DEFAULT_MATERIEL 0
	
	


	DefaultMaterial::DefaultMaterial()
		: m_Change(true)
		, m_BufferData({
			glm::vec3{ 1.0f, 1.0f, 1.0f }
			, 1.0f
			, -1.0f
			, -2.0f
			, 0.0f
			, -1
		})
		, m_DefaultMap(nullptr)
		, m_ShadeRenderMode(
			RenderMode::CallFace_None
			| RenderMode::A_Buffer
			| RenderMode::Death_Buffer
#if RY_MESH_OPTIMIZE_TRINGLE_STRIPS
			| RenderMode::PrimitivReset
#endif
		)
		, m_DepthRenderMode(
			RenderMode::CallFace_None
			| RenderMode::Death_Buffer
#if RY_MESH_OPTIMIZE_TRINGLE_STRIPS
			| RenderMode::PrimitivReset
#endif
		)
		, m_PassesVec()
		, m_LastPassPtr(nullptr)
	{
		m_DefaultMap = Texture::White();
	}

	DefaultMaterial::DefaultMaterial(const MaterielShaderData& data, const std::vector<Pass>& vec)
		: m_BufferData(data)
		, m_PassesVec(vec)
		, m_ShadeRenderMode(
			RenderMode::CallFace_None
			| RenderMode::A_Buffer
			| RenderMode::Death_Buffer
		)
		, m_DepthRenderMode(
			RenderMode::CallFace_None
			| RenderMode::Death_Buffer
		)
		, m_LastPassPtr(nullptr)
	{
		m_DefaultMap = Texture::White();

		
	}


	DefaultMaterial::DefaultMaterial(std::vector<std::filesystem::path>&& paths, const Ref<Shader>& shaderShade, const Ref<Shader>& shaderDepth)
		: m_Change(true)
		, m_BufferData({
			glm::vec3{ 1.0f, 1.0f, 1.0f }
			, 1.0f
			, -1.0f
			, -2.0f
			, 0.0f
			, -1
		})
		, m_DefaultMap(nullptr)
		, m_UniformMaterial()
		, m_ShadeRenderMode(
			RenderMode::CallFace_None
			| RenderMode::A_Buffer
			| RenderMode::Death_Buffer
		)
		, m_DepthRenderMode(
			RenderMode::CallFace_None
			| RenderMode::Death_Buffer
		)
		, m_PassesVec()
		, m_LastPassPtr(nullptr)
	{
		std::vector<std::filesystem::path> filePaths = std::move(paths);
		if (filePaths.empty())
		{
			m_DefaultMap = Texture::White();
		}
		else
		{			
			const std::filesystem::path& filePath = filePaths.at(1ull);
			AssetManager::GetAssetAsync(filePath, &m_DefaultMap);
			
		}
		m_PassesVec = std::vector<Pass>{
			Pass{
				shaderShade,
				Ref<UniformBuffer>(nullptr),
				
				std::vector<TextureTypes>{ TextureTypes::AlbedoMap }
				,std::vector<Ref<Texture>>{  m_DefaultMap }

				
				, DrawSpecification::RenderObject_Vertex_Array
				, m_ShadeRenderMode
			},
			Pass{
				shaderDepth
				, Ref<UniformBuffer>(nullptr)
				, std::vector<TextureTypes>{}
				, std::vector<Ref<Texture>>{}
				, DrawSpecification::RenderObject_Vertex_Array
				, m_DepthRenderMode
			}
		};
		const BufferLayout& layoutShade = shaderShade->GetOutPut();
		const BufferLayout& layoutDepth = shaderDepth->GetOutPut();
		RY_CORE_ASSERT(layoutShade != layoutDepth);
		RY_CORE_ASSERT(layoutShade == layoutShade);
		RY_CORE_ASSERT(layoutDepth == layoutDepth);

	}

	DefaultMaterial::DefaultMaterial(const MaterielShaderData& data, Ref<Texture> tex, const Ref<Shader>& shaderShade, const Ref<Shader>& shaderDepth)
		: m_Change(true)
		, m_BufferData(data)
		, m_DefaultMap(tex)
		, m_ShadeRenderMode(
			RenderMode::CallFace_None
			| RenderMode::A_Buffer
			| RenderMode::Death_Buffer
		)
		, m_DepthRenderMode(
			RenderMode::CallFace_None
			| RenderMode::Death_Buffer
		)
		, m_LastPassPtr(nullptr)
		, m_PassesVec()
	{	
		if (nullptr == m_DefaultMap)
		{
			m_DefaultMap = Texture::White();
		}

		m_PassesVec = {
			Pass{
				shaderShade,
				Ref<UniformBuffer>(nullptr),

				std::vector<TextureTypes>{ TextureTypes::AlbedoMap }
				,std::vector<Ref<Texture>>{  m_DefaultMap }


				, DrawSpecification::RenderObject_Vertex_Array
				, m_ShadeRenderMode
			},
			Pass{
				shaderDepth
				, Ref<UniformBuffer>(nullptr)
				, std::vector<TextureTypes>{}
				, std::vector<Ref<Texture>>{}
				, DrawSpecification::RenderObject_Vertex_Array
				, m_DepthRenderMode
			}
		};

		const BufferLayout& layoutShade = shaderShade->GetOutPut();
		const BufferLayout& layoutDepth = shaderDepth->GetOutPut();
		RY_CORE_ASSERT(layoutShade != layoutDepth);
		RY_CORE_ASSERT(layoutShade == layoutShade);
		RY_CORE_ASSERT(layoutDepth == layoutDepth);


	}

	DefaultMaterial::~DefaultMaterial()
	{		
		RY_DESTROY_REF(m_DefaultMap);
	}
	
	void DefaultMaterial::UpdateMaterielData(void* materielArrayData, uint32_t offset, uint32_t byteSize)const
	{
		uint32_t bufferByteSize = sizeof(MaterielShaderData);
		RY_CORE_ASSERT(byteSize == bufferByteSize);

		const void* bufferPtr = &m_BufferData;
		uint8_t* materielArrayDataPtr = (uint8_t*)materielArrayData;
		void* materielArrayDataOffsetPtr = offset + materielArrayDataPtr;

		std::memcpy(materielArrayDataOffsetPtr, bufferPtr, byteSize);
	}

	int DefaultMaterial::AddMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlesTexures) const
	{
		int indexSotre = bindlesTexures->AddTextures(m_DefaultMap);
		return indexSotre;
	}

	int DefaultMaterial::GetMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlesTexures) const
	{
		int indexSotre = bindlesTexures->GetStoredTextureIndex(m_DefaultMap);
		return indexSotre;
	}

	Ref<UniformBuffer> DefaultMaterial::GetMaterielUniformBuffer()
	{
		if (Ref<UniformBuffer> ubMaterialRef = m_UniformMaterial.lock())
		{
			return ubMaterialRef;
		}

		else
		{
			m_UniformMaterial.reset();

			BufferLayout layout = BufferLayout({
				{ SDT::Float3,	"Color"			},
				{ SDT::Float,	"Alpha"			},
				{ SDT::Float,	"Shinines"		},
				{ SDT::Float,	"Specular"		},
				{ SDT::Float,	"AmbientLigthe"	},
				{ SDT::Int,		"UseTexure"		}
			}, 0);
			uint32_t byteSize = sizeof(m_BufferData);
			MaterielShaderData* materielShaderDataPtr = &m_BufferData;
			Ref<UniformBuffer> ubMateril = UniformBuffer::Create(materielShaderDataPtr, byteSize, layout);
			m_UniformMaterial = ubMateril;
			return ubMateril;
		}
	}


	bool DefaultMaterial::HasSpecForDraw(const BufferLayout& layout, int lodTier) const
	{
		int index = GetLayaoutIndex(layout, lodTier);
		return index != -1;
	}

	bool DefaultMaterial::HasSpecForDraw(const BufferLayout& layout, int lodTier)
	{
		int index = GetLayaoutIndex(layout, lodTier);
		if (index == -1)
			return false;

		m_LastPassPtr = m_PassesVec.data() + index;
		return true;
	}

	const std::vector<Ref<Texture>>& DefaultMaterial::GetTextureForDraw()
	{
		RY_CORE_ASSERT(nullptr != m_LastPassPtr);
		return m_LastPassPtr->texturesVec;
	}

	const Ref<Shader>& DefaultMaterial::GetShaderForDraw()
	{
		RY_CORE_ASSERT(nullptr != m_LastPassPtr);
		return m_LastPassPtr->shader;
	}

	DrawSpecification::BatchConfig DefaultMaterial::GetDrawSpecification()
	{
		RY_CORE_ASSERT(nullptr != m_LastPassPtr);
		DrawSpecification::BatchConfig drawSpecification = m_LastPassPtr->drawSpecification;
		return drawSpecification;
	}

	int DefaultMaterial::GetRenderMode()
	{
		RY_CORE_ASSERT(nullptr != m_LastPassPtr);
		return m_LastPassPtr->renderMode;
	}

	int DefaultMaterial::GetLayaoutIndex()
	{
		RY_CORE_ASSERT(nullptr != m_LastPassPtr);
		int differnz = m_LastPassPtr - m_PassesVec.data();
		return differnz;
	}


	const std::vector<Ref<Texture>>& DefaultMaterial::GetTextureForDraw(const BufferLayout& layout, int lodTier) const
	{
		int index = GetLayaoutIndex(layout, lodTier);
		if (index != -1)
		{
			const Pass& pass = m_PassesVec.at(index);
			return pass.texturesVec;
		}
		RY_CORE_ASSERT(false);
		std::vector<Ref<Texture>> vec;
		return vec;
	}

	const Ref<Shader>& DefaultMaterial::GetShaderForDraw(const BufferLayout& layout, int lodTier) const
	{
		int index = GetLayaoutIndex(layout, lodTier);
		if (index != -1)
		{
			const Pass& pass = m_PassesVec.at(index);
			return pass.shader;
		}
		RY_CORE_ASSERT(false);
		static Ref<Shader> shader = Ref<Shader>(nullptr);
		return shader;
	}

	int DefaultMaterial::GetDrawSpecification(const BufferLayout& layout, int lodTier) const
	{
		int index = GetLayaoutIndex(layout, lodTier);
		if (index != -1)
		{
			const Pass& pass = m_PassesVec.at(index);
			return pass.drawSpecification;
		}
		RY_CORE_ASSERT(false);
		return DrawSpecification::None;
	}

	int DefaultMaterial::GetRenderMode(const BufferLayout& layout, int lodTier) const
	{
		int index = GetLayaoutIndex(layout, lodTier);
		if (index != -1)
		{
			const Pass& pass = m_PassesVec.at(index);
			return pass.renderMode;
		}
		RY_CORE_ASSERT(false);
		return RenderMode::None;
	}

	int DefaultMaterial::GetLayaoutIndex(const BufferLayout& layout, int lodTier) const
	{
		if (nullptr != m_LastPassPtr)
		{
			const Ref<Shader>& shader = m_LastPassPtr->shader;
			if (nullptr != shader)
			{
				const BufferLayout& layoutOut = shader->GetOutPut();
				if (layout == layoutOut)
				{
					const Pass* dataPtr = m_PassesVec.data();
					int differnz = m_LastPassPtr - dataPtr;
					return differnz;
				}
			}

		}
		for (int i = 0; i < m_PassesVec.size(); i++)
		{
			const Pass& pass = m_PassesVec.at(i);
			const Ref<Shader>& shader = pass.shader;
			if (nullptr == shader)
				continue;

			const BufferLayout& layoutOut = shader->GetOutPut();
			if (layout == layoutOut)
				return i;
		}

		return -1;
	}

}
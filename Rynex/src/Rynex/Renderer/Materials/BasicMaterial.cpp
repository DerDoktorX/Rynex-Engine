#include "rypch.h"
#include "BasicMaterial.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Asset/Base/AssetManager.h>


#include <glm/gtc/type_ptr.hpp>

namespace Rynex {
#define DEFAULT_PATH_BASIC_MATERIAL 1 ? "../Rynex-Editor/Editor-Assets/shaders/3DLigthe.glsl" : "../Rynex-Editor/Editor-Assets/shaders/3DLShadowSimple.glsl"
#define DEFAULT_PATH_SHADOW_SHADER "../Rynex-Editor/Editor-Assets/shaders/3DShadowShader.glsl"

	BasicMaterial::BasicMaterial(const std::vector<std::filesystem::path>& paths)
		: m_BufferData(nullptr)
	{		
		m_Specular = nullptr;
		m_DefuseMap = nullptr;
		int shadernum = 0;
		switch (paths.size())
		{
		case 2:
			m_Specular = AssetManager::GetAsset<Texture>(paths[1]);
			shadernum = 2;
		case 1:
			m_DefuseMap = AssetManager::GetAsset<Texture>(paths[0]);
			shadernum = 1;
		default:
			break;
		}
		
		

		BufferLayout layoute = {
			{ ShaderDataType::Float4x4 ,"ModelMatrix" },
			{ ShaderDataType::Float3, "Color" },
			{ ShaderDataType::Float ,"Alpha" },
			{ ShaderDataType::Float ,"Shinines" },
			{ ShaderDataType::Float ,"Specular" },
			
			
			{ ShaderDataType::Int ,"EntityID" },
			{ ShaderDataType::Int ,"UseShader" },
			
		};
		
		
		UniformDataBasic bufferData = {
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			},
			{ 0.45f, 0.45f, 0.45f },
			1.0f,
			16.0f,
			1.0f,
			-1,
			shadernum,
		};
		
		RY_CORE_ASSERT(layoute.GetStride() == sizeof(bufferData));

		

		m_UniformBuffer = UniformBuffer::Create(&bufferData, sizeof(bufferData), layoute, 1);
		m_Shader = AssetManager::GetAsset<Shader>(DEFAULT_PATH_BASIC_MATERIAL);
		
		
		

		m_ShadowShader = AssetManager::GetAsset<Shader>(DEFAULT_PATH_BASIC_MATERIAL);
		m_BufferData = (UniformDataBasic*)m_UniformBuffer->GetBufferData().data();
		m_UniformBuffer->SetOnDelete([this]() {
			m_BufferData = nullptr;
		});
		if (shadernum)
		{
			m_BufferData->Color[0] = 1;
			m_BufferData->Color[1] = 1;
			m_BufferData->Color[2] = 1;
		}
		
	}
	BasicMaterial::BasicMaterial(std::vector<std::filesystem::path>&& paths)
		: m_BufferData(nullptr)
	{
		std::vector<std::filesystem::path> path = std::move(paths);
		m_Specular = nullptr;
		m_DefuseMap = nullptr;
		int shadernum = 0;
		switch (path.size())
		{
		case 2:
			AssetManager::GetAssetAsync<Texture>(path[1], &m_Specular);
			shadernum = 2;
		case 1:
			AssetManager::GetAssetAsync<Texture>(path[0], &m_DefuseMap);
			shadernum = 1;
		default:
			break;
		}

		BufferLayout layoute = {
			{ ShaderDataType::Float4x4 ,"ModelMatrix" },
			{ ShaderDataType::Float3, "Color" },
			{ ShaderDataType::Float ,"Alpha" },
			{ ShaderDataType::Float ,"Shinines" },
			{ ShaderDataType::Float ,"Specular" },


			{ ShaderDataType::Int ,"EntityID" },
			{ ShaderDataType::Int ,"UseShader" },

		};


		UniformDataBasic bufferData = {
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			},
			{ 0.45f, 0.45f, 0.45f },
			1.0f,
			16.0f,
			1.0f,
			-1,
			shadernum,
		};
		

		RY_CORE_ASSERT(layoute.GetStride() == sizeof(bufferData));
		std::vector<unsigned char> data(sizeof(bufferData));

		std::memcpy(data.data(), &bufferData, sizeof(bufferData));

		m_UniformBuffer = UniformBuffer::CreateAsync(std::move(data), layoute, BufferDataUsage::DynamicDraw, 1);
		AssetManager::GetAssetAsync<Shader>(DEFAULT_PATH_BASIC_MATERIAL, &m_Shader);
		AssetManager::GetAssetAsync<Shader>(DEFAULT_PATH_SHADOW_SHADER, &m_ShadowShader);
		Application::Get().SubmiteToMainThreedQueue([this, shadernum]() {
			InitAsync();
			if(shadernum)
			{
				m_BufferData->Color[0] = 1;
				m_BufferData->Color[1] = 1;
				m_BufferData->Color[2] = 1;
			}
		});
	}



	BasicMaterial::~BasicMaterial()
	{
		m_Shader.reset();
		m_UniformBuffer.reset();
	}

	void BasicMaterial::Bind(int* entityIDs, uint32_t size)
	{
		m_BufferData->EntityID = *entityIDs;
		m_UniformBuffer->SetLocelData({ ShaderDataType::Int, "EntityID" }, &m_BufferData->EntityID, sizeof(m_BufferData->EntityID) );
		m_Shader->Bind();
		m_UniformBuffer->Bind();
		if(m_DefuseMap)
			m_DefuseMap->Bind(6);
		if (m_Specular)
			m_Specular->Bind(7);
	}

	void BasicMaterial::UnBind()
	{
		m_Shader->UnBind();
	}

	bool BasicMaterial::IsRady()
	{
		return m_BufferData && m_Shader && m_ShadowShader && m_UniformBuffer;
	}

	void BasicMaterial::InitAsync()
	{
		m_BufferData = (UniformDataBasic*)m_UniformBuffer->GetBufferData().data();
		m_UniformBuffer->SetOnDelete([this]() {
			m_BufferData = nullptr;
		});
	}

	void BasicMaterial::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer)
	{
	}

	void BasicMaterial::SetUniformValue(const BufferElement& element, void* data)
	{
		m_UniformBuffer->SetLocelData(element, data, element.Size);
	}

	const Ref<UniformBuffer>& BasicMaterial::GetUniformBuffer() const
	{
		return m_UniformBuffer;
	}

	void* BasicMaterial::GetUniformData()
	{
		return m_BufferData;
	}

	Ref<Shader> BasicMaterial::GetShadowShader()
	{
		return m_ShadowShader;
	}

	void BasicMaterial::SetColor(const glm::vec3& color)
	{
		m_BufferData->Color[0] = color.x;
		m_BufferData->Color[1] = color.y;
		m_BufferData->Color[2] = color.z;

		m_UniformBuffer->SetLocelData({ ShaderDataType::Float3, "Color" }, m_BufferData->Color, sizeof(m_BufferData->Color));
	}

	const glm::vec3& BasicMaterial::GetColor() const
	{
		return glm::vec3(m_BufferData->Color[0], m_BufferData->Color[1], m_BufferData->Color[2]);
	}

	void BasicMaterial::SetAlpha(float v)
	{
		m_BufferData->Alpha = v;
		m_UniformBuffer->SetLocelData({ ShaderDataType::Float, "Alpha" }, &m_BufferData->Alpha, sizeof(m_BufferData->Alpha));
	}

	float BasicMaterial::GetAlpha() const
	{
		return m_BufferData->Alpha;
	}

	void BasicMaterial::SetShader(const Ref<Shader>& shader)
	{
		m_Shader = shader;
	}

	const Ref<Shader>& BasicMaterial::GetShader() const
	{
		return m_Shader;
	}

	void BasicMaterial::SetMatrix(const glm::mat4& matrix)
	{
		
		m_BufferData->ModelMatrix[0] = matrix[0].x;
		m_BufferData->ModelMatrix[1] = matrix[0].y;
		m_BufferData->ModelMatrix[2] = matrix[0].z;
		m_BufferData->ModelMatrix[3] = matrix[0].w;

		m_BufferData->ModelMatrix[4] = matrix[1].x;
		m_BufferData->ModelMatrix[5] = matrix[1].y;
		m_BufferData->ModelMatrix[6] = matrix[1].z;
		m_BufferData->ModelMatrix[7] = matrix[1].w;

		m_BufferData->ModelMatrix[8]  = matrix[2].x;
		m_BufferData->ModelMatrix[9]  = matrix[2].y;
		m_BufferData->ModelMatrix[10] = matrix[2].z;
		m_BufferData->ModelMatrix[11] = matrix[2].w;

		m_BufferData->ModelMatrix[12] = matrix[3].x;
		m_BufferData->ModelMatrix[13] = matrix[3].y;
		m_BufferData->ModelMatrix[14] = matrix[3].z;
		m_BufferData->ModelMatrix[15] = matrix[3].w;

		m_UniformBuffer->SetLocelData({ ShaderDataType::Float4x4, "ModelMatrix" }, m_BufferData->ModelMatrix, sizeof(m_BufferData->ModelMatrix));

	}

	void BasicMaterial::SetShadowTextures(const std::array<Ref<Texture>, 72>& shadowsTex, uint32_t size)
	{
		RY_CORE_ASSERT(false);
	}

	void BasicMaterial::BindShadow()
	{
		m_ShadowShader->Bind();
		m_UniformBuffer->Bind(1);
	}

	void BasicMaterial::SetFlage(int flage)
	{
		m_Flage = flage;
	}

	int BasicMaterial::GetFlage()
	{
		return m_Flage;
	}
}
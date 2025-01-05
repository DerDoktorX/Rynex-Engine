#include "rypch.h"
#include "PhysicalMaterial.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Asset/Base/AssetManager.h>


#include <glm/gtc/type_ptr.hpp>

namespace Rynex {
#define UNIFORM_DATA_SIZE 112
#define DEFAULT_PATH_PHIYSIK_MATERIAL "D:/dev/Rynex-Test-Projects/Test-Project-System/Assets/Shaders/PBR-Material.glsl"


	PhysicalMaterial::PhysicalMaterial(bool async)
		: m_BufferData(nullptr)
	{
		BufferLayout layoute = {
			{ ShaderDataType::Float4x4 ,"ModelMatrix" },
			{ ShaderDataType::Float3, "Color" },
			{ ShaderDataType::Float ,"Alpha" },
			{ ShaderDataType::Float ,"Shinines" },
			{ ShaderDataType::Float ,"Specular" },


			{ ShaderDataType::Int ,"EntityID" },
			{ ShaderDataType::Int2 ,"Empty" },
			
		};


		UniformDataPhyicel bufferData = {
			{
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			},
			{ 0.65f, 0.65f, 0.65f },
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			1.0f,
			-1,
			{-2,-3}
			
		};

		RY_CORE_ASSERT(layoute.GetStride() == sizeof(bufferData));
		std::vector<unsigned char> data(sizeof(bufferData));

		std::memcpy(data.data(), &bufferData, sizeof(bufferData));

		if (async)
		{
			m_UniformBuffer = UniformBuffer::CreateAsync(std::move(data), layoute, BufferDataUsage::DynamicDraw, 1);
			AssetManager::GetAssetAsync<Shader>(DEFAULT_PATH_PHIYSIK_MATERIAL, &m_Shader);
			Application::Get().SubmiteToMainThreedQueue([this]() {
				InitAsync();
				});
		}
		else
		{
			m_UniformBuffer = UniformBuffer::Create(data.data(), data.size(), layoute, 1);
			m_Shader = AssetManager::GetAsset<Shader>(DEFAULT_PATH_PHIYSIK_MATERIAL);
			m_BufferData = (UniformDataPhyicel*)m_UniformBuffer->GetBufferData().data();
			m_UniformBuffer->SetOnDelete([this]() {
				m_BufferData = nullptr;
				});
		}

	}


	PhysicalMaterial::~PhysicalMaterial()
	{
		m_Shader.reset();
		m_UniformBuffer.reset();
	}

	void PhysicalMaterial::Bind(int* entityID, uint32_t size, CameraData& camera, CameraData& ligthCam)
	{
		m_BufferData->EntityID = *entityID;
		m_UniformBuffer->SetLocelData({ ShaderDataType::Int, "EntityID" }, &m_BufferData->EntityID, sizeof(m_BufferData->EntityID));
		m_UniformBuffer->Bind();
		m_Shader->Bind();


	}

	void PhysicalMaterial::UnBind()
	{
		m_Shader->UnBind();
	}

	void PhysicalMaterial::InitAsync()
	{
		m_BufferData = (UniformDataPhyicel*)m_UniformBuffer->GetBufferData().data();
		m_UniformBuffer->SetOnDelete([this]() {
			m_BufferData = nullptr;
			});
	}

	void PhysicalMaterial::SetUniformBuffer(Ref<UniformBuffer> uniformBuffer)
	{
	}

	void PhysicalMaterial::SetUniformValue(const BufferElement& element, void* data)
	{
		m_UniformBuffer->SetLocelData(element, data, element.Size);
	}

	const Ref<UniformBuffer>& PhysicalMaterial::GetUniformBuffer() const
	{
		return m_UniformBuffer;
	}

	Ref<Shader> PhysicalMaterial::GetShadowShader()
	{
		return m_ShadowShader;
	}

	void* PhysicalMaterial::GetUniformData()
	{
		return m_BufferData;
	}

	void PhysicalMaterial::SetColor(const glm::vec3& color)
	{
		m_BufferData->Color[0] = color.x;
		m_BufferData->Color[1] = color.y;
		m_BufferData->Color[2] = color.z;

		m_UniformBuffer->SetLocelData({ ShaderDataType::Float3, "Color" }, m_BufferData->Color, sizeof(m_BufferData->Color));
	}

	const glm::vec3& PhysicalMaterial::GetColor() const
	{
		return glm::vec3(m_BufferData->Color[0], m_BufferData->Color[1], m_BufferData->Color[2]);
	}

	void PhysicalMaterial::SetAlpha(float v)
	{
		m_BufferData->Alpha = v;
		m_UniformBuffer->SetLocelData({ ShaderDataType::Float, "Alpha" }, &m_BufferData->Alpha, sizeof(m_BufferData->Alpha));
	}

	float PhysicalMaterial::GetAlpha() const
	{
		return m_BufferData->Alpha;
	}

	void PhysicalMaterial::SetShader(const Ref<Shader>& shader)
	{
		m_Shader = shader;
	}

	const Ref<Shader>& PhysicalMaterial::GetShader() const
	{
		return m_Shader;
	}

	void PhysicalMaterial::SetMatrix(const glm::mat4& matrix)
	{
		m_BufferData->ModelMatrix[0] = matrix[0].x;
		m_BufferData->ModelMatrix[1] = matrix[0].y;
		m_BufferData->ModelMatrix[2] = matrix[0].z;
		m_BufferData->ModelMatrix[3] = matrix[0].w;

		m_BufferData->ModelMatrix[4] = matrix[1].x;
		m_BufferData->ModelMatrix[5] = matrix[1].y;
		m_BufferData->ModelMatrix[6] = matrix[1].z;
		m_BufferData->ModelMatrix[7] = matrix[1].w;

		m_BufferData->ModelMatrix[8] = matrix[2].x;
		m_BufferData->ModelMatrix[9] = matrix[2].y;
		m_BufferData->ModelMatrix[10] = matrix[2].z;
		m_BufferData->ModelMatrix[11] = matrix[2].w;

		m_BufferData->ModelMatrix[12] = matrix[3].x;
		m_BufferData->ModelMatrix[13] = matrix[3].y;
		m_BufferData->ModelMatrix[14] = matrix[3].z;
		m_BufferData->ModelMatrix[15] = matrix[3].w;

		m_UniformBuffer->SetLocelData({ ShaderDataType::Float4x4, "ModelMatrix" }, m_BufferData->ModelMatrix, sizeof(m_BufferData->ModelMatrix));

	}

	void PhysicalMaterial::SetShadowTextures(const std::array<Ref<Texture>, 72>& shadowsTex, uint32_t size)
	{
		RY_CORE_ASSERT(false);
	}

	void PhysicalMaterial::BindShadow(CameraData& ligthCam)
	{
		RY_CORE_ASSERT(false);
	}

	void PhysicalMaterial::SetFlage(int flage)
	{
		m_Flage = flage;
	}

	int PhysicalMaterial::GetFlage()
	{
		return m_Flage;
	}
}
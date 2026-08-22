#include "rypch.h"
#include "BoundingVolume.h"

#include "Rynex/Core/Assert.h"

namespace Rynex {

#define RY_MAX_FLOAT std::numeric_limits<float>::max()
#define RY_MIN_FLOAT std::numeric_limits<float>::min()

	namespace Utils {

		static void CheckAndSetAABB(float* min, float* max, float value)
		{
			if (value > *max)
				*max = value;
			if (value < *min)
				*min = value;
		}
	}

	BoundingVolume::BoundingVolume()
	{
	}

	BoundingVolume::BoundingVolume(const BoundingVolume& b)
		: m_AABB(b.m_AABB)
		, m_Sphere(b.m_Sphere)
	{
	}

	BoundingVolume::~BoundingVolume()
	{
	}

	BoundingVolume::BoundingVolume(const glm::vec3& max, const glm::vec3& min)
		: m_AABB(max, min)
		, m_Sphere(max, min)
	{
	}

	BoundingVolume::BoundingVolume(const glm::vec3& max, const glm::vec3& min, const glm::vec3& center, float radius)
		: m_AABB(max, min)
		, m_Sphere(center, radius)
	{
	}

	BoundingVolume::BoundingVolume(const AABB& aabb, const Sphere& sphere)
		: m_AABB(aabb)
		, m_Sphere(sphere)
	{
	}

	BoundingVolume::BoundingVolume(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement)
	{
		SetBoxAABB(vertexBuffer, postionElement);
	}

	BoundingVolume::BoundingVolume(std::array<glm::vec3, 8>& points)
	{
		SetBoxAABB(points);
	}

	

	void BoundingVolume::SetBoxAABB(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement)
	{
		const BufferLayout& layoute = vertexBuffer->GetLayout();
		const std::vector<unsigned char>& data = vertexBuffer->GetBufferData();
		uint32_t size = data.size(), stride = layoute.GetStride();
		bool finde = false;
		for (const BufferElement& element : layoute)
		{
			if (postionElement == element)
			{
				postionElement = element;
				for (uint32_t i = 0; i < size; i += stride)
				{
					CalculatMinMax(data, i + postionElement.offset, postionElement);
				}
				glm::vec3 differenze = m_AABB.Min - m_AABB.Max;
				glm::vec3 half = glm::vec3(0.5f, 0.5f, 0.5f) * differenze;
				m_Sphere.Center = m_AABB.Max + half;
				break;
			}
		}
		
		RY_CORE_INFO("Succesfull Create AABB Box! Min({0}, {1}, {2})  Max({3}, {4}, {5})", m_AABB.Min.x, m_AABB.Min.y, m_AABB.Min.z, m_AABB.Max.x, m_AABB.Max.y, m_AABB.Max.z);
		float extremValue[6] = {
			m_AABB.Min[0],
			m_AABB.Min[1],
			m_AABB.Min[2],

			m_AABB.Max[0],
			m_AABB.Max[1],
			m_AABB.Max[2]
		};
		m_Sphere.Radius = 0.0f;


		for (uint8_t i = 0; i < 6; i++)
		{
			float value = glm::abs(extremValue[i]);
			m_Sphere.Radius = value > m_Sphere.Radius ? value : m_Sphere.Radius;
		}
	}
	
	void BoundingVolume::SetBoxAABB(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, BufferElement& postionElement)
	{
		const BufferLayout& layoute = vertexBuffer->GetLayout();
		const std::vector<uint8_t>& dataVec = vertexBuffer->GetBufferData();
		const std::vector<uint8_t>& indiciesVec = indexBuffer->GetBufferData();
		std::vector<uint32_t> indices32ByteVec;
		
		const uint32_t elementByteSize = indexBuffer->GetElementByte();
		const uint32_t byteSizeModuleElementByteSize = indiciesVec.size() % elementByteSize;
		const uint32_t indicesCount = indiciesVec.size() / elementByteSize;
		RY_CORE_ASSERT(0u == byteSizeModuleElementByteSize, "not Viald BytsSize Found!");
		RY_CORE_ASSERT(sizeof(uint32_t) == elementByteSize, "not expexted Default count like 4 bytes = 32 bit!");
		indices32ByteVec.insert(indices32ByteVec.begin(), indiciesVec.begin(), indiciesVec.end());
		

		uint32_t verticesSize = dataVec.size(), stride = layoute.GetStride();
		bool finde = false;
		for (const BufferElement& element : layoute)
		{
			if (postionElement == element)
			{
				postionElement = element;
				
				
				for (const uint32_t& vertexIndex : indices32ByteVec)
				{
					uint32_t vertexBytePos = vertexIndex * stride;
					CalculatMinMax(dataVec, vertexBytePos + postionElement.offset, postionElement);
				}
				glm::vec3 differenze = m_AABB.Min - m_AABB.Max;
				glm::vec3 half = glm::vec3(0.5f, 0.5f, 0.5f) * differenze;
				m_Sphere.Center = m_AABB.Max + half;
				m_Sphere.Radius = 0.0f;
				for (const uint32_t& vertexIndex : indices32ByteVec)
				{
					uint32_t vertexBytePos = vertexIndex * stride;
					FindeRadius(dataVec, vertexBytePos + postionElement.offset, postionElement);
				}
				break;
			}
		}

		RY_CORE_INFO("Succesfull Create AABB Box! Min({0}, {1}, {2})  Max({3}, {4}, {5})", m_AABB.Min.x, m_AABB.Min.y, m_AABB.Min.z, m_AABB.Max.x, m_AABB.Max.y, m_AABB.Max.z);
		

	}

	void BoundingVolume::SetBoxAABB(std::array<glm::vec3, 8>& points)
	{
		if (RY_MIN_FLOAT != m_AABB.Max.x)
		{
			m_AABB.Max = glm::vec3(RY_MIN_FLOAT, RY_MIN_FLOAT, RY_MIN_FLOAT);
			m_AABB.Min = glm::vec3(RY_MAX_FLOAT, RY_MAX_FLOAT, RY_MAX_FLOAT);
		}
		for (auto& point : points)
		{
			Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, point.x);
			Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, point.y);
			Utils::CheckAndSetAABB(&m_AABB.Min.z, &m_AABB.Max.z, point.z);
		}
		float extremValue[6] = {
			m_AABB.Min[0],
			m_AABB.Min[1],
			m_AABB.Min[2],

			m_AABB.Max[0],
			m_AABB.Max[1],
			m_AABB.Max[2]
		};

		m_Sphere.Radius = 0.0f;

		for (uint8_t i = 0; i < 6; i++)
		{
			float value = glm::abs(extremValue[i]);
			m_Sphere.Radius = value > m_Sphere.Radius ? value : m_Sphere.Radius;
		}
	}

	glm::vec3 BoundingVolume::GetMin(const glm::mat4& model) const
	{
		glm::vec4 min = model * glm::vec4(m_AABB.Min, 1.0f);
		return glm::vec3(min / min.w) ;
	}

	glm::vec3 BoundingVolume::GetMax(const glm::mat4& model) const
	{
		glm::vec4 max = model * glm::vec4(m_AABB.Max, 1.0f);
		return glm::vec3(max / max.w);
	}

	AABB BoundingVolume::GetAABB(const glm::mat4& model) const
	{
		AABB aabb = {
			this->GetMax(model),
			this->GetMin(model)
		};
		return aabb;
	}

	Sphere BoundingVolume::GetSphere(const glm::mat4& model) const
	{
		Sphere sphere = {
			this->GetCenter(model),
			this->GetRadius()
		};
		return sphere;
	}

	glm::vec3 BoundingVolume::GetCenter(const glm::mat4& model) const
	{
		glm::vec4 centerModel = model * glm::vec4(m_Sphere.Center, 1.0f);
		return glm::vec3(centerModel / centerModel.w);
	}

	bool BoundingVolume::GetRadius(const glm::mat4& model, const glm::mat4& inverseViewProjtion) const
	{
		return false;
	}

	void BoundingVolume::operator=(const BoundingVolume& rigth)
	{
		m_AABB = rigth.m_AABB;
		m_Sphere = rigth.m_Sphere;
	}

	

	void BoundingVolume::CalculatMinMax(const std::vector<unsigned char>& data, uint32_t offset, BufferElement element)
	{
		switch (element.type)
		{
		case ShaderDataType::Float2:
		{
			glm::vec<2, float>* value = (glm::vec<2, float>*)(data.data() + offset);
			CheckValues(value);

			break;
		}
		case ShaderDataType::Float3:
		{
			glm::vec<3, float>* value = (glm::vec<3, float>*)(data.data() + offset);
			CheckValues(value);
			break;
		}
		case ShaderDataType::Float4:
		{
			glm::vec<4, float>* value = (glm::vec<4, float>*)(data.data() + offset);
			CheckValues(value);
			break;
		}

		case ShaderDataType::Int2:
		{
			glm::vec<2, int>* value = (glm::vec<2, int>*)(data.data() + offset);
			CheckValues(value);
			break;
		}
		case ShaderDataType::Int3:
		{
			glm::vec<3, int>* value = (glm::vec<3, int>*)(data.data() + offset);
			CheckValues(value);
			break;
		}
		case ShaderDataType::Int4:
		{
			glm::vec<4, int>* value = (glm::vec<4, int>*)(data.data() + offset);
			CheckValues(value);
			break;
		}

		case ShaderDataType::Uint2:
		{
			glm::vec<2, uint32_t>* value = (glm::vec<2, uint32_t>*)(data.data() + offset);
			CheckValues(value);
			break;
		}
		case ShaderDataType::Uint3:
		{
			glm::vec<3, uint32_t>* value = (glm::vec<3, uint32_t>*)(data.data() + offset);
			CheckValues(value);
			break;
		}
		case ShaderDataType::Uint4:
		{
			glm::vec<4, uint32_t>* value = (glm::vec<4, uint32_t>*)(data.data() + offset);
			CheckValues(value);
			break;
		}
		default:
		{
			RY_CORE_ASSERT(false, ("Not Seport Data Formate {0}, for Box Min Max", (int)element.type));
			break;
		}
		}
	}

	void BoundingVolume::FindeRadius(const std::vector<unsigned char>& data, uint32_t offset, BufferElement element)
	{
		switch (element.type)
		{
		case ShaderDataType::Float2:
		{
			RY_CORE_NOT_IMPL();
			break;
		}
		case ShaderDataType::Float3:
		{
			glm::vec<3, float>* value = (glm::vec<3, float>*)(data.data() + offset);
			CheckRadius(value);
			break;
		}
		case ShaderDataType::Float4:
		{
			RY_CORE_NOT_IMPL();
			break;
		}

		case ShaderDataType::Int2:
		{
			RY_CORE_NOT_IMPL();
			break;
		}
		case ShaderDataType::Int3:
		{
			RY_CORE_NOT_IMPL();
			break;
		}
		case ShaderDataType::Int4:
		{
			RY_CORE_NOT_IMPL();
			break;
		}

		case ShaderDataType::Uint2:
		{
			RY_CORE_NOT_IMPL();
			break;
		}
		case ShaderDataType::Uint3:
		{
			RY_CORE_NOT_IMPL();
			break;
		}
		case ShaderDataType::Uint4:
		{
			RY_CORE_NOT_IMPL();
			break;
		}
		default:
		{
			RY_CORE_ASSERT(false, ("Not Seport Data Formate {0}, for Box Min Max", (int)element.type));
			break;
		}
		}
	}
	void BoundingVolume::CheckValues(glm::vec<2, float>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
	}

	void BoundingVolume::CheckValues(glm::vec<3, float>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
		Utils::CheckAndSetAABB(&m_AABB.Min.z, &m_AABB.Max.z, value->z);
	}

	void BoundingVolume::CheckValues(glm::vec<4, float>* value)
	{

		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
		Utils::CheckAndSetAABB(&m_AABB.Min.z, &m_AABB.Max.z, value->z);
	}


	void BoundingVolume::CheckValues(glm::vec<2, int>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
	}

	void BoundingVolume::CheckValues(glm::vec<3, int>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
		Utils::CheckAndSetAABB(&m_AABB.Min.z, &m_AABB.Max.z, value->z);
	}

	void BoundingVolume::CheckValues(glm::vec<4, int>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
		Utils::CheckAndSetAABB(&m_AABB.Min.z, &m_AABB.Max.z, value->z);
	}


	void BoundingVolume::CheckValues(glm::vec<2, uint32_t>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
	}

	void BoundingVolume::CheckValues(glm::vec<3, uint32_t>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
		Utils::CheckAndSetAABB(&m_AABB.Min.z, &m_AABB.Max.z, value->z);
	}

	void BoundingVolume::CheckValues(glm::vec<4, uint32_t>* value)
	{
		Utils::CheckAndSetAABB(&m_AABB.Min.x, &m_AABB.Max.x, value->x);
		Utils::CheckAndSetAABB(&m_AABB.Min.y, &m_AABB.Max.y, value->y);
		Utils::CheckAndSetAABB(&m_AABB.Min.z, &m_AABB.Max.z, value->z);
	}

	void BoundingVolume::CheckRadius(glm::vec<3, float>* value)
	{
		glm::vec3& position = *value;
		glm::vec3& center = m_Sphere.Center;
		float& radius = m_Sphere.Radius;

		float distenc = glm::length(glm::abs(position - center));
		if (radius < distenc)
			radius = distenc;

	}

}
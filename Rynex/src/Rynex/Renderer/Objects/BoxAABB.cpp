#include "rypch.h"
#include "BoxAABB.h"

#include "Rynex/Core/Assert.h"

namespace Rynex {

#define RY_MAX_FLOAT 1000.0f
#define RY_MIN_FLOAT -1000.0f

	namespace Utils {

		static void CheckAndSetAABB(float* min, float* max, float value)
		{
			if (value > *max)
				*max = value;
			if (value < *min)
				*min = value;
		}

	}

	BoxAABB::BoxAABB()
		: m_Max(RY_MIN_FLOAT, RY_MIN_FLOAT, RY_MIN_FLOAT)
		, m_Min(RY_MAX_FLOAT, RY_MAX_FLOAT, RY_MAX_FLOAT)
	{
	}

	BoxAABB::~BoxAABB()
	{
	}

	BoxAABB::BoxAABB(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement)
		: m_Max(RY_MIN_FLOAT, RY_MIN_FLOAT, RY_MIN_FLOAT)
		, m_Min(RY_MAX_FLOAT, RY_MAX_FLOAT, RY_MAX_FLOAT)
	{
		SetBoxAABB(vertexBuffer, postionElement);
	}

	void BoxAABB::SetBoxAABB(const Ref<VertexBuffer>& vertexBuffer, BufferElement& postionElement)
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
					CalculatMinMax(data, i + postionElement.Offset, postionElement);
					
				}
				glm::vec3 differenze = m_Min-m_Max;
				glm::vec3 half = glm::vec3(2.0f, 2.0f, 2.0f)/differenze;
				m_CenterPoint = m_Max + half;
				break;
			}
		}
		
		RY_CORE_INFO("Succesfull Create AABB Box! Min({0}, {1}, {2})  Max({3}, {4}, {5})", m_Min.x, m_Min.y, m_Min.z, m_Max.x, m_Max.y, m_Max.z);
	}

	void BoxAABB::CalculatMinMax(const std::vector<unsigned char>& data, uint32_t offset, BufferElement element)
	{
		switch (element.Type)
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
			RY_CORE_ASSERT(false, ("Not Seport Data Formate {0}, for Box Min Max", (int)element.Type));
			break;
		}
		}
	}
#if 0
	template<int L, typename T>
	void BoxAABB::CheckValues<L, T>(glm::vec<L, T>* value)
	{
		static_assert(false);
	}
#endif
#if 1
	void BoxAABB::CheckValues(glm::vec<2, float>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
	}

	void BoxAABB::CheckValues(glm::vec<3, float>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
		Utils::CheckAndSetAABB(&m_Min.z, &m_Max.z, value->z);
	}

	void BoxAABB::CheckValues(glm::vec<4, float>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
		Utils::CheckAndSetAABB(&m_Min.z, &m_Max.z, value->z);
	}


	void BoxAABB::CheckValues(glm::vec<2, int>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
	}

	void BoxAABB::CheckValues(glm::vec<3, int>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
		Utils::CheckAndSetAABB(&m_Min.z, &m_Max.z, value->z);
	}

	void BoxAABB::CheckValues(glm::vec<4, int>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
		Utils::CheckAndSetAABB(&m_Min.z, &m_Max.z, value->z);
	}


	void BoxAABB::CheckValues(glm::vec<2, uint32_t>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
	}

	void BoxAABB::CheckValues(glm::vec<3, uint32_t>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
		Utils::CheckAndSetAABB(&m_Min.z, &m_Max.z, value->z);
	}

	void BoxAABB::CheckValues(glm::vec<4, uint32_t>* value)
	{
		Utils::CheckAndSetAABB(&m_Min.x, &m_Max.x, value->x);
		Utils::CheckAndSetAABB(&m_Min.y, &m_Max.y, value->y);
		Utils::CheckAndSetAABB(&m_Min.z, &m_Max.z, value->z);
	}
#endif
}
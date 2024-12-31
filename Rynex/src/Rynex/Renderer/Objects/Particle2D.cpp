#include "rypch.h"
#include "Particle2D.h"

#include "Rynex/Renderer/Rendering/Renderer2D.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include "Rynex/Utils/Random.h"

namespace Rynex {

#if 0
	class Random
	{
	public:
		static void Init()
		{
			s_RandomeEngine.seed(std::random_device()());
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomeEngine) / (float)std::numeric_limits<uint32_t>::max();
		}

		static float Float(float maxValue)
		{
			return (maxValue/(float)s_Distribution(s_RandomeEngine) / (float)std::numeric_limits<uint32_t>::max());
		}

		static float Float(float center, float aabbRanage)
		{
			return (Float(1.0f) * 2 - 1) * aabbRanage + center;
		}

		
		static glm::vec3 Float3(const glm::vec3& center, float aabbRanage)
		{
			return {
				(Float(1.0f) * 2 - 1) * aabbRanage + center.x,
				(Float(1.0f) * 2 - 1) * aabbRanage + center.y,
				(Float(1.0f) * 2 - 1) * aabbRanage + center.z 
			};
		}

		static glm::vec3 Float3(const glm::vec3& center, const glm::vec3& aabbRanage)
		{
			return {
				(Float(1.0f) * 2 - 1) * aabbRanage.x + center.x,
				(Float(1.0f) * 2 - 1) * aabbRanage.y + center.y,
				(Float(1.0f) * 2 - 1) * aabbRanage.z + center.z
			};
		}


		static glm::vec4 Float4(const glm::vec4& center, float aabbRanage)
		{
			return {
				(Float(1.0f) * 2 - 1) * aabbRanage + center.x,
				(Float(1.0f) * 2 - 1) * aabbRanage + center.y,
				(Float(1.0f) * 2 - 1) * aabbRanage + center.z,
				(Float(1.0f) * 2 - 1) * aabbRanage + center.w
			};
		}

		static glm::vec4 Float4(const glm::vec4& center, const glm::vec4& aabbRanage)
		{
			return {
				(Float(1.0f) * 2 - 1) * aabbRanage.x + center.x,
				(Float(1.0f) * 2 - 1) * aabbRanage.y + center.y,
				(Float(1.0f) * 2 - 1) * aabbRanage.z + center.z,
				(Float(1.0f) * 2 - 1) * aabbRanage.w + center.w
			};
		}

	private:
		static std::mt19937 s_RandomeEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};

	static std::mt19937 s_RandomeEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
#endif

	Particle2D::Particle2D()
	{
		
	}

	Particle2D::Particle2D(uint32_t size)
		: m_MaxParticle(size)
	{
		m_Particle.resize(size);
	}

	void Particle2D::Emite(uint32_t particleNew)
	{
		m_CountParticle += particleNew;
		if (m_CountParticle > m_MaxParticle)
		{
			m_Particle.resize(m_CountParticle);
			m_MaxParticle = m_CountParticle;
		}
		std::vector<Particle2DData>::iterator particleListEnd = m_Particle.end();
		if (m_Allive._Ptr == nullptr)
			m_Allive = m_Particle.begin();
		for (; m_Allive != particleListEnd && 0 < particleNew ; particleNew--, ++m_Allive)
		{
			AddParticle(*m_Allive);
		}
	}

	void Particle2D::UpdatePatricles(float ts, glm::vec3 camerPos)
	{
		if (m_Allive != m_Particle.begin())
			return;

		std::vector<Particle2DData>::iterator particleListEnd = m_Allive._Ptr != nullptr ? m_Allive : m_Particle.end();
		
		for (std::vector<Particle2DData>::iterator it = m_Particle.begin(); it != particleListEnd; ++it)
		{
			Particle2DData& particle = *it;
			if (!particle.Alive)
				break;

			particle.Lifetime += ts;
			if(particle.Lifetime > particle.MaxLifeTime)
			{
				particle.Alive = false;
				continue;
			}

			particle.Position += particle.Vilocity * ts;
			float life = particle.MaxLifeTime / particle.Lifetime;
			particle.CurrenteColor = glm::lerp(particle.BeginColor, particle.EndeColor, life);
			particle.Size = glm::lerp(particle.BeginSize, particle.EndeSize, life);
			particle.DistenceCamera = glm::length(particle.Position - camerPos);
			particle.Rotation += ts;
		}
		m_Allive = std::partition(m_Particle.begin(), m_Particle.end(), IsParticleAlive);


		if (m_Allive != m_Particle.begin())
			RY_CORE_WARN("Zero Particle in Use Now");


		std::sort(m_Particle.begin(), m_Allive, SortParticle);
		

	}

	void Particle2D::Draw(const glm::mat4& tranform, int entityID)
	{
		std::vector<Particle2DData>::iterator particleListEnd = m_Particle.end();
		for (std::vector<Particle2DData>::iterator it = m_Particle.begin(); it != particleListEnd; ++it)
		{
			Particle2DData& partikle = *it;
			Renderer2D::DrawParticle(tranform, partikle.Position, partikle.CurrenteColor, partikle.Size, partikle.Rotation, m_Texture, entityID);
		}
	}

	void Particle2D::AddParticle(Particle2DData& particle)
	{
		particle.Alive = true;

		particle.BeginColor = Random::Float4(m_PropsParticle.BeginColor, 0.1f);
		particle.EndeColor = Random::Float4(m_PropsParticle.EndeColor, 0.1f);

		particle.BeginSize = Random::Float(m_PropsParticle.BeginSize, m_PropsParticle.AABBSize);
		particle.Vilocity = Random::Float3({0.0f,1.0f,0.0f}, m_PropsParticle.AABBVilocity);
		particle.MaxLifeTime = Random::Float(m_PropsParticle.MaxLifeTime, m_PropsParticle.AABBLifeTime);
		particle.Lifetime = 0.0f;
		particle.DistenceCamera = -1.0f;
		particle.Position = Random::Float3({ 0.0f,0.0f,0.0f }, 0.5f);;
		particle.Rotation = Random::Float(1);
		
	}

	float Particle2D::SortParticle(const Particle2DData& a, const Particle2DData& b)
	{
		return a.DistenceCamera > b.DistenceCamera;
	}

	bool Particle2D::IsParticleAlive(const Particle2DData& p)
	{
		return p.Alive;
	}

}
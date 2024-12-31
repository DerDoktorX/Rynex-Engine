#pragma once
#include <Rynex/Renderer/API/Texture.h>

namespace Rynex {

	struct Particle2DData
	{
		glm::vec3 Position;
		glm::vec4 BeginColor;
		glm::vec4 EndeColor;
		glm::vec4 CurrenteColor;
		glm::vec3 Vilocity;
		float BeginSize;
		float EndeSize;
		float Size;
		float Rotation;
		float Lifetime;
		float MaxLifeTime;
		float DistenceCamera = 0;
		bool Alive = true;
	};

	struct Particle2DIntData
	{
		float AABBPosition;
		float AABBVilocity;
		glm::vec3 MaxVilocity;
		glm::vec4 BeginColor;
		glm::vec4 EndeColor;
		float AABBSize;
		float BeginSize;
		float EndeSize;
		float Rotation;
		float AABBLifeTime;
		float MaxLifeTime;
	};

	class Particle2D
	{
	public:
		Particle2D();
		Particle2D(uint32_t size);
		void Emite(uint32_t particleNew = 1);
		void UpdatePatricles(float ts, glm::vec3 camerPos);
		void Draw(const glm::mat4& tranform, int entityID);

		
	private:
		void AddParticle(Particle2DData& particle);
		static float SortParticle(const Particle2DData& a, const Particle2DData& b);
		static bool IsParticleAlive(const Particle2DData& p);
		
	private:
		std::vector<Particle2DData> m_Particle;
		Ref<Texture> m_Texture = nullptr;
		std::vector<Particle2DData>::iterator m_Allive;
		uint32_t m_MaxParticle = 0;
		uint32_t m_CountParticle = 0;
		uint32_t m_AliveParticles = 0;
		Particle2DIntData m_PropsParticle;
	};

}


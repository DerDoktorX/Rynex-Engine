#pragma once

#include "Rynex/Renderer/API/Shader.h"

namespace Rynex {
	class Material : public Asset
	{
	public:
		Material();
	private:
		Ref<Shader> m_Shader;
		Ref<Texture> m_AlbedoMap;
		Ref<Texture> m_NormaleMap;
		Ref<Texture> m_MetalnessMap;
		Ref<Texture> m_RoughnessMap;
		float m_Metalness;
		float m_Roughness;
		bool m_UseNormaleMap;
		float m_Emssion;
		glm::vec3 m_AlbedoColor;
	};
}


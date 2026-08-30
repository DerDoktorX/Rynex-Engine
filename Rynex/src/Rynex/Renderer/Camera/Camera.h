#pragma once

#include <glm/glm.hpp>

namespace Rynex {

	class RYNEX_API Camera
	{
	public:
		enum Update : int
		{
			None = 0
			, Projtion = BIT(0)
			, View = BIT(1)
			, ViewProjtion = Projtion | View
		};
	public:
		Camera() = default;
		Camera(const glm::mat4& projektion)
			: m_Projektion(projektion)
			, m_Change(Update::Projtion) {}

		int NeedUpdateProjektion() const { return m_Change; }
		const glm::mat4& GetProjektion() const { return m_Projektion; }
		const glm::mat4& GetProjektion() 
		{ 
			m_Change = (Update)BIT_NOT(BIT_AND(m_Change, Update::Projtion));
			return m_Projektion; 
		}
	protected:
		glm::mat4 m_Projektion = glm::mat4(1.0f); 
		Update m_Change = Update::Projtion;
	};

	

}
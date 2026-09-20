#pragma once



namespace Rynex {

	class RYNEX_API Camera
	{
	public:
		enum Update : int
		{
			None = 0
			, Projection = BIT(0)
			, View = BIT(1)
			, ViewProjection = Projection | View
		};
	// public methode ---------------------------------------------------------------------------------------------------------
		Camera() = default;

        explicit Camera(const glm::mat4& projection)
			: m_Projection(projection)
			, m_Change(Update::Projection) {}

		int NeedUpdateProjection() const { return m_Change; }
		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetProjection()
		{ 
			m_Change = static_cast<Update>(
			    BIT_NOT(
			        BIT_AND(m_Change, Update::Projection)
			    )
			);
			return m_Projection;
		}
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		Update m_Change = Update::Projection;
	};

	

}
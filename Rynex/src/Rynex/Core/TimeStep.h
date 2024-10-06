#pragma once
namespace Rynex {
	class RYNEX_API TimeStep
	{
	public:
		TimeStep(float delta = 0.0f, double alpha = 0.0)
			: m_Delta(delta)
			, m_Alpha(alpha)
		{
		}

		//TimeStep(const TimeStep&) = default;
		//TimeStep(TimeStep&&) = default;

		operator float() { return m_Delta; }
		float GetSecounds() const { return m_Delta; }
		float GetMillsecounds() const { return m_Delta * 1000.0f; }
		float GetFPS() const { return 1.0f/m_Delta; }
		double GetAppRuningTime() const { return m_Alpha; }
	private:
		float m_Delta;
		double m_Alpha;
	};
}



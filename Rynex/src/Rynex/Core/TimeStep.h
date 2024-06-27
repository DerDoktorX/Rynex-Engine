#pragma once
namespace Rynex {
	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f)
			: m_Time(time)
		{
		}
		operator float() { return m_Time; }
		float GetSecounds() const { return m_Time; }
		float GetMillsecounds() const { return m_Time * 1000.0f; }

	private:
		float m_Time;
	};
}



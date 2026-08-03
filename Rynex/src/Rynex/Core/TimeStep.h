#pragma once
namespace Rynex {

	class RYNEX_API TimeStep
	{
	public:
		TimeStep()
			: m_Delta(0.0f)
			, m_Alpha(0.0)
		{
		}

		TimeStep(float delta)
			: m_Delta(delta)
			, m_Alpha(0.0)
		{
		}

		TimeStep(float delta, double alpha)
			: m_Delta(delta)
			, m_Alpha(alpha)
		{
		}

		static TimeStep CreateMin()
		{
			constexpr double alpha = std::numeric_limits<double>::lowest();
			constexpr float delta = std::numeric_limits<float>::lowest();
			TimeStep timeStepMin =TimeStep(delta, alpha);
			return timeStepMin;
		}

		static TimeStep CreateMax()
		{
			constexpr double alpha = std::numeric_limits<double>::max();
			constexpr float delta = std::numeric_limits<float>::max();
			TimeStep timeStepMax = TimeStep(delta, alpha);
			return timeStepMax;
		}


		operator float() { return m_Delta; }
		float GetSecounds() const { return m_Delta; }
		float GetMillsecounds() const { return m_Delta * 1000.0f; }
		float GetFPS() const { return 1.0f/m_Delta; }
		double GetAppRuningTime() const { return m_Alpha; }
	private:
		float m_Delta;
		double m_Alpha;

	private:
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(TimeStep, bool, ==);
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(TimeStep, bool, !=);

		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(TimeStep, bool, < );
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(TimeStep, bool, <=);

		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(TimeStep, bool, > );
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(TimeStep, bool, >=);

	};

	RY_NONE_MEBER_OPERATOR_BOOL(TimeStep, ==, &&, m_Alpha, m_Delta);
	RY_NONE_MEBER_OPERATOR_BOOL(TimeStep, != , &&, m_Alpha, m_Delta);
	RY_NONE_MEBER_OPERATOR_BOOL(TimeStep, < , &&, m_Alpha, m_Delta);
	RY_NONE_MEBER_OPERATOR_BOOL(TimeStep, <=, &&, m_Alpha, m_Delta);
	RY_NONE_MEBER_OPERATOR_BOOL(TimeStep, > , &&, m_Alpha, m_Delta);
	RY_NONE_MEBER_OPERATOR_BOOL(TimeStep, >=, &&, m_Alpha, m_Delta);



	


	
}



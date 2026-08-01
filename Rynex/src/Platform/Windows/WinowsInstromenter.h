#pragma once

#include "Rynex/Core/Instrumentor.h"

namespace Rynex{

	class WinowsInstromenterTimer : public PlatformTimer
	{
	private:
		using TimePoint = std::chrono::steady_clock::time_point;
		using TimeUnit = std::chrono::nanoseconds;

	public:
		WinowsInstromenterTimer(int64_t* timeElaps)
			: m_TimeElapsPtr(timeElaps)
			, m_Stopped(true)
		{
			Start();
		};

	

		virtual ~WinowsInstromenterTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		virtual void Start() override
		{
			if (!m_Stopped)
				Stop();

			Continue();
			Rest();
		}

		virtual void Start(int64_t* timeElaps) override
		{
			SetTimePtr(timeElaps);
			if (!m_Stopped)
				Stop();

			Continue();
			Rest();
		}

		virtual void Stop() override
		{
			
#if 0
			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
#endif
			CurentPastTime();
			m_Stopped = true;
		}
		
		virtual void SetTimePtr(int64_t* timeElaps)
		{
			m_TimeElapsPtr = timeElaps;
		}
		
		virtual int64_t GetCurentTime() const override
		{
			
			TimePoint endTimePoint = std::chrono::high_resolution_clock::now();

			
#if 1
			TimeUnit timeNanoSec = std::chrono::duration_cast<TimeUnit>(endTimePoint - m_StartTimepoint);
			int64_t result = timeNanoSec.count();
#else
			using Duration = std::chrono::steady_clock::duration;
			TimePoint startTime = std::chrono::time_point_cast<TimeUnit>(m_StartTimepoint);
			Duration startDuration = startTime.time_since_epoch();
			int64_t start = startDuration.count();

			TimePoint endTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint);
			Duration startDuration = endTime.time_since_epoch();
			int64_t end = startDuration.count();
			int64_t result = end - start;
#endif
			return result;
		}

		virtual void CurentPastTime() override
		{
			int64_t result = GetCurentTime();
			if (nullptr != m_TimeElapsPtr)
			{
				*m_TimeElapsPtr = result;
			}
		}

		virtual void Continue() override
		{
			m_Stopped = false;
		}

		virtual void Rest() override
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

	private:
		int64_t* m_TimeElapsPtr;
		TimePoint m_StartTimepoint;

		bool m_Stopped;
	};

}
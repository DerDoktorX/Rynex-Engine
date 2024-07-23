#pragma once

#include "Rynex/Core/Instrumentor.h"

namespace Rynex{

	class WinowsInstromenterTimer : public PlatformTimer
	{
	public:
		WinowsInstromenterTimer(const char* name)
			: m_Name(name)
			, m_Stopped(false)
		{
			QueryPerformanceFrequency(&m_Frequency);
			QueryPerformanceCounter(&m_StartCount);
			// m_StartTimepoint = std::chrono::high_resolution_clock::now();
		};

		WinowsInstromenterTimer()
		{
			if (!m_Stopped)
				Stop();
		}

		void Stop()
		{
			// auto endTimePoint = std::chrono::high_resolution_clock::now();
			LARGE_INTEGER endTimePoint;
			QueryPerformanceCounter(&endTimePoint);
			// long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			// long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
			//long long start = (long long)endTimePoint;

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			//Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

			m_Stopped = true;
		}

	private:
		const char* m_Name;
		// std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		LARGE_INTEGER m_Frequency;
		LARGE_INTEGER m_StartCount;
		bool m_Stopped;
	};

}
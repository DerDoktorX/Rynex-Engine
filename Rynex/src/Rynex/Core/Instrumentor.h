#pragma once

#include "Rynex/Core/Log.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>


namespace Rynex {

	struct ProfileResult
	{
		std::string Name;
		long long Start, End;
		uint32_t ThreadID;
	};

	struct InstrumentationSession
	{
		std::string Name;
	};

	class Instrumentor
	{
	public:
		Instrumentor(const Instrumentor&) = delete;
		Instrumentor(Instrumentor&&) = delete;


		void BegineSession(const std::string& name, const std::string& filePath = "results.json")
		{
			std::lock_guard lock(m_Mutex);
			if (m_CurrentSession)
			{
				// If there is already a current session, then close it before beginning new one.
				// Subsequent profiling output meant for the original session will end up in the
				// newly opened session instead.  That's better than having badly formatted
				// profiling output.
				if (Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
				{
						RY_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name.c_str());
				}
				InternalEndSession();
			}
			m_OutputStream.open(filePath);

			if (m_OutputStream.is_open())
			{
				RY_CORE_MEMORY_ALICATION("m_CurrentSession", "Instrumentor::BegineSession", InstrumentationSession);
				m_CurrentSession = new InstrumentationSession({ name });
				WriteHeader();
			}
			else
			{
				if (Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
				{
					RY_CORE_ERROR("Instrumentor could not open results file '{0}'.", filePath.c_str());
				}
			}
		}

		void EndSession()
		{
#if 0
			WriteFooter();
			m_OutputStream.close();
			delete m_CurrentSession;
			m_CurrentSession = nullptr;
			m_ProfileCount = 0;
#endif
			std::lock_guard lock(m_Mutex);
			InternalEndSession();

		}

		void WriteProfile(const ProfileResult& result)
		{
			std::stringstream json;

			json << std::setprecision(3) << std::fixed;
			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.End - result.Start) << ',';
			json << "\"name\":\"" << result.Name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.ThreadID << ",";
			json << "\"ts\":" << result.Start;
			json << "}";

			std::lock_guard lock(m_Mutex);
			if (m_CurrentSession)
			{
				m_OutputStream << json.str();
				m_OutputStream.flush();
			}
		}
	
		static Instrumentor& Get()
		{
			static Instrumentor instance;
			return instance;
		}

	private:
		Instrumentor()
			: m_CurrentSession(nullptr)
		{
		}

		~Instrumentor()
		{
			EndSession();
		}

		void WriteHeader()
		{
			m_OutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
			m_OutputStream.flush();
		}

		void WriteFooter()
		{
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		void InternalEndSession()
		{
			if (m_CurrentSession)
			{
				WriteFooter();
				m_OutputStream.close();
				RY_CORE_MEMORY_FREE("m_CurrentSession", "Instrumentor::BegineSession");
				delete m_CurrentSession;
				m_CurrentSession = nullptr;
			}
		}

	private:
		std::mutex m_Mutex;
		InstrumentationSession* m_CurrentSession;
		std::ofstream m_OutputStream;
	};

	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char* name)
			: m_Name(name)
			, m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!m_Stopped)
				Stop();
			delete[] m_Name;
		}

		void Stop()
		{
			auto endTimePoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });
			
			m_Stopped = true;
		}
	private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		bool m_Stopped;
		
	};

	class PlatformTimer
	{
	public:
		static Scope<PlatformTimer> Create(const char* name);
		virtual ~PlatformTimer() {};

		virtual void Stop() = 0;
	};
}

#if RY_PROFILE

	#define RY_PROFILE_BEGIN_SESSION(name, filepath)		::Rynex::Instrumentor::Get().BegineSession(name, filepath)
	#define RY_PROFILE_END_SESSION()						::Rynex::Instrumentor::Get().EndSession()
#if RY_PLATFORM_PROFILER
	#define RY_PROFILE_SCOPE(name)							::Rynex::PlatformTimer timer##__LINE__(name);
#else
#define RY_PROFILE_SCOPE(name)								::Rynex::InstrumentationTimer timer##__LINE__(name);
#endif
#define RY_PROFILE_FUNCTION()								RY_PROFILE_SCOPE(__FUNCSIG__)



#else
#define RY_PROFILE_BEGIN_SESSION(name, filepath)
#define RY_PROFILE_END_SESSION()
#define RY_PROFILE_SCOPE(name)
#define RY_PROFILE_FUNCTION()
#endif


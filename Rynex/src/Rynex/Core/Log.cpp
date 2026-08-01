#include "rypch.h"
#include "Rynex/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Rynex {

#if 0
	Ref<spdlog::logger>			Log::s_CoreLogger;
	Ref<spdlog::logger>			Log::s_ClientLogger;

	Ref<spdlog::logger>			Log::s_CoreGraficsCreateLogger;
	Ref<spdlog::logger>			Log::s_CoreGraficsDeleteLogger;
	Ref<spdlog::logger>			Log::s_CoreAssetLogger;
	Ref<spdlog::logger>			Log::s_CoreMemoryLogger;
	Ref<spdlog::logger>			Log::s_ThreadLogger;
	std::atomic<uint32_t>		Log::s_NummberFormating;
#else
	Log Log::s_LogInstance;
#endif
#ifdef RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_SYMBOL && RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_COUNT
	struct CustomNumpunct : std::numpunct<char> 
	{
	protected:
		char do_thousands_sep() const override { return RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_SYMBOL; } // Trennzeichen
		std::string do_grouping() const override { return RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_COUNT; }
		
	};
#endif

	Log::Log()
	{
#if 0
		spdlog::set_pattern("%^[%T] %n: %v%$");
		m_CoreLogger = spdlog::stdout_color_mt("Rynex");
		m_CoreLogger->set_level(spdlog::level::trace);

		m_ClientLogger = spdlog::stdout_color_mt("App");
		m_ClientLogger->set_level(spdlog::level::trace);
#else
#ifdef RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_SYMBOL && RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_COUNT
		{
			std::locale defaultLocaleC = std::locale("C");
			CustomNumpunct* numPunctPtr = new CustomNumpunct();
			std::locale customLocale = std::locale(defaultLocaleC, numPunctPtr);

			std::locale::global(customLocale);
		}
#elif RY_CONSOLE_LOG_NUMBER
		m_NummberFormating = 1u;
#else
		m_NummberFormating = 0u;
#endif
		{
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Rynex.log", true));


			logSinks[0]->set_pattern("%^[%T] %n: %v%$");
			logSinks[1]->set_pattern("[%T] [%l] %n: %v");

			m_CoreLogger = std::make_shared<spdlog::logger>("RYNEX", begin(logSinks), end(logSinks));
			spdlog::register_logger(m_CoreLogger);
			m_CoreLogger->set_level(spdlog::level::trace);
			m_CoreLogger->flush_on(spdlog::level::trace);

			m_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
			spdlog::register_logger(m_ClientLogger);
			m_ClientLogger->set_level(spdlog::level::trace);
			m_ClientLogger->flush_on(spdlog::level::trace);
		}


		{
			{
				std::vector<spdlog::sink_ptr> logSinksGrafics;
				// logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
				logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexGraficCreate.log", true));
				// logSinksGrafics[0]->set_pattern("%^[%T] %n: %v%$");
				logSinksGrafics[0]->set_pattern("[%T] [%l] %n: %v");

				m_CoreGraficsCreateLogger = std::make_shared<spdlog::logger>("CREATE", begin(logSinksGrafics), end(logSinksGrafics));

				spdlog::register_logger(m_CoreGraficsCreateLogger);
				m_CoreGraficsCreateLogger->set_level(spdlog::level::trace);
				m_CoreGraficsCreateLogger->flush_on(spdlog::level::trace);

			}

			{
				std::vector<spdlog::sink_ptr> logSinksGrafics;
				// logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
				logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexGraficDelete.log", true));
				// logSinksGrafics[0]->set_pattern("%^[%T] %n: %v%$");
				logSinksGrafics[0]->set_pattern("[%T] [%l] %n: %v");

				m_CoreGraficsDeleteLogger = std::make_shared<spdlog::logger>("DELETE", begin(logSinksGrafics), end(logSinksGrafics));

				spdlog::register_logger(m_CoreGraficsDeleteLogger);
				m_CoreGraficsDeleteLogger->set_level(spdlog::level::trace);
				m_CoreGraficsDeleteLogger->flush_on(spdlog::level::trace);
			}
		}

		{
			std::vector<spdlog::sink_ptr> logSinksAsset;

			logSinksAsset.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinksAsset.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexAsset.log", true));
			logSinksAsset[0]->set_pattern("%^[%T] %n: %v%$");
			logSinksAsset[1]->set_pattern("[%T] [%l] %n: %v");

			m_CoreAssetLogger = std::make_shared<spdlog::logger>("[RYNEX][ASSET]", begin(logSinksAsset), end(logSinksAsset));

			spdlog::register_logger(m_CoreAssetLogger);
			m_CoreAssetLogger->set_level(spdlog::level::trace);
			m_CoreAssetLogger->flush_on(spdlog::level::trace);
		}

		{
			std::vector<spdlog::sink_ptr> logSinksThread;

			logSinksThread.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinksThread.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexThread.log", true));
			logSinksThread[0]->set_pattern("%^[%T] %n: %v%$");
			logSinksThread[1]->set_pattern("[%T] [%l] %n: %v");

			m_ThreadLogger = std::make_shared<spdlog::logger>("[RYNEX][THREAD]", begin(logSinksThread), end(logSinksThread));

			spdlog::register_logger(m_ThreadLogger);
			m_ThreadLogger->set_level(spdlog::level::trace);
			m_ThreadLogger->flush_on(spdlog::level::trace);


		}
#endif
	}

	Log::~Log()
	{
	}

	Log& Log::Get()
	{
		return s_LogInstance;
	}

#if 0
	void Log::Init()
	{

#if 0
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("Rynex");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("App");
		s_ClientLogger->set_level(spdlog::level::trace);
#else
#ifdef RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_SYMBOL && RY_CONSOLE_LOG_LARGE_NUMBER_GRUPING_COUNT
		{
			std::locale defaultLocaleC = std::locale("C");
			CustomNumpunct* numPunctPtr = new CustomNumpunct();
			std::locale customLocale = std::locale(defaultLocaleC, numPunctPtr);

			std::locale::global(customLocale);
		}
#elif RY_CONSOLE_LOG_NUMBER
		s_NummberFormating = 1u;
#else
		s_NummberFormating = 0u;
#endif
		{
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Rynex.log", true));


			logSinks[0]->set_pattern("%^[%T] %n: %v%$");
			logSinks[1]->set_pattern("[%T] [%l] %n: %v");

			s_CoreLogger = std::make_shared<spdlog::logger>("RYNEX", begin(logSinks), end(logSinks));
			spdlog::register_logger(s_CoreLogger);
			s_CoreLogger->set_level(spdlog::level::trace);
			s_CoreLogger->flush_on(spdlog::level::trace);

			s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
			spdlog::register_logger(s_ClientLogger);
			s_ClientLogger->set_level(spdlog::level::trace);
			s_ClientLogger->flush_on(spdlog::level::trace);
		}


		{
			{
				std::vector<spdlog::sink_ptr> logSinksGrafics;
				// logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
				logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexGraficCreate.log", true));
				// logSinksGrafics[0]->set_pattern("%^[%T] %n: %v%$");
				logSinksGrafics[0]->set_pattern("[%T] [%l] %n: %v");

				s_CoreGraficsCreateLogger = std::make_shared<spdlog::logger>("CREATE", begin(logSinksGrafics), end(logSinksGrafics));

				spdlog::register_logger(s_CoreGraficsCreateLogger);
				s_CoreGraficsCreateLogger->set_level(spdlog::level::trace);
				s_CoreGraficsCreateLogger->flush_on(spdlog::level::trace);

			}

			{
				std::vector<spdlog::sink_ptr> logSinksGrafics;
				// logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
				logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexGraficDelete.log", true));
				// logSinksGrafics[0]->set_pattern("%^[%T] %n: %v%$");
				logSinksGrafics[0]->set_pattern("[%T] [%l] %n: %v");

				s_CoreGraficsDeleteLogger = std::make_shared<spdlog::logger>("DELETE", begin(logSinksGrafics), end(logSinksGrafics));

				spdlog::register_logger(s_CoreGraficsDeleteLogger);
				s_CoreGraficsDeleteLogger->set_level(spdlog::level::trace);
				s_CoreGraficsDeleteLogger->flush_on(spdlog::level::trace);
			}
		}

		{
			std::vector<spdlog::sink_ptr> logSinksAsset;
			
			logSinksAsset.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinksAsset.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexAsset.log", true));
			logSinksAsset[0]->set_pattern("%^[%T] %n: %v%$");
			logSinksAsset[1]->set_pattern("[%T] [%l] %n: %v");

			s_CoreAssetLogger = std::make_shared<spdlog::logger>("[RYNEX][ASSET]", begin(logSinksAsset), end(logSinksAsset));

			spdlog::register_logger(s_CoreAssetLogger);
			s_CoreAssetLogger->set_level(spdlog::level::trace);
			s_CoreAssetLogger->flush_on(spdlog::level::trace);
		}

		{
			std::vector<spdlog::sink_ptr> logSinksThread;

			logSinksThread.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinksThread.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexThread.log", true));
			logSinksThread[0]->set_pattern("%^[%T] %n: %v%$");
			logSinksThread[1]->set_pattern("[%T] [%l] %n: %v");

			s_ThreadLogger = std::make_shared<spdlog::logger>("[RYNEX][THREAD]", begin(logSinksThread), end(logSinksThread));

			spdlog::register_logger(s_ThreadLogger);
			s_ThreadLogger->set_level(spdlog::level::trace);
			s_ThreadLogger->flush_on(spdlog::level::trace);

			
		}
#endif
	}
#endif

	void Log::EnableLockNumberLoging()
	{
#if RY_CONSOLE_LOG_NUMBER

		s_NummberFormating--;
		if (s_NummberFormating == 0)
		{
			std::locale::global(std::locale("L"));
		}
#endif
	}

	void Log::DisableLockNumberLoging()
	{
#if RY_CONSOLE_LOG_NUMBER

		std::locale::global(std::locale());
		s_NummberFormating++;
#endif

	}
}
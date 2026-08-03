#include "rypch.h"
#include "Rynex/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Rynex {

	Log Log::s_LogInstance;


	Log::Log()
	{


		m_NummberFormating = 0u;
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
				logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexGraficCreate.log", true));
				logSinksGrafics[0]->set_pattern("[%T] [%l] %n: %v");

				m_CoreGraficsCreateLogger = std::make_shared<spdlog::logger>("CREATE", begin(logSinksGrafics), end(logSinksGrafics));

				spdlog::register_logger(m_CoreGraficsCreateLogger);
				m_CoreGraficsCreateLogger->set_level(spdlog::level::trace);
				m_CoreGraficsCreateLogger->flush_on(spdlog::level::trace);

			}

			{
				std::vector<spdlog::sink_ptr> logSinksGrafics;
				logSinksGrafics.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RynexGraficDelete.log", true));
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
	}

	Log::~Log()
	{
	}

	Log& Log::Get()
	{
		return s_LogInstance;
	}


	void Log::EnableLockNumberLoging()
	{
	}

	void Log::DisableLockNumberLoging()
	{
	}
}
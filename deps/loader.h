#pragma once
#include <windows.h>
#include <sdkddkver.h>

#include <sstream>
#include <vector>

#ifdef DINPUT8MHW_EXPORTS
#define DllExport   __declspec( dllexport )
#else
#define DllExport   __declspec( dllimport )
#endif

#pragma warning( disable: 4251 )

namespace loader {
	auto engine_logger = spdlog::basic_logger_mt("file_logger", "logs/LuaEngine.log");

	enum LogLevel {
		DEBUG = 0,
		INFO = 1,
		WARN = 2,
		ERR = 3
	};

	DllExport extern const char* GameVersion;
	DllExport extern LogLevel MinLogLevel;

	class DllExport LOG
	{
	private:
		std::stringstream stream;
		LogLevel logLevel;
		LOG(const LOG& o) = delete;
		LOG& operator=(const LOG& o) = delete;
	public:
		LOG(LogLevel level) :logLevel(level) {}
		~LOG();

		template<class T>
		LOG& operator<<(const T& x) {
			switch (logLevel)
			{
			case loader::DEBUG:
				engine_logger->debug("引擎信息: {}", x);
				break;
			case loader::INFO:
				engine_logger->info("引擎信息: {}", x);
				break;
			case loader::WARN:
				engine_logger->warn("引擎信息: {}", x);
				break;
			case loader::ERR:
				engine_logger->error("引擎信息: {}", x);
				break;
			default:
				engine_logger->info("引擎信息: {}", x);
				break;
			}
			
			if (logLevel >= MinLogLevel)
				stream << x;
			return *this;
		}
	};
}

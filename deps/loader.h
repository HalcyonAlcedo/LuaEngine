#pragma once
#include <windows.h>
#include <sdkddkver.h>

#include <sstream>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

#ifdef MHWLOADER_EXPORTS
#define DllExport   __declspec( dllexport )
#else
#define DllExport   __declspec( dllimport )
#endif

#pragma warning( disable: 4251 )

namespace loader {
	auto engine_logger = spdlog::basic_logger_mt("ÒýÇæ", "logs/LuaEngine.log");
	auto framework_logger = spdlog::basic_logger_mt("¿ò¼Ü", "logs/LuaEngine.log");
	auto lua_logger = spdlog::basic_logger_mt("Lua", "logs/LuaEngine.log");
	auto error_logger = spdlog::basic_logger_mt("´íÎó", "logs/Error.log");
	auto memory_logger = spdlog::rotating_logger_mt("ÄÚ´æ", "logs/Memory.log", 1048576, 2);

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
				lua_logger->debug(x);
				break;
			case loader::INFO:
				lua_logger->info(x);
				break;
			case loader::WARN:
				lua_logger->warn(x);
				break;
			case loader::ERR:
				lua_logger->error(x);
				break;
			default:
				lua_logger->info(x);
				break;
			}
			
			if (logLevel >= MinLogLevel)
				stream << x;
			return *this;
		}
	};
}

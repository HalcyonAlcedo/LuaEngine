#pragma once

/// <summary>
/// 为 FFI 调用提供的兼容层
/// </summary>
extern "C" {
	typedef int (*LuaCore_mapStatesCallback) (lua_State* L);

	/// <summary>
	/// 为所有 lua_State 注册一个全局函数
	/// </summary>
	DllExport void LuaCore_regGlobalFunc(const char* name, lua_CFunction func) {
		for (auto& s : LuaCore::LuaScript) {
			LuaCore::LuaScriptData& data = s.second;
			if (data.L != nullptr) {
				lua_register(data.L, name, func);
			}
		}
	}

	/// <summary>
	/// 为所有 lua_State 应用一个自定义回调函数
	/// </summary>
	DllExport void LuaCore_mapStates(LuaCore_mapStatesCallback callback) {
		for (auto& s : LuaCore::LuaScript) {
			LuaCore::LuaScriptData& data = s.second;
			if (data.L != nullptr) {
				callback(data.L);
			}
		}
	}

	/// <summary>
	/// <para>输出日志</para>
	/// 
	/// 日志等级：
	/// <para>0 -> DEBUG</para>
	/// <para>1 -> INFO</para>
	/// <para>2 -> WARN</para>
	/// <para>3 -> ERROR</para>
	/// </summary>
	/// <param name="level">
	/// 日志等级
	/// </param>
	/// <param name="content">日志内容</param>
	DllExport void Log(int level, const char* content) {
		switch (level)
		{
		case 0:
			engine_logger->debug(content);
			break;
		case 1:
			engine_logger->info(content);
			break;
		case 2:
			engine_logger->warn(content);
			break;
		case 3:
			engine_logger->error(content);
			break;
		default:
			engine_logger->error(content);
			break;
		}
	}
}

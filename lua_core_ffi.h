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
}

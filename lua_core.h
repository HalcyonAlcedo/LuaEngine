#pragma once
#include "lua_register.h"

using namespace loader;

lua_State* L = nullptr;

namespace LuaCore {
	//错误回调
	static int LuaErrorCallBack(lua_State* L)
	{
		lua_Debug debug = {};
		//错误所处的的调用层级
		int rank = 0;
		//逐层获取lua抛出的错误，直到获取到
		while (lua_getstack(L, rank, &debug)) {
			rank++;
		}
		//判断栈顶是否为字符串类型
		int type = lua_type(L, -1);
		if (type != 4)
			return 0;
		std::string error = lua_tostring(L, -1);
		std::string errorShort = debug.short_src;
		int errorLine = debug.currentline;
		std::string errorMsg = "error:" + error + ",errorShort:" + errorShort
			+ ",line:" + std::to_string(errorLine);
		//将错误信息压人栈
		lua_pushstring(L, errorMsg.c_str());
		return 1;
	}
	int LuaErrorRecord(string error) {
		LOG(ERR) << error;
		return 1;
	}

	static int Lua_Load(string LuaFile)
	{
		int err = 0;
		if (LuaHandle::LuaCode[LuaFile].hotReload) {
			err = luaL_dofile(L, LuaHandle::LuaCode[LuaFile].file.c_str());
		}
		else {
			err = luaL_dostring(L, LuaHandle::LuaCode[LuaFile].code.c_str());
		}

		if (err != 0)
		{
			int type = lua_type(L, -1);
			if (type == 4) {
				string error = lua_tostring(L, -1);
				LuaErrorRecord(error);
			}
			return -1;
		}
		//设置错误回调函数
		lua_pushcfunction(L, LuaErrorCallBack);
		LOG(INFO) << LuaFile;
		return 1;
	}
}


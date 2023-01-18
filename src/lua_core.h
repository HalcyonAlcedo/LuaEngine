#pragma once
#include "lua_register.h"

using namespace loader;

namespace LuaCore {
	//绘制开关
	bool luaframe = false;
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
		LOG(ERR) << errorMsg;
		//将错误信息压人栈
		lua_pushstring(L, errorMsg.c_str());
		return 1;
	}
	static int LuaErrorRecord(string error) {
		if(error != "error in error handling")
		LOG(ERR) << error;
		return 1;
	}
	//加载Lua游戏引擎
	static void loadEngine(lua_State* L) {
		engine_logger->info("初始化Engine.lua数据引擎");
		int err = 0;
		err = luaL_dofile(L, "Lua\\Engine.lua");
		if (err != 0)
		{
			int type = lua_type(L, -1);
			if (type == 4) {
				string error = lua_tostring(L, -1);
				LuaErrorRecord(error);
			}
		}
	}
	//加载Lua脚本
	static void Lua_Load(string path, vector<string>& files)
	{
		engine_logger->info("开始进行Lua脚本加载");
		using namespace std::filesystem;
		if (exists(path) && is_directory(path))
		{
			for (auto& fe : directory_iterator(path))
			{
				auto fp = fe.path();
				auto temp = fp.filename();
				if (fp.extension().string() == ".lua" && temp.stem().string() != "Engine") {
					LuaHandle::LuaScript[temp.stem().string()] = LuaHandle::LuaScriptData(
						luaL_newstate(),
						temp.stem().string(),
						fp.string());
					files.push_back(temp.stem().string());
				}
			}
		}
	}
	//运行lua脚本
	static int Lua_Run(lua_State* L, string LuaFile)
	{
		int err = 0;
		//加载引擎
		loadEngine(L);
		err = luaL_dofile(L, LuaHandle::LuaScript[LuaFile].file.c_str());
		if (err != 0)
		{
			int type = lua_type(L, -1);
			if (type == 4) {
				string error = lua_tostring(L, -1);
				LuaErrorRecord(error);
			}
			engine_logger->warn("Lua脚本{}文件加载出现错误", LuaFile);
			return -1;
		}
		//设置错误回调函数
		lua_pushcfunction(L, LuaErrorCallBack);
		engine_logger->info("加载{}文件至Lua引擎", LuaFile);
		return 1;
	}
	//运行lua代码
	static void run(string func) {
		for (string file_name : LuaHandle::LuaFiles) {
			if (LuaHandle::LuaScript[file_name].start) {
				lua_State* L = LuaHandle::LuaScript[file_name].L;
				int err = 0;
				int callBack = lua_gettop(L);
				lua_getglobal(L, func.c_str());
				if (lua_type(L, -1) == LUA_TFUNCTION)
				{
					err = lua_pcall(L, 0, 0, callBack);
					if (err != 0)
					{
						string error = lua_tostring(L, -1);
						LuaCore::LuaErrorRecord(error);
					}
				}
			}
		}
	}
	//为lua注册新函数
	static void Lua_register(string funcName, int(*func)(lua_State* pL)) {
		for (string file_name : LuaHandle::LuaFiles) {
			if (LuaHandle::LuaScript[file_name].start) {
				lua_State* L = LuaHandle::LuaScript[file_name].L;
				lua_register(L, funcName.c_str(), func);
			}
		}
	}
}


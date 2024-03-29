#include <fstream>
#include <queue>
#include <functional>

#include <random>
#include <iostream>
#include <cmath>

#include <windows.h>
#include <Shlwapi.h>

#include "minhook/include/MinHook.h"
#include "json/json.hpp"
#include "sol/sol.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>

#include "game_utils.h"
#include "lua_core.h"
#include "lua_core_ffi.h"
#include "lua_register.h"
#include "lua_thread.h"

#include "hook_monster.h"
#include "hook_animals.h"
#include "hook_camera.h"
#include "hook_hit.h"
#include "hook_shlp.h"
#include "hook_frame.h"
#include <future>

using namespace loader;

#pragma region Basic data processing program
static void MassageCommand() {
	void* MassagePlot = *(undefined**)MH::World::Message;
	if (MassagePlot != nullptr) {
		string Massage = offsetPtr<char>(MassagePlot, 0xC0);
		string::size_type idx;
		//执行实时命令
		idx = Massage.find("luac:");
		if (idx != string::npos) {
			string command = Massage.substr(Massage.find("luac:") + 5);
			framework_logger->info("执行游戏内发出的Lua实时指令: {}", command);
			int err = 0;
			err = luaL_dostring(LuaCore::Lc, command.c_str());
			if (err != 0)
			{
				int type = lua_type(LuaCore::Lc, -1);
				if (type == 4) {
					string error = lua_tostring(LuaCore::Lc, -1);
					LuaCore::LuaErrorRecord(error);
				}
			}
			*offsetPtr<char>(MassagePlot, 0xC0) = *"";
		}
		//重载虚拟机
		idx = Massage.find("reload ");
		if (idx != string::npos) {
			string luae = Massage.substr(Massage.find("reload ") + 7);
			std::map<string, LuaCore::LuaScriptData>::iterator it;
			it = LuaCore::LuaScript.find(luae);
			if (it != LuaCore::LuaScript.end()) {
				framework_logger->info("重载脚本{}的Lua虚拟机", luae);
				lua_close(LuaCore::LuaScript[luae].L);
				LuaCore::LuaScript[luae].L = luaL_newstate();
				luaL_openlibs(LuaCore::LuaScript[luae].L);
				registerFunc(LuaCore::LuaScript[luae].L);
				lua_thread::Registe(LuaCore::LuaScript[luae].L);
				hook_monster::Registe(LuaCore::LuaScript[luae].L);
				hook_animals::Registe(LuaCore::LuaScript[luae].L);
				hook_camera::Registe(LuaCore::LuaScript[luae].L);
				hook_hit::Registe(LuaCore::LuaScript[luae].L);
				hook_frame::Registe(LuaCore::LuaScript[luae].L);
				hook_shlp::Registe(LuaCore::LuaScript[luae].L);
				LuaCore::reloadTime = time(nullptr);;
				if (LuaCore::Lua_Run(LuaCore::LuaScript[luae].L, luae) != 1) {
					engine_logger->warn("脚本{}重载后出现异常，已停止该脚本继续运行", luae);
					LuaCore::LuaScript[luae].start = false;
				}
				else {
					engine_logger->info("脚本{}已完成重载操作，代码运行正常", luae);
					string message = "脚本" + luae + "已完成重载操作";
					MH::Chat::ShowGameMessage(*(undefined**)MH::Chat::MainPtr, (undefined*)&utils::string_To_UTF8(message)[0], -1, -1, 0);
					LuaCore::LuaScript[luae].start = true;
					LuaCore::run("on_init", LuaCore::LuaScript[luae].L);
				}
			}
			*offsetPtr<char>(MassagePlot, 0xC0) = *"";
		}
	}
}
//数据更新程序
static void updata() {
	//地图更新时清理数据
	void* TimePlot = utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50, 0x7D20 });
	if (TimePlot != nullptr && Chronoscope::NowTime > *offsetPtr<float>(TimePlot, 0xC24)) {
		framework_logger->info("游戏内发生场景变换，更新框架缓存数据");
		//清除计时器数据
		Chronoscope::ChronoscopeList.clear();
		//清除按键数据
		Keyboard::TempData::t_KeyCount.clear();
		Keyboard::TempData::t_KeyDown.clear();
		//清除Xbox手柄数据
		XboxPad::TempData::t_KeyCount.clear();
		XboxPad::TempData::t_KeyDown.clear();
		//钩子数据
		hook_frame::SpeedList.clear();
		engine_logger->info("运行脚本on_switch_scenes场景切换代码");
		LuaCore::run("on_switch_scenes");
	}
	//更新计时器
	Chronoscope::chronoscope();
	//更新手柄数据
	XboxPad::Updata();
	//执行玩家消息指令
	MassageCommand();
}
#pragma endregion

static void InitLogger() {
	engine_logger->set_level(spdlog::level::info);
	engine_logger->flush_on(spdlog::level::trace);
	framework_logger->set_level(spdlog::level::info);
	framework_logger->flush_on(spdlog::level::trace);
	lua_logger->set_level(spdlog::level::info);
	lua_logger->flush_on(spdlog::level::trace);
	error_logger->set_level(spdlog::level::info);
	error_logger->flush_on(spdlog::level::trace);
	memory_logger->set_level(spdlog::level::info);
	memory_logger->flush_on(spdlog::level::trace);
}

void LoadLuaFiles() {
	framework_logger->info("加载Lua文件");
	LuaCore::Lua_Load("Lua\\", LuaCore::LuaFiles);
	framework_logger->info("创建并运行实时脚本环境");
	LuaCore::Lc = luaL_newstate();
}

void ApplyDelayedHooks() {
	for (string file_name : LuaCore::LuaFiles) {
		if (LuaCore::LuaScript[file_name].start && LuaCore::LuaScript[file_name].L != nullptr) {
			lua_State* L = LuaCore::LuaScript[file_name].L;
			lua_thread::Registe(L);
			hook_monster::Registe(L);
			hook_animals::Registe(L);
			hook_camera::Registe(L);
			hook_hit::Registe(L);
			hook_frame::Registe(L);
			hook_shlp::Registe(L);
		}
	}
	framework_logger->info("开始执行创建各功能钩子代码");
	hook_monster::Hook();
	hook_animals::Hook();
	hook_camera::Hook();
	hook_hit::Hook();
	hook_shlp::Hook();
	hook_frame::Hook();
}

void ApplyTimeHooks() {
	framework_logger->info("创建on_time钩子");
	MH_Initialize();
	HookLambda(MH::World::MapClockLocal,
		[](auto clock, auto clock2) {
			auto ret = original(clock, clock2);
			//更新基础数据
			updata();
			//运行lua虚拟机
			LuaCore::run("on_time");
			return ret;
		});
	MH_ApplyQueued();
}

void RegFuncs() {
	luaL_openlibs(LuaCore::Lc);
	registerFunc(LuaCore::Lc);

	for (string file_name : LuaCore::LuaFiles) {
		if (LuaCore::LuaScript[file_name].start && LuaCore::LuaScript[file_name].L != nullptr) {
			lua_State* L = LuaCore::LuaScript[file_name].L;
			luaL_openlibs(L);
			engine_logger->info("为 {} 脚本注册引擎初始化函数", file_name);
			registerFunc(L);
		}
	}
}

void RunLuaFiles() {
	for (string file_name : LuaCore::LuaFiles) {
		lua_State* L = LuaCore::LuaScript[file_name].L;
		if (LuaCore::Lua_Run(L, file_name) != 1) {
			engine_logger->warn("脚本{}运行过程中出现异常，已停止该脚本继续运行", file_name);
			LuaCore::LuaScript[file_name].start = false;
		}
	}
}

void RunOnInit() {
	engine_logger->info("运行脚本on_init初始化代码");
	LuaCore::run("on_init");
}

__declspec(dllexport) extern bool Load()
{
	LoadLuaFiles();
	ApplyDelayedHooks();
	RegFuncs();
	RunLuaFiles();
	RunOnInit();
	ApplyTimeHooks();

	return true;
}

#define MAX_LINE 1024
#define MAX_SECTION 50
#define MAX_KEY 50
#define MAX_VALUE 50
// 从一行字符串中解析出节、键和值，返回类型：0-空行或注释，1-节，2-键值对，-1-错误
int parse_line(char* line, char* section, char* key, char* value)
{
	char* start, * end;
	line[strlen(line) - 1] = '\0'; // 去掉换行符
	start = line; // 起始位置
	while (*start == ' ') start++; // 跳过空格
	if (*start == '\0' || *start == ';') return 0; // 空行或注释
	if (*start == '[') // 节
	{
		start++;
		end = strchr(start, ']');
		if (end == NULL) return -1; // 没有找到右括号
		strncpy_s(section, MAX_SECTION, start, end - start); // 复制节名
		section[end - start] = '\0';
		return 1;
	}
	else // 键值对
	{
		end = strchr(start, '=');
		if (end == NULL) return -1; // 没有找到等号
		strncpy_s(key, MAX_KEY, start, end - start); // 复制键名
		key[end - start] = '\0';
		start = end + 1;
		strcpy_s(value, MAX_VALUE, start); // 复制值
		return 2;
	}
}

// 从ini文件中获取指定节和键的值，如果成功返回1，否则返回0
int get_value_from_ini(const char* file_name, const char* section, const char* key, char* value)
{
	FILE* fp;
	char line[MAX_LINE];
	char cur_section[MAX_SECTION] = "";
	int found = 0; // 是否找到指定节和键
	fopen_s(&fp, file_name, "r"); // 打开文件
	if (fp == NULL) // 文件不存在
	{
		return 0;
	}
	while (fgets(line, MAX_LINE, fp) != NULL) // 逐行读取
	{
		char tmp_section[MAX_SECTION], tmp_key[MAX_KEY], tmp_value[MAX_VALUE];
		int type = parse_line(line, tmp_section, tmp_key, tmp_value); // 解析行
		if (type == 1) // 节
		{
			strcpy_s(cur_section, MAX_SECTION, tmp_section); // 更新当前节
		}
		else if (type == 2) // 键值对
		{
			if (strcmp(cur_section, section) == 0 && strcmp(tmp_key, key) == 0) // 找到指定节和键
			{
				strcpy_s(value, MAX_VALUE, tmp_value); // 复制值
				found = 1; // 标记已找到
				break; // 退出循环
			}
		}
	}
	fclose(fp); // 关闭文件
	return found;
}

void MemoryLogThread() {
	char MemoryLog[MAX_VALUE];
	if (get_value_from_ini("graphics_option.ini", "LuaEngine", "MemoryLog", MemoryLog)) {
		if (string(MemoryLog) == "On") {
			framework_logger->info("显示内存日志");
			LOG(WARN) << "注意：内存日志已经开启，会严重影响系统性能，如果不是在进行调试，请删除graphics_option.ini中的MemoryLog=On";
			LuaCore::MemoryLog = true;
		}
	}
}

/// <summary>
/// 添加 Dll 扫描目录
/// </summary>
/// <returns></returns>
BOOL AddDllSearchPath()
{
	TCHAR currentPath[MAX_PATH];
	TCHAR luaLibPath[MAX_PATH];

	// 获取进程文件路径
	DWORD dwResult = GetModuleFileName(NULL, currentPath, MAX_PATH);
	if (dwResult == 0 || dwResult == MAX_PATH)
	{
		return FALSE;
	}
	// 去掉文件名部分
	PathRemoveFileSpec(currentPath);
	// 添加子目录
	PathCombine(luaLibPath, currentPath, TEXT("Lua\\lib"));
	// 添加DLL搜索路径
	if (!SetDllDirectory(luaLibPath))
	{
		return FALSE;
	}
	return TRUE;
}

void mainThread() {
	// 先延迟2秒加载，以便其他组件注册完毕
	// TODO: 更高效的注册方法
	LoadLuaFiles();
	ApplyDelayedHooks();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	RegFuncs();
	RunLuaFiles();
	RunOnInit();
	ApplyTimeHooks();
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(hModule);
		if (!AddDllSearchPath()) { LOG(ERR) << "LuaEngine 错误：添加动态库扫描目录失败，已忽略。"; };
		InitLogger();
		std::thread tmlog(MemoryLogThread);
		tmlog.detach();
		// TODO: 在新线程中处理组件注册消息，以及完成模块初始化
		std::thread tmain(mainThread);
		tmain.detach();
		break;
	}
	}
	return TRUE;
}

#pragma once
#include <windows.h>
#include <sdkddkver.h>

#include <sstream>
#include <vector>

#include <map>

#define DllExport   __declspec( dllimport )

namespace LuaCore {
    struct LuaScriptData {
        lua_State* L;
        std::string name;
        std::string file;
        bool start;
        LuaScriptData(
            lua_State* L = nullptr,
            std::string name = "",
            std::string file = "",
            bool start = true
        ) :L(L), name(name), file(file), start(start) { };
    };
    bool initWeapon = false;
    DllExport extern time_t reloadTime;
    time_t reload;
    DllExport extern void run(std::string func, lua_State* runL = nullptr);
    DllExport extern std::vector<std::string> getLuaFiles();
    DllExport extern std::map<std::string, LuaScriptData> getLuas();
}

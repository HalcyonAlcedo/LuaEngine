#include <fstream>
#include <queue>
#include <functional>

#include <windows.h>

#include "minhook/include/MinHook.h"
#include "sol/sol.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>
using namespace loader;

extern "C" void* _stdcall GetRBXPtr(void*);

void* CameraPtr = nullptr;
bool SetVisual = false;
float Visualx = 0;
float Visualy = 0;
float Visualz = 0;
float Coordinatex = 0;
float Coordinatey = 0;
float Coordinatez = 0;

extern "C" int __declspec(dllexport) luaopen_camera(lua_State * L)
{
    MH_Initialize();
    HookLambda(MH::Player::Visual,
        []() {
            GetRBXPtr(&CameraPtr);
            if (CameraPtr != nullptr) {
                Visualx = *offsetPtr<float>(CameraPtr, 0x0);
                Visualy = *offsetPtr<float>(CameraPtr, 0x4);
                Visualz = *offsetPtr<float>(CameraPtr, 0x8);
                if (SetVisual) {
                    *offsetPtr<float>(CameraPtr, 0x0) = Coordinatex;
                    *offsetPtr<float>(CameraPtr, 0x4) = Coordinatey;
                    *offsetPtr<float>(CameraPtr, 0x8) = Coordinatez;
                }
            }
            return original();
        });
    MH_ApplyQueued();
    lua_register(L, "GetCameraData", [](lua_State* pL) -> int
        {
            lua_pushnumber(pL, Visualx);
            lua_pushnumber(pL, Visualy);
            lua_pushnumber(pL, Visualz);
            lua_pushboolean(pL, SetVisual);
            return 4;
        });
    lua_register(L, "SetCameraData", [](lua_State* pL) -> int
        {
            SetVisual = (bool)lua_toboolean(pL, 1);
            if (SetVisual) {
                Coordinatex = (float)lua_tonumber(pL, 2);
                Coordinatey = (float)lua_tonumber(pL, 3);
                Coordinatez = (float)lua_tonumber(pL, 4);
            }
            return 0;
        });
    return 1;
}


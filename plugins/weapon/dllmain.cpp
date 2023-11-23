#include <fstream>
#include <queue>
#include <functional>

#include <windows.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "minhook/include/MinHook.h"
#include "sol/sol.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>
using namespace loader;

extern "C" void* _stdcall GetRBXPtr(void*);

struct Vector3 {
	float x, y, z;
	Vector3(float x = 0, float y = 0, float z = 0) :x(x), y(y), z(z) { };
};
namespace WeaponOrnaments {
	//缓存数据
	namespace TempData {
		void* t_ornaments = nullptr;
		bool t_setOrnamentsCoordinate = false;
		Vector3 t_SetOrnamentsCoordinate;
		bool t_setOrnamentsSize = false;
		Vector3 t_SetOrnamentsSize;
	}
	//装饰物坐标
	Vector3 OrnamentsCoordinate = Vector3();
	//装饰物模型大小
	Vector3 OrnamentsSize = Vector3();
	//解除装饰物坐标控制
	static void DecontrolOrnamentsCoordinate() {
		TempData::t_setOrnamentsCoordinate = false;
	}
	//解除装饰物大小控制
	static void DecontrolOrnamentsSize() {
		TempData::t_setOrnamentsSize = false;
	}
	//装饰物坐标设置(X坐标,Y坐标,Z坐标)
	static void SetOrnamentsCoordinate(float X, float Y, float Z) {
		TempData::t_SetOrnamentsCoordinate = Vector3(X, Y, Z);
		TempData::t_setOrnamentsCoordinate = true;
	}
	//装饰物大小设置(X坐标,Y坐标,Z坐标)
	static void SetOrnamentsSize(float X, float Y, float Z) {
		TempData::t_SetOrnamentsSize = Vector3(X, Y, Z);
		TempData::t_setOrnamentsSize = true;
	}
}
namespace Weapons {
	//缓存数据
	namespace TempData {
		void* t_mainWeapon = nullptr;
		void* t_secondaryWeapon = nullptr;
		void* t_weaponHit = nullptr;
		bool t_setMainWeaponCoordinate = false;
		Vector3 t_SetMainWeaponCoordinate;
		bool t_setMainWeaponSize = false;
		Vector3 t_SetMainWeaponSize;
		bool t_setSecondaryWeaponCoordinate = false;
		Vector3 t_SetSecondaryWeaponCoordinate;
		bool t_setSecondaryWeaponSize = false;
		Vector3 t_SetSecondaryWeaponSize;
	}
	//主武器坐标
	Vector3 MainWeaponCoordinate = Vector3();
	//主武器模型大小
	Vector3 MainWeaponSize = Vector3();
	//副武器坐标
	Vector3 SecondaryWeaponCoordinate = Vector3();
	//副武器模型大小
	Vector3 SecondaryWeaponSize = Vector3();
	//武器命中坐标
	Vector3 HitCoordinate = Vector3();
	//解除主武器坐标控制
	static void DecontrolMainWeaponCoordinate() {
		TempData::t_setMainWeaponCoordinate = false;
	}
	//解除主武器大小控制
	static void DecontrolMainWeaponSize() {
		TempData::t_setMainWeaponSize = false;
	}
	//主武器坐标设置(X坐标,Y坐标,Z坐标)
	static void SetMainWeaponCoordinate(float X, float Y, float Z) {
		TempData::t_SetMainWeaponCoordinate = Vector3(X, Y, Z);
		TempData::t_setMainWeaponCoordinate = true;
	}
	//主武器大小设置(X坐标,Y坐标,Z坐标)
	static void SetMainWeaponSize(float X, float Y, float Z) {
		TempData::t_SetMainWeaponSize = Vector3(X, Y, Z);
		TempData::t_setMainWeaponSize = true;
	}

	//解除副武器坐标控制
	static void DecontrolSecondaryWeaponCoordinate() {
		TempData::t_setSecondaryWeaponCoordinate = false;
	}
	//解除副武器大小控制
	static void DecontrolSecondaryWeaponSize() {
		TempData::t_setSecondaryWeaponSize = false;
	}
	//副武器坐标设置(X坐标,Y坐标,Z坐标)
	static void SetSecondaryWeaponCoordinate(float X, float Y, float Z) {
		TempData::t_SetSecondaryWeaponCoordinate = Vector3(X, Y, Z);
		TempData::t_setSecondaryWeaponCoordinate = true;
	}
	//副武器大小设置(X坐标,Y坐标,Z坐标)
	static void SetSecondaryWeaponSize(float X, float Y, float Z) {
		TempData::t_SetSecondaryWeaponSize = Vector3(X, Y, Z);
		TempData::t_setSecondaryWeaponSize = true;
	}
}

extern "C" int __declspec(dllexport) luaopen_Weapon(lua_State * L)
{
    MH_Initialize();
	HookLambda(MH::Weapon::WeaponOrnaments,
		[]() {
			GetRBXPtr(&WeaponOrnaments::TempData::t_ornaments);
			if (WeaponOrnaments::TempData::t_ornaments != nullptr) {
				WeaponOrnaments::OrnamentsCoordinate.x = *offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x160);
				WeaponOrnaments::OrnamentsCoordinate.y = *offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x164);
				WeaponOrnaments::OrnamentsCoordinate.z = *offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x168);
				WeaponOrnaments::OrnamentsSize.x = *offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x180);
				WeaponOrnaments::OrnamentsSize.y = *offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x184);
				WeaponOrnaments::OrnamentsSize.z = *offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x188);
				if (WeaponOrnaments::TempData::t_setOrnamentsCoordinate) {
					*offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x160) = WeaponOrnaments::TempData::t_SetOrnamentsCoordinate.x;
					*offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x164) = WeaponOrnaments::TempData::t_SetOrnamentsCoordinate.y;
					*offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x168) = WeaponOrnaments::TempData::t_SetOrnamentsCoordinate.z;
				}
				if (WeaponOrnaments::TempData::t_setOrnamentsSize) {
					*offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x180) = WeaponOrnaments::TempData::t_SetOrnamentsSize.x;
					*offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x184) = WeaponOrnaments::TempData::t_SetOrnamentsSize.y;
					*offsetPtr<float>(WeaponOrnaments::TempData::t_ornaments, 0x188) = WeaponOrnaments::TempData::t_SetOrnamentsSize.z;
				}
			}
			return original();
		});
	HookLambda(MH::Weapon::MainWeaponPtr,
		[]() {
			GetRBXPtr(&Weapons::TempData::t_mainWeapon);
			if (Weapons::TempData::t_mainWeapon != nullptr) {
				Weapons::MainWeaponCoordinate = Vector3(
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x160),
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x164),
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x168)
				);
				Weapons::MainWeaponSize = Vector3(
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x180),
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x184),
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x188)
				);
				if (Weapons::TempData::t_setMainWeaponCoordinate) {
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x160) = Weapons::TempData::t_SetMainWeaponCoordinate.x;
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x164) = Weapons::TempData::t_SetMainWeaponCoordinate.y;
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x168) = Weapons::TempData::t_SetMainWeaponCoordinate.z;
				}
				if (Weapons::TempData::t_setMainWeaponSize) {
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x180) = Weapons::TempData::t_SetMainWeaponSize.x;
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x184) = Weapons::TempData::t_SetMainWeaponSize.y;
					*offsetPtr<float>(Weapons::TempData::t_mainWeapon, 0x188) = Weapons::TempData::t_SetMainWeaponSize.z;
				}
			}
			return original();
		});
	HookLambda(MH::Weapon::SecondaryWeaponPtr,
		[]() {
			GetRBXPtr(&Weapons::TempData::t_secondaryWeapon);
			if (Weapons::TempData::t_secondaryWeapon != nullptr) {
				Weapons::SecondaryWeaponCoordinate = Vector3(
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x160),
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x164),
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x168)
				);
				Weapons::SecondaryWeaponSize = Vector3(
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x180),
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x184),
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x188)
				);
				if (Weapons::TempData::t_setSecondaryWeaponCoordinate) {
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x160) = Weapons::TempData::t_SetSecondaryWeaponCoordinate.x;
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x164) = Weapons::TempData::t_SetSecondaryWeaponCoordinate.y;
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x168) = Weapons::TempData::t_SetSecondaryWeaponCoordinate.z;
				}
				if (Weapons::TempData::t_setSecondaryWeaponSize) {
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x180) = Weapons::TempData::t_SetSecondaryWeaponSize.x;
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x184) = Weapons::TempData::t_SetSecondaryWeaponSize.y;
					*offsetPtr<float>(Weapons::TempData::t_secondaryWeapon, 0x188) = Weapons::TempData::t_SetSecondaryWeaponSize.z;
				}
			}
			return original();
		});
    MH_ApplyQueued();

	//获取玩家武器装饰物坐标
    lua_register(L, "GetOrnamentsCoordinate", [](lua_State* pL) -> int
        {
			lua_pushnumber(pL, WeaponOrnaments::OrnamentsCoordinate.x);
			lua_pushnumber(pL, WeaponOrnaments::OrnamentsCoordinate.y);
			lua_pushnumber(pL, WeaponOrnaments::OrnamentsCoordinate.z);
			return 3;
        });
	//获取玩家武器装饰物模型大小
    lua_register(L, "GetOrnamentsSize", [](lua_State* pL) -> int
        {
			lua_pushnumber(pL, WeaponOrnaments::OrnamentsSize.x);
			lua_pushnumber(pL, WeaponOrnaments::OrnamentsSize.y);
			lua_pushnumber(pL, WeaponOrnaments::OrnamentsSize.z);
			return 3;
        });
	//设置玩家武器装饰物坐标
	lua_register(L, "SetOrnamentsCoordinate", [](lua_State* pL) -> int
		{
			float x = (float)lua_tonumber(pL, 1);
			float y = (float)lua_tonumber(pL, 2);
			float z = (float)lua_tonumber(pL, 3);
			WeaponOrnaments::SetOrnamentsCoordinate(x, y, z);
			return 0;
		});
	//设置玩家武器装饰物模型大小
	lua_register(L, "SetOrnamentsSize", [](lua_State* pL) -> int
		{
			float x = (float)lua_tonumber(pL, 1);
			float y = (float)lua_tonumber(pL, 2);
			float z = (float)lua_tonumber(pL, 3);
			WeaponOrnaments::SetOrnamentsSize(x, y, z);
			return 0;
		});
	//解除玩家武器装饰物坐标设置
	lua_register(L, "DecontrolOrnamentsCoordinate", [](lua_State* pL) -> int
		{
			WeaponOrnaments::DecontrolOrnamentsCoordinate();
			return 0;
		});
	//解除玩家武器装饰物模型大小设置
	lua_register(L, "DecontrolOrnamentsSize", [](lua_State* pL) -> int
		{
			WeaponOrnaments::DecontrolOrnamentsSize();
			return 0;
		});
	//获取玩家主武器坐标
	lua_register(L, "GetMainWeaponCoordinate", [](lua_State* pL) -> int
		{
			lua_pushnumber(pL, Weapons::MainWeaponCoordinate.x);
			lua_pushnumber(pL, Weapons::MainWeaponCoordinate.y);
			lua_pushnumber(pL, Weapons::MainWeaponCoordinate.z);
			return 3;
		});
	//获取玩家主武器模型大小
	lua_register(L, "GetMainWeaponSize", [](lua_State* pL) -> int
		{
			lua_pushnumber(pL, Weapons::MainWeaponSize.x);
			lua_pushnumber(pL, Weapons::MainWeaponSize.y);
			lua_pushnumber(pL, Weapons::MainWeaponSize.z);
			return 3;
		});
	//设置玩家主武器坐标
	lua_register(L, "SetMainWeaponCoordinate", [](lua_State* pL) -> int
		{
			float x = (float)lua_tonumber(pL, 1);
			float y = (float)lua_tonumber(pL, 2);
			float z = (float)lua_tonumber(pL, 3);
			Weapons::SetMainWeaponCoordinate(x, y, z);
			return 0;
		});
	//设置玩家主武器模型大小
	lua_register(L, "SetMainWeaponSize", [](lua_State* pL) -> int
		{
			float x = (float)lua_tonumber(pL, 1);
			float y = (float)lua_tonumber(pL, 2);
			float z = (float)lua_tonumber(pL, 3);
			Weapons::SetMainWeaponSize(x, y, z);
			return 0;
		});
	//解除玩家主武器坐标设置
	lua_register(L, "DecontrolMainWeaponCoordinate", [](lua_State* pL) -> int
		{
			Weapons::DecontrolMainWeaponCoordinate();
			return 0;
		});
	//解除玩家主武器模型大小设置
	lua_register(L, "DecontrolMainWeaponSize", [](lua_State* pL) -> int
		{
			Weapons::DecontrolMainWeaponSize();
			return 0;
		});
	//获取玩家副武器坐标
	lua_register(L, "GetSecondaryWeaponCoordinate", [](lua_State* pL) -> int
		{
			lua_pushnumber(pL, Weapons::SecondaryWeaponCoordinate.x);
			lua_pushnumber(pL, Weapons::SecondaryWeaponCoordinate.y);
			lua_pushnumber(pL, Weapons::SecondaryWeaponCoordinate.z);
			return 3;
		});
	//获取玩家副武器模型大小
	lua_register(L, "GetSecondaryWeaponSize", [](lua_State* pL) -> int
		{
			lua_pushnumber(pL, Weapons::SecondaryWeaponSize.x);
			lua_pushnumber(pL, Weapons::SecondaryWeaponSize.y);
			lua_pushnumber(pL, Weapons::SecondaryWeaponSize.z);
			return 3;
		});
	//设置玩家副武器坐标
	lua_register(L, "SetSecondaryWeaponCoordinate", [](lua_State* pL) -> int
		{
			float x = (float)lua_tonumber(pL, 1);
			float y = (float)lua_tonumber(pL, 2);
			float z = (float)lua_tonumber(pL, 3);
			Weapons::SetSecondaryWeaponCoordinate(x, y, z);
			return 0;
		});
	//设置玩家副武器模型大小
	lua_register(L, "SetSecondaryWeaponSize", [](lua_State* pL) -> int
		{
			float x = (float)lua_tonumber(pL, 1);
			float y = (float)lua_tonumber(pL, 2);
			float z = (float)lua_tonumber(pL, 3);
			Weapons::SetSecondaryWeaponSize(x, y, z);
			return 0;
		});
	//解除玩家副武器坐标设置
	lua_register(L, "DecontrolSecondaryWeaponCoordinate", [](lua_State* pL) -> int
		{
			Weapons::DecontrolSecondaryWeaponCoordinate();
			return 0;
		});
	//解除玩家副武器模型大小设置
	lua_register(L, "DecontrolSecondaryWeaponSize", [](lua_State* pL) -> int
		{
			Weapons::DecontrolSecondaryWeaponSize();
			return 0;
		});

    return 1;
}


#pragma once

extern "C" void* _stdcall GetRBP160(void*);

#pragma region screen
namespace hook_screen {
	struct ScreenaData {
		float x;
		float y;
		ScreenaData(
			float x = 0,
			float y = 0)
			: x(x), y(y) {
		};
	};
	map<void*, ScreenaData> ScreenList;
	static void Hook() {
		framework_logger->info("创建屏幕操作钩子");
		MH_Initialize();
		HookLambda(MH::World::Screen,
			[](auto rcx) {
				/*
				void* Plot = nullptr;
				GetRBP160(&Plot);
				*/
				ScreenList[rcx] = ScreenaData(*offsetPtr<float>(rcx, 0xb0), *offsetPtr<float>(rcx, 0xb4));
				return original(rcx);
			});
		MH_ApplyQueued();
	}
	static void Registe(lua_State* L) {
		engine_logger->info("注册屏幕相关函数");
		lua_register(L, "GetScreenPos", [](lua_State* pL) -> int
			{
				/*
				void* screenPtr = (void*)(long long)lua_tointeger(pL, 1);
				lua_pushnumber(pL, ScreenList[screenPtr].x);
				lua_pushnumber(pL, ScreenList[screenPtr].y);
				return 2;
				*/
				lua_newtable(pL);//创建一个表格，放在栈顶
				for (auto [e, data] : ScreenList) {
					if (e != nullptr) {
						lua_pushinteger(pL, (long long)e);//压入地址
						lua_newtable(pL);//压入编号信息表
						lua_pushstring(pL, "x");//怪物Id
						lua_pushinteger(pL, data.x);
						lua_settable(pL, -3);
						lua_pushstring(pL, "y");//怪物SubId
						lua_pushinteger(pL, data.y);
						lua_settable(pL, -3);
						lua_settable(pL, -3);//弹出到顶层
					}
				}
				return 1;
			});
	}
}
#pragma endregion

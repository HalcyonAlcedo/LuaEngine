#pragma once

#pragma region frame
namespace hook_frame {
	map<void*, float> SpeedList;
	SafetyHookInline g_hook_ActionFrameSpeed;
	static void Hook() {
		framework_logger->info("创建动作帧速率调节钩子");

		g_hook_ActionFrameSpeed = safetyhook::create_inline(MH::World::ActionFrameSpeed, reinterpret_cast<void*>(
			+[](void* entity) {
				for (auto [ptr, addSpeed] : SpeedList) {
					if (entity == ptr) {
						float now_speed = *(float*)((long long)*(long long*)(0x1451238C8) + (*(int*)((long long)entity + 0x10) * 0xf8) + 0xa0);
						if (now_speed + addSpeed >= 0) {
							*(float*)((long long)*(long long*)(0x1451238C8) + (*(int*)((long long)entity + 0x10) * 0xf8) + 0xa0) = now_speed + addSpeed;
						}
						else {
							*(float*)((long long)*(long long*)(0x1451238C8) + (*(int*)((long long)entity + 0x10) * 0xf8) + 0xa0) = 0;
						}
					}
				}
				return g_hook_ActionFrameSpeed.call<int>(entity);
			}));
	}
	static void Registe(lua_State* L) {
		engine_logger->info("注册动作帧速率相关函数");
		lua_register(L, "AddFrameSpeed", [](lua_State* pL) -> int
			{
				SpeedList[(void*)(long long)lua_tointeger(pL, 1)] = (float)lua_tonumber(pL, 2);
				return 0;
			});
		lua_register(L, "GetAddFrameSpeed", [](lua_State* pL) -> int
			{
				lua_pushnumber(pL, SpeedList[(void*)(long long)lua_tointeger(pL, 1)]);
				return 1;
			});
	}
}
#pragma endregion

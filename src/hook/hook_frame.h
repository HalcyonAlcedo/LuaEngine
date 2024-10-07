#pragma once

#pragma region frame
namespace hook_frame {
	map<void*, float> SpeedList;
	static void Hook() {
		framework_logger->info("创建动作帧速率调节钩子");
		MH_Initialize();
		HookLambda(MH::World::ActionFrameSpeed,
			[](auto entity) {
				for (auto [ptr, addSpeed] : SpeedList) {
					if (entity == ptr) {
						//0xa4可能用不上
						//1422373E1 -> 0x145224688 + 0x94 可能是补帧程序，这个钩子无法修改，对应位置的钩子不记录数据只进行列表补帧，难受，摆烂了
						float now_speed = *(float*)((long long)*(long long*)(0x1451238C8) + (*(int*)((long long)entity + 0x10) * 0xf8) + 0xa0);
						if (now_speed + addSpeed >= 0) {
							*(float*)((long long)*(long long*)(0x1451238C8) + (*(int*)((long long)entity + 0x10) * 0xf8) + 0xa0) = now_speed + addSpeed;
							//*(float*)((long long)*(long long*)(0x145224688) + 0x94) = now_speed + addSpeed;
							//*(float*)((long long)*(long long*)(0x145183e00) + (*(int*)((long long)utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50 }) + 0x10) * 0xf8) + 0xa4) = now_speed + addSpeed;
						}
						else {
							*(float*)((long long)*(long long*)(0x1451238C8) + (*(int*)((long long)entity + 0x10) * 0xf8) + 0xa0) = 0;
							//*(float*)((long long)*(long long*)(0x145224688) + 0x94) = 0;
							//*(float*)((long long)*(long long*)(0x145183e00) + (*(int*)((long long)utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50 }) + 0x10) * 0xf8) + 0xa4) = 0;
						}
					}
				}
			return original(entity);
			});
		MH_ApplyQueued();
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

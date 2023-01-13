#pragma once

#pragma region frame
namespace hook_frame {
	map<void*, float> SpeedList;
	static void Hook() {
		MH_Initialize();
		HookLambda(MH::World::ActionFrameSpeed,
			[](auto entity) {
				for (auto [ptr, addSpeed] : SpeedList) {
					if (entity == ptr) {
						float now_speed = *(float*)((long long)*(long long*)(0x145183e00) + (*(int*)((long long)utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50 }) + 0x10) * 0xf8) + 0xa0);
						if (now_speed + addSpeed >= 0)
							*(float*)((long long)*(long long*)(0x145183e00) + (*(int*)((long long)utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50 }) + 0x10) * 0xf8) + 0xa0) = now_speed + addSpeed;
						else
							*(float*)((long long)*(long long*)(0x145183e00) + (*(int*)((long long)utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50 }) + 0x10) * 0xf8) + 0xa0) = 0;
					}
				}
			return original(entity);
			});
		MH_ApplyQueued();
	}
	static void Registe(lua_State* L) {
		lua_register(L, "AddFrameSpeed", [](lua_State* pL) -> int
			{
				SpeedList[(void*)(long long)lua_tointeger(pL, 1)] = (float)lua_tonumber(pL, 2);
				return 0;
			});
	}
}
#pragma endregion

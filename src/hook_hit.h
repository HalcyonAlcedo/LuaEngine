#pragma once

#pragma region hit
namespace hook_hit {
	struct HitData {
		void* HitPlot;
		bool Invulnerable;
		int HitCount;
		HitData(
			void* HitPlot = nullptr,
			bool Invulnerable = false,
			int HitCount = 0)
			:HitPlot(HitPlot), Invulnerable(Invulnerable), HitCount(HitCount) {
		};
	};
	HitData Hit;
	static void Hook() {
		framework_logger->info("创建受击处理钩子");
		MH_Initialize();
		HookLambda(MH::Player::HitPtr,
			[](auto rcx, auto rdx) {
				//受击统计，清除计数器交给逻辑代码，不在这里执行
				Hit.HitCount += 1;
				Hit.HitPlot = *((void**)(rdx + 0x8));
				//如果设定了无敌，则设置rcx为0，后续交给程序自己执行
				if (Hit.Invulnerable) rcx = 0;

				auto ret = original(rcx, rdx);
				return ret;
			});
		MH_ApplyQueued();
	}
	static void Registe(lua_State* L) {
		engine_logger->info("注册受击相关函数");
		lua_register(L, "Invulnerable", [](lua_State* pL) -> int
			{
				Hit.Invulnerable = (bool)lua_toboolean(pL, 1);
				return 0;
			});
		lua_register(L, "GetHitCount", [](lua_State* pL) -> int
			{
				lua_pushinteger(pL, Hit.HitCount);
				return 1;
			});
		lua_register(L, "ResetHitCount", [](lua_State* pL) -> int
			{
				Hit.HitCount = 0;
				return 0;
			});
	}
}
#pragma endregion

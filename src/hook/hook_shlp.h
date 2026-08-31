#pragma once
#include "game_utils.h"

#pragma region shlp
namespace hook_shlp {
	struct ProjectilesData {
		void* Plot;
		int Id;
		ProjectilesData(
			void* Plot = nullptr,
			int Id = 0)
			:Plot(Plot),Id(Id) {
		};
	};
	map<void*, ProjectilesData> ProjectilesList;

	SafetyHookInline g_hook_dtor;
	SafetyHookMid g_hook_ctor;
	static void Hook() {
		framework_logger->info("创建投射物shlp生成和销毁钩子");

		g_hook_ctor = safetyhook::create_mid(MH::Shlp::ctor,
			+[](SafetyHookContext& ctx) {
				try {
					int shlpid = (int)ctx.r12;
					void* shlp = reinterpret_cast<void*>(ctx.rax);
					// mid-hook 时 rax 不一定是已构造好的对象,校验有效性后再入表
					if (shlp == nullptr || !utils::IsMemoryReadable(shlp, sizeof(void*)))
						return;
					ProjectilesList[shlp] = ProjectilesData(shlp, shlpid);
				}
				catch (...) {
					framework_logger->error("投射物生成钩子发生 C++ 异常,已捕获");
				}
			});

		g_hook_dtor = safetyhook::create_inline(MH::Shlp::dtor, reinterpret_cast<void*>(
			+[](void* shlp) {
				try {
					ProjectilesList.erase(shlp);
				}
				catch (...) {
					framework_logger->error("投射物销毁钩子发生 C++ 异常,已捕获");
				}
				return g_hook_dtor.call<int>(shlp);
			}));
	}
	static void Registe(lua_State* L) {
		engine_logger->info("注册投射物shlp相关函数");
		//注册投射物获取函数
		lua_register(L, "GetShlp", [](lua_State* pL) -> int
			{
				lua_newtable(pL);//创建一个表格，放在栈顶
				for (auto [Plot, shlpData] : ProjectilesList) {
					lua_pushinteger(pL, (long long)Plot);
					lua_newtable(pL);//压入编号信息表
					lua_pushstring(pL, "Id");//Id
					lua_pushinteger(pL, (long long)shlpData.Id);
					lua_settable(pL, -3);
					lua_settable(pL, -3);//弹出到顶层
				}
				return 1;
			});
	}
}
#pragma endregion

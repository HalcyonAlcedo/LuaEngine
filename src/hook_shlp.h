#pragma once

extern "C" void* _stdcall GetR12DPtr(void*);

#pragma region shlp
namespace hook_shlp {
	struct ProjectilesData {
		void* Plot;
		void* From;
		int Id;
		ProjectilesData(
			void* Plot = nullptr,
			void* From = nullptr,
			int Id = 0)
			:Plot(Plot), From(From), Id(Id) {
		};
	};
	map<void*, ProjectilesData> ProjectilesList;

	static void Hook() {
		framework_logger->info("创建投射物shlp生成和销毁钩子");
		MH_Initialize();
		HookLambda(MH::Shlp::dtor,
			[](auto rcx) {
				ProjectilesList.erase(rcx);
				return original(rcx);
			});
		HookLambda(MH::Shlp::ctor,
			[]() {
				void* ret = original();
				int shlpid = 0;
				GetR12DPtr(&shlpid);
				ProjectilesList[ret] = ProjectilesData(ret, *offsetPtr<void*>(ret, 0x2B0), shlpid);
				return ret;
			});
		MH_ApplyQueued();
	}
	static void Registe(lua_State* L) {
		engine_logger->info("注册投射物shlp相关函数");
		//注册环境生物获取函数
		lua_register(L, "GetShlp", [](lua_State* pL) -> int
			{
				lua_newtable(pL);//创建一个表格，放在栈顶
				for (auto [Plot, shlpData] : ProjectilesList) {
					lua_pushinteger(pL, (long long)Plot);
					lua_newtable(pL);//压入编号信息表
					lua_pushstring(pL, "From");//来源指针
					lua_pushinteger(pL, (long long)shlpData.From);
					lua_settable(pL, -3);
					lua_pushstring(pL, "Id");//来源指针
					lua_pushinteger(pL, (long long)shlpData.Id);
					lua_settable(pL, -3);
					lua_settable(pL, -3);//弹出到顶层
				}
				return 1;
			});
	}
}
#pragma endregion

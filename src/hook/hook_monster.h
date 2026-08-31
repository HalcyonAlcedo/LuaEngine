#pragma once

#pragma region monster
namespace hook_monster {
	struct MonsterData {
		void* Plot;
		int Id;
		int SubId;
		MonsterData(
			void* Plot = nullptr,
			int Id = 0,
			int SubId = 0)
			:Plot(Plot), Id(Id), SubId(SubId) {
		};
	};
	map<void*, MonsterData> Monsters;
	SafetyHookInline g_hook_ctor, g_hook_dtor;
	static void Hook() {
		framework_logger->info("创建怪物生成和销毁钩子");

		g_hook_ctor = safetyhook::create_inline(MH::Monster::ctor, reinterpret_cast<void*>(
			+[](void* monster, int id, int subId) {
				try {
					auto ret = g_hook_ctor.call<int>(monster, id, subId);
					Monsters[monster] = MonsterData(
						monster, id, subId
					);
					LuaCore::run("on_monster_create");
					return ret;
				}
				catch (...) {
					// 异常无法跨游戏帧展开,就地捕获防止 terminate
					framework_logger->error("怪物生成钩子发生 C++ 异常,已捕获");
					return 0;
				}
			}));
		g_hook_dtor = safetyhook::create_inline(MH::Monster::dtor, reinterpret_cast<void*>(
			+[](void* monster) {
				try {
					Monsters.erase(monster);
					LuaCore::run("on_monster_destroy");
				}
				catch (...) {
					framework_logger->error("怪物销毁钩子发生 C++ 异常,已捕获");
				}
				return g_hook_dtor.call<int>(monster);
			}));
	}
	static void Registe(lua_State* L) {
		//注册怪物获取函数
		engine_logger->info("注册怪物相关函数");
		lua_register(L, "GetAllMonster", [](lua_State* pL) -> int
			{
				lua_newtable(pL);//创建一个表格，放在栈顶
				for (auto [monster, monsterData] : Monsters) {
					if (monster != nullptr) {
						lua_pushinteger(pL, (long long)monster);//压入地址
						lua_newtable(pL);//压入编号信息表
						lua_pushstring(pL, "Id");//怪物Id
						lua_pushinteger(pL, monsterData.Id);
						lua_settable(pL, -3);
						lua_pushstring(pL, "SubId");//怪物SubId
						lua_pushinteger(pL, monsterData.SubId);
						lua_settable(pL, -3);
						lua_settable(pL, -3);//弹出到顶层
					}
				}
				return 1;
			});
	}
}
#pragma endregion

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
	static void Hook() {
		MH_Initialize();
		HookLambda(MH::Monster::ctor,
			[](auto monster, auto id, auto subId) {
				auto ret = original(monster, id, subId);
				Monsters[monster] = MonsterData(
					monster, id, subId
				);
				LuaCore::run("on_monster_create");
				return ret;
			});
		HookLambda(MH::Monster::dtor,
			[](auto monster) {
				Monsters.erase(monster);
				LuaCore::run("on_monster_destroy");
				return original(monster);
			});
		MH_ApplyQueued();
		//注册怪物获取函数
		LuaCore::Lua_register("GetAllMonster", [](lua_State * pL) -> int
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

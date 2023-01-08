#pragma once

#pragma region animals
namespace hook_animals {
	struct AnimalsData {
		void* Plot;
		int Id;
		int SubId;
		AnimalsData(
			void* Plot = nullptr,
			int Id = 0,
			int SubId = 0)
			:Plot(Plot), Id(Id), SubId(SubId) {
		};
	};
	map<void*, AnimalsData> Animals;
	static void Hook() {
		MH_Initialize();	
		HookLambda(MH::EnvironmentalBiological::ctor,
			[](auto environmental, auto id, auto subId) {
				auto ret = original(environmental, id, subId);
				Animals[environmental] = AnimalsData(
					environmental, id, subId
				);
				return ret;
			});
		HookLambda(MH::EnvironmentalBiological::dtor,
			[](auto environmental) {
				Animals.erase(environmental);
				return original(environmental);
			});
		MH_ApplyQueued();
		//注册环境生物获取函数
		LuaCore::Lua_register("GetAllAnimals", [](lua_State* pL) -> int
			{
				lua_newtable(pL);//创建一个表格，放在栈顶
				for (auto [animals, animalsData] : Animals) {
					if (animals != nullptr) {
						lua_pushinteger(pL, (long long)animals);//压入地址
						lua_newtable(pL);//压入编号信息表
						lua_pushstring(pL, "Id");//生物Id
						lua_pushinteger(pL, animalsData.Id);
						lua_settable(pL, -3);
						lua_pushstring(pL, "SubId");//生物SubId
						lua_pushinteger(pL, animalsData.SubId);
						lua_settable(pL, -3);
						lua_settable(pL, -3);//弹出到顶层
					}
				}
				return 1;
			});
	}
}
#pragma endregion

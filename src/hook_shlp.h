#pragma once

#pragma region shlp
namespace hook_shlp {
	struct ProjectilesData {
		void* Plot;
		void* From;
		float CoordinatesX;
		float CoordinatesY;
		float CoordinatesZ;
		ProjectilesData(
			void* Plot = nullptr,
			void* From = nullptr,
			float CoordinatesX = 0,
			float CoordinatesY = 0,
			float CoordinatesZ = 0)
			:Plot(Plot), From(From), CoordinatesX(CoordinatesX), CoordinatesY(CoordinatesY), CoordinatesZ(CoordinatesZ) {
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
			[](auto rcx) {
				ProjectilesList[rcx] = ProjectilesData(rcx, *offsetPtr<void*>(rcx, 0x2B0), 0, 0, 0);
				return original(rcx);
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
					ostringstream ptr;
					ptr << shlpData.Plot;
					string ptrstr = ptr.str();
					lua_pushstring(pL, ptrstr.c_str());//压入编号
					lua_newtable(pL);//压入编号信息表
					lua_pushstring(pL, "X");//X坐标
					lua_pushnumber(pL, shlpData.CoordinatesX);//value
					lua_settable(pL, -3);//弹出X坐标
					lua_pushstring(pL, "Y");//Y坐标
					lua_pushnumber(pL, shlpData.CoordinatesY);
					lua_settable(pL, -3);
					lua_pushstring(pL, "Z");//Z坐标
					lua_pushnumber(pL, shlpData.CoordinatesZ);
					lua_settable(pL, -3);
					lua_pushstring(pL, "Ptr");//投射物指针
					lua_pushstring(pL, ptrstr.c_str());
					lua_settable(pL, -3);
					lua_pushstring(pL, "From");//来源指针
					ostringstream fromptr;
					fromptr << shlpData.From;
					string fromptrptrstr = fromptr.str();
					lua_pushstring(pL, fromptrptrstr.c_str());
					lua_settable(pL, -3);

					lua_settable(pL, -3);//弹出到顶层
				}
				return 1;
			});
	}
}
#pragma endregion

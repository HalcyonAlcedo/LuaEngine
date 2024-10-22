#pragma once

#pragma region camera
namespace hook_camera {
	struct CameraData {
		float position_x;
		float position_y;
		float position_z;
		float lockPos_x;
		float lockPos_y;
		float lockPos_z;
		bool lock;
		CameraData(
			float position_y = 0,
			float position_z = 0,
			float lockPos_x = 0,
			float lockPos_y = 0,
			float lockPos_z = 0,
			bool lock = false)
			:position_y(position_y), position_z(position_z), lockPos_x(lockPos_x), lockPos_y(lockPos_y), lockPos_z(lockPos_z) , lock(lock) {
		};
	};
	CameraData Camera;
	SafetyHookMid g_hook_visual;
	static void Hook() {
		framework_logger->info("创建相机操作钩子");
		g_hook_visual = safetyhook::create_mid(MH::Player::Visual,
			+[](SafetyHookContext& ctx) {
				void* rcx = reinterpret_cast<void*>(ctx.rcx);
				Camera.position_x = *offsetPtr<float>(rcx, 0x10);
				Camera.position_y = *offsetPtr<float>(rcx, 0x14);
				Camera.position_z = *offsetPtr<float>(rcx, 0x18);
				if (Camera.lock) {
					*offsetPtr<float>(rcx, 0x10) = Camera.lockPos_x;
					*offsetPtr<float>(rcx, 0x14) = Camera.lockPos_y;
					*offsetPtr<float>(rcx, 0x18) = Camera.lockPos_z;
				}
			});
	}
	static void Registe(lua_State* L) {
		engine_logger->info("注册相机相关函数");
		lua_register(L, "GetCameraData", [](lua_State* pL) -> int
			{
				lua_pushnumber(pL, Camera.position_x);
				lua_pushnumber(pL, Camera.position_y);
				lua_pushnumber(pL, Camera.position_z);
				lua_pushboolean(pL, Camera.lock);
				return 4;
			});
		lua_register(L, "SetCameraData", [](lua_State* pL) -> int
			{
				Camera.lock = (bool)lua_toboolean(pL, 1);
				if (Camera.lock) {
					Camera.lockPos_x = (float)lua_tonumber(pL, 2);
					Camera.lockPos_y = (float)lua_tonumber(pL, 3);
					Camera.lockPos_z = (float)lua_tonumber(pL, 4);
				}
				return 0;
			});
	}
}
#pragma endregion

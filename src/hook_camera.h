#pragma once

extern "C" void* _stdcall GetRBXPtr(void*);

#pragma region camera
namespace hook_camera {
	struct CameraData {
		void* Plot;
		float position_x;
		float position_y;
		float position_z;
		float lockPos_x;
		float lockPos_y;
		float lockPos_z;
		bool lock;
		CameraData(
			void* Plot = nullptr,
			float position_y = 0,
			float position_z = 0,
			float lockPos_x = 0,
			float lockPos_y = 0,
			float lockPos_z = 0,
			bool lock = false)
			:Plot(Plot), position_y(position_y), position_z(position_z), lockPos_x(lockPos_x), lockPos_y(lockPos_y), lockPos_z(lockPos_z) , lock(lock) {
		};
	};
	CameraData Camera;
	static void Hook() {
		framework_logger->info("创建相机操作钩子");
		MH_Initialize();
		HookLambda(MH::Player::Visual,
			[]() {
				GetRBXPtr(&Camera.Plot);
				if (Camera.Plot != nullptr) {
					Camera.position_x = *offsetPtr<float>(Camera.Plot, 0x0);
					Camera.position_y = *offsetPtr<float>(Camera.Plot, 0x4);
					Camera.position_z = *offsetPtr<float>(Camera.Plot, 0x8);
					if (Camera.lock) {
						*offsetPtr<float>(Camera.Plot, 0x0) = Camera.lockPos_x;
						*offsetPtr<float>(Camera.Plot, 0x4) = Camera.lockPos_y;
						*offsetPtr<float>(Camera.Plot, 0x8) = Camera.lockPos_z;
					}
				}
				return original();
			});
		MH_ApplyQueued();
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

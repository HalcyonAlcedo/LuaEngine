#include "Extension.hpp"

namespace fs = std::filesystem;

typedef int (*InitializeFunc)(lua_State*);
typedef int (*DeInitializeFunc)(lua_State*);

int ExtensionMgr::load(const std::string& dllPath) {
	HMODULE hModule = LoadLibraryA(dllPath.c_str());
	if (!hModule) {
		framework_logger->error("加载拓展 {} 失败", dllPath);
		return -1;
	}
	extensions.insert(std::pair(dllPath, hModule));

	return 0;
};

//int ExtensionMgr::init(HMODULE hModule, lua_State* luaState) {
//	// 获取函数指针  
//	InitializeFunc initFunc = reinterpret_cast<InitializeFunc>(GetProcAddress(hModule, "Init"));
//	if (!initFunc) {
//		framework_logger->error("无法从拓展中获取初始化函数");
//		return -1;
//	}
//
//	// 调用初始化函数  
//	int result = initFunc(luaState);
//	if (result != 0) {
//		framework_logger->error("拓展初始化失败 code={}", result);
//		return result;
//	}
//
//	return 0;
//};
//
//int ExtensionMgr::deinit(HMODULE hModule, lua_State* luaState) {
//	// 获取函数指针  
//	InitializeFunc deinitFunc = reinterpret_cast<DeInitializeFunc>(GetProcAddress(hModule, "Deinit"));
//	if (!deinitFunc) {
//		framework_logger->error("无法从拓展中获取销毁函数");
//		return -1;
//	}
//
//	// 调用销毁函数  
//	int result = deinitFunc(luaState);
//	if (result != 0) {
//		framework_logger->error("拓展销毁失败 code={}", result);
//		return result;
//	}
//
//	return 0;
//};

int ExtensionMgr::loadAll(const std::string& dirPath) {
	int success_count = 0;

	for (const auto& entry : fs::directory_iterator(dirPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".dll") {
			std::string dllPath = entry.path().string();

			if (load(dllPath)) {
				success_count++;
			};
		}
	}

	return success_count;
};

//int ExtensionMgr::initAll(lua_State* luaState) {
//	int success_count = 0;
//
//	for (auto pair : extensions) {
//		if (init(pair.second, luaState)) {
//			success_count++;
//		};
//	}
//
//	return success_count;
//};
//
//int ExtensionMgr::deinitAll(lua_State* luaState) {
//	int success_count = 0;
//
//	for (auto pair : extensions) {
//		if (deinit(pair.second, luaState)) {
//			success_count++;
//		};
//	}
//
//	return success_count;
//};
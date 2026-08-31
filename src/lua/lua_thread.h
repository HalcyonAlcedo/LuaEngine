#pragma once
#include "core.h"

#pragma region thread

namespace lua_thread {

	static void AddTask(sol::this_state s, sol::protected_function task_func) {
		sol::state_view lua(s);
		uint64_t gen = LuaEngine::StateGeneration().load();
		// 在一个子线程中新建一个 sol::thread
		std::thread t([lua, task_func, gen]() mutable {
			// 协程对象与主 lua_State 同属一个状态,必须持有全局 Lua 互斥体,
			// 与主线程/钩子线程的 Lua 执行串行化,避免 lua_State 并发访问。
			// 锁的生命周期覆盖任务执行与所有 lua_State 引用的释放
			// (sol::thread / protected_function / result 均在锁内析构),
			// 防止与脚本重载(reload)中的 lua_close 竞争。
			std::lock_guard<std::recursive_mutex> luaLock(LuaEngine::LuaMutex());
			// 排队期间脚本被重载(状态已销毁重建),任务作废。
			// 此时捕获的 task_func 已被 move 为空引用,析构无害。
			if (gen != LuaEngine::StateGeneration().load())
				return;
			// 将任务函数移动到锁作用域内,确保其引用在锁内释放
			sol::protected_function protected_task_func(std::move(task_func));
			// 新建一个 sol::thread
			sol::thread t = sol::thread::create(lua);
			// 从 sol::thread 中获取 sol::state_view
			sol::state_view new_lua = t.state();
			// 在新的 sol::state_view 中执行任务函数
			sol::protected_function_result result = protected_task_func(new_lua);
			// 检查执行是否成功
			if (!result.valid()) {
				// 获取错误信息
				sol::error err = result;
				std::string what = err.what();
				lua_logger->error("LuaEngine Thread Error:\n" + what);
				LOG(ERR) << "LuaEngine Thread Error:\n" + what;
			}
			// 在锁内显式释放对 lua_State 的引用
			protected_task_func = {};
		});
		t.detach();
	}

	static void Registe(lua_State* L) {
		engine_logger->info("注册多线程相关函数");
		sol::state_view lua(L);
		//注册多线程相关函数
		lua.set_function("addTask", AddTask);
	}
}
#pragma endregion

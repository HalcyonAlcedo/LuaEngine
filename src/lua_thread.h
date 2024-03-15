#pragma once

#pragma region thread

namespace lua_thread {

	static void AddTask(sol::this_state s, sol::protected_function task_func) {
		sol::state_view lua(s);
		// 在一个子线程中新建一个 sol::thread
		std::thread t([lua, task_func] {
			// 新建一个 sol::thread
			sol::thread t = sol::thread::create(lua);
			// 从 sol::thread 中获取 sol::state_view
			sol::state_view new_lua = t.state();
			// 将任务函数包装在 protected_function 中
			sol::protected_function protected_task_func(task_func);
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

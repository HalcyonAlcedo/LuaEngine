#pragma once
#include <mutex>
#include <atomic>

// LuaEngine 全局同步原语
//
// 线程模型:游戏逻辑(时间钩子、怪物/投射物 ctor/dtor 等)基本运行在游戏主线程,
// 但 addTask 工作线程与 luaHook(InstallHook)回调可能在任意线程执行 Lua。
// 一个 lua_State 本身不是线程安全的,因此所有对 lua_State、LuaCore::LuaScript、
// hook_general::hookMap 以及各全局数据容器的访问,必须持有 LuaMutex。
//
// 使用 recursive_mutex 允许同一线程重入(例如在 pcall 持有锁时,
// Lua 内部再调用 InstallHook / SetAddressData 等注册函数)。
//
// 锁序约定:任何代码路径同一时刻至多持有一把锁(LuaMutex 与各模块私有锁
// 不在嵌套持有),避免跨线程死锁。主线程上的 Lua 入口(LuaCore::run、
// 时间钩子)使用 try_lock,拿不到锁时跳过本次执行,绝不阻塞主线程;
// 工作线程(addTask、luaHook 回调)则阻塞等待,保证互斥。
namespace LuaEngine {
inline std::recursive_mutex& LuaMutex() {
    static std::recursive_mutex mutex;
    return mutex;
}

// 脚本状态代数:每次 reload 关闭并重建某个 lua_State 时自增。
// addTask 工作线程在启动任务前对比捕获的代数,不一致说明状态已被
// 销毁重建,任务作废,避免在已关闭的状态上执行 Lua。
inline std::atomic<uint64_t>& StateGeneration() {
    static std::atomic<uint64_t> generation{0};
    return generation;
}
}

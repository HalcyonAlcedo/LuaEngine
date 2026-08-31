#pragma once
#include <windows.h>
#include <intrin.h>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "core.h"
#include "game_utils.h"

// 通用 Lua 钩子(luaHook / InstallHook)
//
// 设计要点(修复 v1.2.8 之前的实现缺陷):
// 1. hookMap 的所有读写都在 HookMapMutex 保护下,禁止在回调线程中用
//    operator[] 插入;
// 2. 回调线程不再直接裸调 lua_pcall,而是先获取全局 LuaEngine::LuaMutex,
//    与主线程/其他钩子线程的 Lua 执行串行化,避免同一 lua_State 被并发访问;
// 3. 不再扫描 rip 之后的 64 字节猜测跳转目标(旧实现可能越页读取崩溃,
//    FF25 的相对位移解析也是错的)。每个钩子通过 stub 调用点(返回地址)
//    唯一映射回目标地址,由创建时验证保证正确性。
namespace hook_general {

    // 类型别名:lua_register 是宏,模板实参中的逗号会破坏宏参数切分,
    // 必须用无逗号的别名(宏调用参数里不能出现裸的 <..., ...> 逗号)
    using LuaHookMap = std::map<lua_State*, int>;

    struct HookData {
        // 写时复制(COW)的注册表:分发线程只拷贝 shared_ptr(原子引用计数),
        // 不再逐节点拷贝整个 map;任何修改都生成新副本,读者持有的
        // 旧快照始终有效,消除高频钩子命中时的堆分配。
        std::shared_ptr<LuaHookMap> Lua;
        SafetyHookMid hook{};
    };

    // SafetyHook mid-hook stub 中 "call 目标回调" 指令的下一条指令偏移。
    // 回调内 _ReturnAddress() == stub + kStubCallReturnOffset。
    // 该偏移由 vendored 的 deps/safetyhook/safetyhook.cpp 中 asm_data 固定,
    // 创建钩子时会验证,若未来 safetyhook 布局变化将拒绝安装并记录错误。
    constexpr uint64_t kStubCallReturnOffset = 207;

    std::unordered_map<uint64_t, HookData> hookMap;
    // stub 调用点返回地址 -> 被钩目标地址
    std::unordered_map<uint64_t, uint64_t> dispatchMap;

    inline std::mutex& HookMapMutex() {
        static std::mutex mutex;
        return mutex;
    }

    void clearLuaHook(lua_State* L) {
        std::lock_guard<std::mutex> lock(HookMapMutex());
        for (auto it = hookMap.begin(); it != hookMap.end(); ) {
            auto& luaRef = it->second.Lua;
            if (!luaRef || luaRef->find(L) == luaRef->end()) {
                ++it;
                continue;
            }
            // 写时复制:分发线程可能正持有旧快照,不能原地修改
            auto next = std::make_shared<LuaHookMap>(*luaRef);
            next->erase(L);
            luaRef = next;
            if (next->empty()) {
                dispatchMap.erase(it->second.hook.stub() ? (uint64_t)it->second.hook.stub() + kStubCallReturnOffset : 0);
                it = hookMap.erase(it); // erase 返回下一个有效迭代器
            }
            else {
                ++it;
            }
        }
    }

    // 将寄存器上下文打包为 Lua 表
    static void PushRegistersTable(lua_State* L, SafetyHookContext& ctx) {
        lua_createtable(L, 0, 31); // 预分配 31 个字段,避免逐项插入时的多次 rehash
        lua_pushstring(L, "rax"); lua_pushinteger(L, ctx.rax); lua_settable(L, -3);
        lua_pushstring(L, "rbx"); lua_pushinteger(L, ctx.rbx); lua_settable(L, -3);
        lua_pushstring(L, "rcx"); lua_pushinteger(L, ctx.rcx); lua_settable(L, -3);
        lua_pushstring(L, "rdx"); lua_pushinteger(L, ctx.rdx); lua_settable(L, -3);
        lua_pushstring(L, "rsi"); lua_pushinteger(L, ctx.rsi); lua_settable(L, -3);
        lua_pushstring(L, "rdi"); lua_pushinteger(L, ctx.rdi); lua_settable(L, -3);
        lua_pushstring(L, "r8"); lua_pushinteger(L, ctx.r8); lua_settable(L, -3);
        lua_pushstring(L, "r9"); lua_pushinteger(L, ctx.r9); lua_settable(L, -3);
        lua_pushstring(L, "r10"); lua_pushinteger(L, ctx.r10); lua_settable(L, -3);
        lua_pushstring(L, "r11"); lua_pushinteger(L, ctx.r11); lua_settable(L, -3);
        lua_pushstring(L, "r12"); lua_pushinteger(L, ctx.r12); lua_settable(L, -3);
        lua_pushstring(L, "r13"); lua_pushinteger(L, ctx.r13); lua_settable(L, -3);
        lua_pushstring(L, "r14"); lua_pushinteger(L, ctx.r14); lua_settable(L, -3);
        lua_pushstring(L, "r15"); lua_pushinteger(L, ctx.r15); lua_settable(L, -3);
        lua_pushstring(L, "rsp"); lua_pushinteger(L, ctx.rsp); lua_settable(L, -3);
        lua_pushstring(L, "xmm0"); lua_pushnumber(L, ctx.xmm0.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm1"); lua_pushnumber(L, ctx.xmm1.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm2"); lua_pushnumber(L, ctx.xmm2.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm3"); lua_pushnumber(L, ctx.xmm3.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm4"); lua_pushnumber(L, ctx.xmm4.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm5"); lua_pushnumber(L, ctx.xmm5.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm6"); lua_pushnumber(L, ctx.xmm6.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm7"); lua_pushnumber(L, ctx.xmm7.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm8"); lua_pushnumber(L, ctx.xmm8.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm9"); lua_pushnumber(L, ctx.xmm9.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm10"); lua_pushnumber(L, ctx.xmm10.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm11"); lua_pushnumber(L, ctx.xmm11.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm12"); lua_pushnumber(L, ctx.xmm12.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm13"); lua_pushnumber(L, ctx.xmm13.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm14"); lua_pushnumber(L, ctx.xmm14.f32[0]); lua_settable(L, -3);
        lua_pushstring(L, "xmm15"); lua_pushnumber(L, ctx.xmm15.f32[0]); lua_settable(L, -3);
    }

    // 从 Lua 返回的表中读回寄存器修改
    static void ReadRegistersTable(lua_State* L, SafetyHookContext& ctx) {
        lua_pushstring(L, "rax"); lua_gettable(L, -2); ctx.rax = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "rbx"); lua_gettable(L, -2); ctx.rbx = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "rcx"); lua_gettable(L, -2); ctx.rcx = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "rdx"); lua_gettable(L, -2); ctx.rdx = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "rsi"); lua_gettable(L, -2); ctx.rsi = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "rdi"); lua_gettable(L, -2); ctx.rdi = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r8"); lua_gettable(L, -2); ctx.r8 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r9"); lua_gettable(L, -2); ctx.r9 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r10"); lua_gettable(L, -2); ctx.r10 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r11"); lua_gettable(L, -2); ctx.r11 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r12"); lua_gettable(L, -2); ctx.r12 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r13"); lua_gettable(L, -2); ctx.r13 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r14"); lua_gettable(L, -2); ctx.r14 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "r15"); lua_gettable(L, -2); ctx.r15 = lua_tointeger(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm0"); lua_gettable(L, -2); ctx.xmm0.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm1"); lua_gettable(L, -2); ctx.xmm1.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm2"); lua_gettable(L, -2); ctx.xmm2.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm3"); lua_gettable(L, -2); ctx.xmm3.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm4"); lua_gettable(L, -2); ctx.xmm4.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm5"); lua_gettable(L, -2); ctx.xmm5.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm6"); lua_gettable(L, -2); ctx.xmm6.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm7"); lua_gettable(L, -2); ctx.xmm7.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm8"); lua_gettable(L, -2); ctx.xmm8.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm9"); lua_gettable(L, -2); ctx.xmm9.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm10"); lua_gettable(L, -2); ctx.xmm10.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm11"); lua_gettable(L, -2); ctx.xmm11.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm12"); lua_gettable(L, -2); ctx.xmm12.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm13"); lua_gettable(L, -2); ctx.xmm13.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm14"); lua_gettable(L, -2); ctx.xmm14.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
        lua_pushstring(L, "xmm15"); lua_gettable(L, -2); ctx.xmm15.f32[0] = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }

    // 单个回调的执行体:压表 -> pcall -> 回写寄存器。
    // 拆出独立函数以便外层捕获 C++ 异常(__try 与 try/catch 不能同函数混用)。
    static void ExecuteLuaHook(lua_State* L, int luaFuncRef, SafetyHookContext& ctx, uint64_t target);

    // 注销某个状态在指定目标上的回调(COW,可安全与分发并发)
    static void UnregisterLuaHook(lua_State* L, uint64_t target);

    // 在命中钩子的线程上执行 Lua 回调。
    // 持有全局 Lua 互斥体,确保与主线程及其他钩子线程的 Lua 执行串行。
    // gen 为钩子触发时捕获的状态代数:拿到锁后若代数已变化,说明脚本
    // 已被 reload 关闭重建,放弃执行,避免在已销毁的 lua_State 上运行。
    void DispatchLuaHook(SafetyHookContext& ctx, uint64_t target, uint64_t gen) {
        // COW 快照:仅拷贝 shared_ptr(原子引用计数),无节点分配
        std::shared_ptr<LuaHookMap> snapshot;
        {
            std::lock_guard<std::mutex> lock(HookMapMutex());
            auto it = hookMap.find(target);
            if (it == hookMap.end())
                return;
            snapshot = it->second.Lua;
        }
        if (!snapshot)
            return;

        for (auto& pair : *snapshot) {
            lua_State* L = pair.first;
            int luaFuncRef = pair.second;

            std::lock_guard<std::recursive_mutex> luaLock(LuaEngine::LuaMutex());
            // 锁内校验:reload 需要同一把锁才能关闭状态,此时检查是可靠的
            if (gen != LuaEngine::StateGeneration().load())
                return;
            try {
                ExecuteLuaHook(L, luaFuncRef, ctx, target);
            }
            catch (...) {
                // 钩子回调中的 C++ 异常无法跨游戏代码帧展开,就地捕获并
                // 注销该回调,避免每帧反复抛出
                LOG(ERR) << "LuaEngine: Lua hook callback threw a C++ exception, unregistered.";
                UnregisterLuaHook(L, target);
            }
        }
    }

    // 注销某个状态在指定目标上的回调(COW,可安全与分发并发)
    static void UnregisterLuaHook(lua_State* L, uint64_t target) {
        std::lock_guard<std::mutex> hlock(HookMapMutex());
        auto it = hookMap.find(target);
        if (it == hookMap.end() || !it->second.Lua)
            return;
        auto jt = it->second.Lua->find(L);
        if (jt == it->second.Lua->end())
            return;
        luaL_unref(L, LUA_REGISTRYINDEX, jt->second);
        auto next = std::make_shared<LuaHookMap>(*it->second.Lua);
        next->erase(L);
        it->second.Lua = next;
    }

    // 单个回调的执行体:压表 -> pcall -> 回写寄存器。
    static void ExecuteLuaHook(lua_State* L, int luaFuncRef, SafetyHookContext& ctx, uint64_t target) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaFuncRef); // 获取 Lua 函数
        PushRegistersTable(L, ctx);                    // 压入寄存器数据表

        if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
            const char* errMsg = lua_tostring(L, -1);
            LOG(ERR) << "Error in Lua hook function: " << (errMsg ? errMsg : "unknown error");
            lua_pop(L, 1); // 弹出错误信息
            // 出错后注销该回调,避免每帧反复报错,并释放注册表引用
            UnregisterLuaHook(L, target);
            return;
        }

        // 读取 Lua 返回的表并回写寄存器
        if (lua_istable(L, -1)) {
            ReadRegistersTable(L, ctx);
        }
        lua_pop(L, 1); // 弹出返回的表
    }

    // 所有钩子共用的回调入口:通过返回地址识别触发钩子的 stub,再分发到目标
    void HookHandler(SafetyHookContext& ctx) {
        uint64_t gen = LuaEngine::StateGeneration().load();
        uint64_t retAddr = (uint64_t)_ReturnAddress();
        uint64_t target = 0;
        {
            std::lock_guard<std::mutex> lock(HookMapMutex());
            auto it = dispatchMap.find(retAddr);
            if (it == dispatchMap.end())
                return;
            target = it->second;
        }
        DispatchLuaHook(ctx, target, gen);
    }

    void InstallHook(lua_State* L, void* targetAddr, int luaFuncRef) {
        uint64_t target = (uint64_t)targetAddr;

        // 目标必须位于可读可执行的代码区域,否则 SafetyHook 内部解码会崩溃
        if (!utils::IsExecutableMemory(targetAddr) || !utils::IsMemoryReadable(targetAddr, 16)) {
            framework_logger->error("InstallHook: invalid target address 0x{:X}", target);
            luaL_unref(L, LUA_REGISTRYINDEX, luaFuncRef); // 释放本次注册的引用
            return;
        }

        // 钩挂 LuaEngine 自身模块会破坏框架运行,记录警告
        {
            HMODULE self = GetModuleHandleA("LuaEngine.dll");
            if (self) {
                MODULEINFO mi{};
                if (GetModuleInformation(GetCurrentProcess(), self, &mi, sizeof(mi)) &&
                    target >= (uint64_t)self && target < (uint64_t)self + mi.SizeOfImage) {
                    framework_logger->warn("InstallHook: target 0x{:X} is inside LuaEngine.dll itself, this may break the framework", target);
                }
            }
        }

        std::lock_guard<std::mutex> lock(HookMapMutex());
        auto& data = hookMap[target];
        // 写时复制注册表:分发线程可能正持有旧快照
        auto next = std::make_shared<LuaHookMap>(data.Lua ? *data.Lua : LuaHookMap());
        auto existing = next->find(L);
        if (existing != next->end()) {
            luaL_unref(L, LUA_REGISTRYINDEX, existing->second); // 释放被覆盖的旧函数引用
            existing->second = luaFuncRef;
        }
        else {
            (*next)[L] = luaFuncRef;
        }
        data.Lua = next;

        if (!data.hook) {
            data.hook = safetyhook::create_mid(targetAddr, HookHandler);
            if (!data.hook) {
                framework_logger->error("InstallHook: create_mid failed for 0x{:X}", target);
                luaL_unref(L, LUA_REGISTRYINDEX, luaFuncRef); // 释放本次注册的引用
                // 回滚注册表(已持有 HookMapMutex,直接内联 COW)
                if (data.Lua) {
                    auto rollback = std::make_shared<LuaHookMap>(*data.Lua);
                    rollback->erase(L);
                    data.Lua = rollback;
                    if (rollback->empty())
                        hookMap.erase(target);
                }
                return;
            }
            // 验证 stub 布局并记录"返回地址 -> 目标"映射
            uint8_t* stub = data.hook.stub();
            if (!stub || stub[201] != 0xFF || stub[202] != 0x15) {
                framework_logger->error("InstallHook: unexpected safetyhook stub layout, hook disabled");
                data.hook.reset();
                luaL_unref(L, LUA_REGISTRYINDEX, luaFuncRef); // 释放本次注册的引用
                if (data.Lua) {
                    auto rollback = std::make_shared<LuaHookMap>(*data.Lua);
                    rollback->erase(L);
                    data.Lua = rollback;
                    if (rollback->empty())
                        hookMap.erase(target);
                }
                return;
            }
            dispatchMap[(uint64_t)stub + kStubCallReturnOffset] = target;
            framework_logger->info("InstallHook: hooked 0x{:X}", target);
        }
    }

    static void Registe(lua_State* L) {
        engine_logger->info("注册通用钩子函数");
        lua_register(L, "InstallHook", [](lua_State* L) -> int {
            void* targetAddr = (void*)lua_tointeger(L, 1);  // 获取目标地址
            int luaFuncRef = luaL_ref(L, LUA_REGISTRYINDEX); // 获取 Lua 函数引用
            InstallHook(L, targetAddr, luaFuncRef);          // 安装钩子
            return 0;
        });

        lua_register(L, "UninstallHook", [](lua_State* L) -> int {
            void* targetAddr = (void*)lua_tointeger(L, 1);
            bool removed = false;
            {
                std::lock_guard<std::mutex> lock(HookMapMutex());
                auto it = hookMap.find((uint64_t)targetAddr);
                if (it != hookMap.end() && it->second.Lua) {
                    auto jt = it->second.Lua->find(L);
                    if (jt != it->second.Lua->end()) {
                        luaL_unref(L, LUA_REGISTRYINDEX, jt->second); // 释放注册表引用
                        auto next = std::make_shared<LuaHookMap>(*it->second.Lua); // 写时复制
                        next->erase(L);
                        it->second.Lua = next;
                        removed = true;
                    }
                    if (it->second.Lua->empty()) {
                        uint8_t* stub = it->second.hook.stub();
                        if (stub)
                            dispatchMap.erase((uint64_t)stub + kStubCallReturnOffset);
                        hookMap.erase(it);
                    }
                }
            }
            lua_pushboolean(L, removed);
            return 1;
        });
    }
}

# LuaEngine - Monster Hunter World Lua 模组框架
# 构建:cmake -S . -B build-cmake -G Ninja && cmake --build build-cmake
# (VS2022 自带 Ninja 与 CMake;也可使用 Visual Studio 生成器)
cmake_minimum_required(VERSION 3.21)
project(LuaEngine VERSION 1.3.2 LANGUAGES C CXX)

# 单配置生成器默认 Release(与原 vcxproj 的 Release|x64 对齐)
if(CMAKE_BUILD_TYPE STREQUAL "" AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 静态 CRT(/MT),与原工程一致
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# safetyhook.hpp 使用 std::expected,需要 C++23(原工程为 /std:c++latest)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    # /utf-8:全库源码统一为 UTF-8 编码
    # /EHsc:同步异常处理(SafetyHook/sol2 需要)
    # /MP:并行编译
    add_compile_options(/W3 /utf-8 /EHsc /MP)
endif()

# 依赖完整性检查
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/spdlog/spdlog.h")
    message(FATAL_ERROR "缺少 deps/spdlog(header-only spdlog)。请将 spdlog 头文件放入 deps/spdlog 目录。")
endif()

# ---------------------------------------------------------------------------
# lua 静态库(内置 Lua 5.4 + lfs 文件系统模块)
# ---------------------------------------------------------------------------
file(GLOB LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/src/*.c")
add_library(lua STATIC ${LUA_SOURCES})
set_source_files_properties(${LUA_SOURCES} PROPERTIES LANGUAGE C)
target_include_directories(lua PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/src")
target_compile_definitions(lua PRIVATE _CRT_SECURE_NO_WARNINGS)

# ---------------------------------------------------------------------------
# 可选:内置 lua 解释器,用于脚本语法检查(默认不参与构建)
# 用法:cmake --build build-cmake --config Release --target luacheck
#       build-cmake/Release/luacheck.exe -e "assert(loadfile('engine/Engine.lua'))"
# ---------------------------------------------------------------------------
add_executable(luacheck EXCLUDE_FROM_ALL deps/lua/src/lua.c)
set_source_files_properties(deps/lua/src/lua.c PROPERTIES LANGUAGE C)
target_link_libraries(luacheck PRIVATE lua)

# ---------------------------------------------------------------------------
# LuaEngine 主模块(DLL)
# ---------------------------------------------------------------------------
add_library(LuaEngine SHARED
    dllmain.cpp
    src/CircularBufferLogger.cpp
    src/md5.cpp
    src/Player.cpp
    src/Sound.cpp
    deps/safetyhook/safetyhook.cpp
    deps/safetyhook/Zydis.c
    LuaEngine.rc
)
set_source_files_properties(deps/safetyhook/Zydis.c PROPERTIES LANGUAGE C)

target_include_directories(LuaEngine PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hook"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lua"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/safetyhook"
    "${CMAKE_CURRENT_SOURCE_DIR}"  # resource.h / LuaEngine.rc
)

target_compile_definitions(LuaEngine PRIVATE
    LuaEngine_EXPORTS
    _WINDOWS
    _USRDLL
    ZYDIS_STATIC_BUILD
)

target_link_libraries(LuaEngine PRIVATE
    lua
    Shlwapi
    Xaudio2
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/loader.lib"
)

# 部署:与原 vcxproj 的 PostBuildEvent 行为一致
set(LUAENGINE_DEPLOY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/build")
add_custom_command(TARGET LuaEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUAENGINE_DEPLOY_ROOT}/nativePC/plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:LuaEngine> "${LUAENGINE_DEPLOY_ROOT}/nativePC/plugins/LuaEngine.dll"
    # 示例脚本(数据窗口)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUAENGINE_DEPLOY_ROOT}/可选脚本/数据窗口"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/examples/examples.lua" "${LUAENGINE_DEPLOY_ROOT}/可选脚本/数据窗口/dataview.lua"
    # 引擎脚本
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUAENGINE_DEPLOY_ROOT}/Lua/modules"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/engine/Engine.lua" "${LUAENGINE_DEPLOY_ROOT}/Lua/Engine.lua"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/engine/modules" "${LUAENGINE_DEPLOY_ROOT}/Lua/modules"
    COMMENT "Deploying LuaEngine to build/ ..."
)

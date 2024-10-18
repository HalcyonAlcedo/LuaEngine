<div align="center">

# LuaEngine

|  [简体中文](./README.md) | **English** |

[![GitHub release](https://img.shields.io/github/release/HalcyonAlcedo/LuaEngine.svg)](https://github.com/HalcyonAlcedo/LuaEngine/releases/latest)

<div align="left">

LuaEngine is a lightweight Lua framework for Monster Hunter World that offers more efficient Lua code performance compared to LuaScript (the previous iteration of this project). It removes less-used features and no longer requires direct access to read and modify game data to do basic functions. Instead, it offers basic memory reading and writing functionality within Lua while retaining a number of game call functions provided by the framework.

## Installation

- Download the latest release from [here](https://github.com/HalcyonAlcedo/LuaEngine/releases/latest)
- Extract the zip file to your MHW game folder.

Lua files are stored in Lua/

## Script Commands

Some commands can be directly entered in the in-game chat box to interact with the framework.
- To execute Lua commands in real-time, use `luac: <command>`.
- To reload a Lua script in the Lua/ folder, use `reload <script name>`.

## Data Parsing Engine

Most game data access and modification is integrated into the data parsing engine. Engine files are stored in `Lua/Engine.lua` and in related files under `Lua/modules/`. The engine is automatically loaded when a script is loaded. Functions of the engine can be called via the `engine` table. To ensure consistency and compatibility when using the engine across different scripts, it's not recommended to modify the engine directly.

## Bound Lua Functions

Relevant operation functions bound and usable in Lua can be referenced in `src/lua_register.h` and all `hook_*.h` files under the `src` directory.

## About the Graphics Rendering System

The graphics rendering system operates using ImGui and only supports DX11 display.

The ImGui bindings being used can be found here: [ImGui Bindings on GitHub](https://github.com/MSeys/sol2_ImGui_Bindings).

## Compatibility with LuaScript

Note: Due to game updates, LuaScript likely doesn't function anymore, so this section may be moot

Due to differences in registered functions, most code is not directly compatible. However, LuaScript functions can be replicated by writing custom Lua function libraries and loading LuaScript scripts afterward. LuaEngine and LuaScript scripts are stored in different directories, and both modules can run simultaneously, although this is not recommended.

## Custom Lua Loading Features

LuaEngine can load DLLs to implement more advanced functions. For an example, refer to the `camera` project in the `plugins` folder, and see `examples/CameraControl.lua` to see how it's accomplished.

## LuaEngine vs. LuaScript Functionality Comparison

Most LuaEngine functionality is handled by the data parsing engine. Refer to the example scripts for usage details. Only the differences in features are listed here:

### Lua Helper Functions

- LuaEngine no longer restarts the virtual machine, making it unnecessary to store and read variables. However, this functionality can still be used if cross-script access is needed.
- LuaEngine no longer provides a random number generation function; instead, Lua’s built-in random number generator can be used.

| Feature              | LuaEngine | LuaScript |
|----------------------|-----------|-----------|
| Random number retrieval | No        | Yes       |

### Lua System Functions

- Key check functionality includes keyboard and Xbox controller support.

| Feature                        | LuaEngine | LuaScript |
|---------------------------------|-----------|-----------|
| Check double-click              | No        | Yes       |
| Set sub-script variable names   | No        | Yes       |
| Retrieve network data           | Yes       | Yes       |
| Download files                  | Yes       | Yes       |
| Connect to websocket service    | Yes       | Yes       |
| Send websocket messages         | Yes       | Yes       |
| Retrieve websocket data         | Yes       | Yes       |
| Retrieve websocket status       | Yes       | Yes       |
| Script info input               | No        | Yes       |
| ImGui operations                | Yes       | No        |

### Memory Operation Functions

- There have been significant changes to the memory address retrieval function. Please refer to the example files for more details.

| Feature                   | LuaEngine | LuaScript |
|----------------------------|-----------|-----------|
| Retrieve memory address     | Changed   | Yes       |

### Game Functions

| Feature                         | LuaEngine | LuaScript |
|----------------------------------|-----------|-----------|
| Retrieve mod build version       | No        | Yes       |
| Retrieve mod release version     | No        | Yes       |
| Retrieve UUID                    | No        | Yes       |
| Console                          | No        | Yes       |
| Retrieve screen info             | No        | Yes       |
| External system operations       | No        | Yes       |

### Game Data

- Players have many buffs; manual retrieval and setting are required. The engine does not currently support these.
- Monster, endemic life, and entity operations require manual handling after retrieving the addresses.

| Feature                                     | LuaEngine | LuaScript |
|---------------------------------------------|-----------|-----------|
| Retrieve weapon coordinates                 | Partial   | Yes       |
| Retrieve navigation coordinates             | No        | Yes       |
| Camera-related functionality                | Partial   | Yes       |
| Player orientation information              | No        | Yes       |
| Derived action execution status             | No        | Yes       |
| Weapon ornament information                 | No        | Yes       |
| Main and secondary weapon operations        | No        | Yes       |
| Player hit information                      | Partial   | Yes       |
| Retrieve player character info              | No        | Yes       |
| Retrieve and set player buffs               | No        | Yes       |
| Retrieve and set player clutch claw coordinates | No    | Yes       |
| Retrieve monster aggro on player            | No        | Yes       |
| Palico data                                 | No        | Yes       |
| Retrieve Steam friend ID                    | No        | Yes       |
| Retrieve gathering hub code                 | No        | Yes       |
| Retrieve projectile data                    | No        | Yes       |
| Set the next quest                          | No        | Yes       |
| Monster-related data                        | Partial   | Yes       |
| Entity operations                           | Partial   | Yes       |
#pragma once
using namespace loader;

namespace LuaData {
    map<string, int> IntVariable;
    map<string, float> FloatVariable;
    map<string, string> StringVariable;
}

#pragma region LuaFun
//存入整数变量
static int Lua_Variable_SaveIntVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, 1);
    int variableValue = (int)lua_tointeger(pL, 2);
    LuaData::IntVariable[variableName] = variableValue;
    return 0;
}
//存入浮点数变量
static int Lua_Variable_SaveFloatVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, 1);
    float variableValue = (float)lua_tonumber(pL, 2);
    LuaData::FloatVariable[variableName] = variableValue;
    return 0;
}
//存入字符串变量
static int Lua_Variable_SaveStringVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, 1);
    string variableValue = (string)lua_tostring(pL, 2);
    LuaData::StringVariable[variableName] = variableValue;
    return 0;
}
//读取整数变量
static int Lua_Variable_ReadIntVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, -1);
    int ret;
    if (LuaData::IntVariable.find(variableName) == LuaData::IntVariable.end())
        ret = 0;
    else
        ret = LuaData::IntVariable[variableName];
    lua_pushinteger(pL, ret);
    return 1;
}
//读取浮点数变量
static int Lua_Variable_ReadFloatVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, -1);
    float ret;
    if (LuaData::FloatVariable.find(variableName) == LuaData::FloatVariable.end())
        ret = 0;
    else
        ret = LuaData::FloatVariable[variableName];
    lua_pushnumber(pL, ret);
    return 1;
}
//读取字符串变量
static int Lua_Variable_ReadStringVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, -1);
    string ret;
    if (LuaData::StringVariable.find(variableName) == LuaData::StringVariable.end())
        ret = "";
    else
        ret = LuaData::StringVariable[variableName];
    lua_pushstring(pL, ret.c_str());
    return 1;
}
//销毁变量
static int Lua_Variable_DestroyVariable(lua_State* pL) {
    string variableTpye = (string)lua_tostring(pL, 1);
    string variableName = (string)lua_tostring(pL, 2);
    if (variableTpye == "Int")
        LuaData::IntVariable.erase(variableName);
    else if (variableTpye == "Float")
        LuaData::FloatVariable.erase(variableName);
    else if (variableTpye == "String")
        LuaData::StringVariable.erase(variableName);
    return 0;
}
//获取随机数
static int Lua_Math_Rander(lua_State* pL) {
    float min = (float)lua_tonumber(pL, 1);
    float max = (float)lua_tonumber(pL, 2);
    lua_pushnumber(pL, utils::GetRandom(min, max));
    return 1;
}
#pragma endregion
#pragma region SystemFun
static int System_Keyboard_CheckKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, Keyboard::CheckKey(key));
    return 1;
}
static int System_Keyboard_CheckDoubleKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, Keyboard::CheckKey(key, 2));
    return 1;
}
static int System_Keyboard_CheckKeyIsPressed(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    bool ret = GetKeyState(key) < 0;
    if (!Keyboard::CheckWindows())
        ret = false;
    lua_pushboolean(pL, ret);
    return 1;
}
static int System_Chronoscope_AddChronoscope(lua_State* pL) {
    float time = (float)lua_tonumber(pL, 1);
    string name = (string)lua_tostring(pL, 2);
    Chronoscope::AddChronoscope(time, name, true);
    return 0;
}
static int System_Chronoscope_CheckPresenceChronoscope(lua_State* pL) {
    string name = (string)lua_tostring(pL, -1);
    lua_pushboolean(pL, Chronoscope::CheckPresenceChronoscope(name));
    return 1;
}
static int System_Chronoscope_DelChronoscope(lua_State* pL) {
    string name = (string)lua_tostring(pL, -1);
    Chronoscope::DelChronoscope(name);
    return 0;
}
static int System_Chronoscope_CheckChronoscope(lua_State* pL) {
    string name = (string)lua_tostring(pL, -1);
    lua_pushboolean(pL, Chronoscope::CheckChronoscope(name));
    return 1;
}
static int System_Message_ShowMessage(lua_State* pL) {
    string message = (string)lua_tostring(pL, -1);
    MH::Chat::ShowGameMessage(*(undefined**)MH::Chat::MainPtr, (undefined*)&message[0], -1, -1, 0);
    return 0;
}
static int System_Console_Info(lua_State* pL) {
    string message = (string)lua_tostring(pL, -1);
    LOG(INFO) << "LUA INFO:" << message;
    return 0;
}
static int System_Console_Error(lua_State* pL) {
    string message = (string)lua_tostring(pL, -1);
    LOG(ERR) << "LUA ERR:" << message;
    return 0;
}
static int System_XboxPad_CheckKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, XboxPad::CheckKey(key));
    return 1;
}
static int System_XboxPad_CheckDoubleKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, XboxPad::CheckKey(key, 2));
    return 1;
}
static int System_XboxPad_CheckKeyIsPressed(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    bool ret = XboxPad::KeyIdHandle(key);
    lua_pushboolean(pL, ret);
    return 1;
}
#pragma endregion
#pragma region MemoryFun
static int System_Memory_GetAddress(lua_State* pL) {
    vector<int> bytes;
    long long ptr = (long long)lua_tonumber(pL, 1);
    lua_pushnil(pL);
    while (lua_next(pL, 2) != 0)
    {
        bytes.push_back((int)lua_tointeger(pL, -1));
        lua_pop(pL, 1);
    }
    void* address = utils::GetPlot((void*)ptr, bytes);
    long long addr = (long long)address;
    lua_pushinteger(pL, addr);
    return 1;
}
static int System_Memory_GetAddressData(lua_State* pL) {
    long long ptr = (long long)lua_tonumber(pL, 1);
    string type = (string)lua_tostring(pL, 2);
    void* address = (void*)ptr;
    if (address != nullptr) {
        if (type == "int")
            lua_pushinteger(pL, *(int*)(ptr));
        else if (type == "float")
            lua_pushnumber(pL, *(float*)(ptr));
        else if (type == "bool")
            lua_pushboolean(pL, *(bool*)(ptr));
        else if (type == "byte")
            lua_pushinteger(pL, *(char*)(ptr));
        else
            lua_pushinteger(pL, *(char*)(ptr));
    }
    else
        lua_pushstring(pL, "Address error");
    return 1;
}
static int System_Memory_SetAddressData(lua_State* pL) {
    long long ptr = (long long)lua_tonumber(pL, 1);
    string type = (string)lua_tostring(pL, 2);
    void* address = (void*)ptr;
    if (address != nullptr) {
        if (type == "int") {
            *(int*)(ptr) = (int)lua_tointeger(pL, 3);
            lua_pushboolean(pL, true);
        }
        else if (type == "float") {
            *(float*)(ptr) = (float)lua_tonumber(pL, 3);
            lua_pushboolean(pL, true);
        }
        else if (type == "bool") {
            *(bool*)(ptr) = (bool)lua_toboolean(pL, 3);
            lua_pushboolean(pL, true);
        }
        else if (type == "byte") {
            *(char*)(ptr) = (char)lua_tointeger(pL, 3);
            lua_pushboolean(pL, true);
        }
        else
            lua_pushboolean(pL, false);
    }
    else
        lua_pushboolean(pL, false);
    return 1;
}
#pragma endregion
static void registerFunc(lua_State* L) {
#pragma region LuaFun
	//存入整数变量
	lua_register(L, "Lua_Variable_SaveIntVariable", Lua_Variable_SaveIntVariable);
	//存入浮点数变量
	lua_register(L, "Lua_Variable_SaveFloatVariable", Lua_Variable_SaveFloatVariable);
	//存入字符串变量
	lua_register(L, "Lua_Variable_SaveStringVariable", Lua_Variable_SaveStringVariable);
	//读取整数变量
	lua_register(L, "Lua_Variable_ReadIntVariable", Lua_Variable_ReadIntVariable);
	//读取浮点数变量
	lua_register(L, "Lua_Variable_ReadFloatVariable", Lua_Variable_ReadFloatVariable);
	//读取字符串变量
	lua_register(L, "Lua_Variable_ReadStringVariable", Lua_Variable_ReadStringVariable);
	//销毁变量
	lua_register(L, "Lua_Variable_DestroyVariable", Lua_Variable_DestroyVariable);
	//获取随机数(可能用不上，lua5.4重做了随机数生成器)
	lua_register(L, "Lua_Math_Rander", Lua_Math_Rander);
#pragma endregion
#pragma region System
    //检查按键
    lua_register(L, "System_Keyboard_CheckKey", System_Keyboard_CheckKey);
    //检查按键双击
    lua_register(L, "System_Keyboard_CheckDoubleKey", System_Keyboard_CheckDoubleKey);
    //检查按键是否处于按下状态
    lua_register(L, "System_Keyboard_CheckKeyIsPressed", System_Keyboard_CheckKeyIsPressed);
    //检查Xbox按键
    lua_register(L, "System_XboxPad_CheckKey", System_XboxPad_CheckKey);
    //检查Xbox按键双击
    lua_register(L, "System_XboxPad_CheckDoubleKey", System_XboxPad_CheckDoubleKey);
    //检查Xbox按键是否处于按下状态
    lua_register(L, "System_XboxPad_CheckKeyIsPressed", System_XboxPad_CheckKeyIsPressed);
    //添加计时器
    lua_register(L, "System_Chronoscope_AddChronoscope", System_Chronoscope_AddChronoscope);
    //检查计时器
    lua_register(L, "System_Chronoscope_CheckChronoscope", System_Chronoscope_CheckChronoscope);
    //检查计时器是否存在
    lua_register(L, "System_Chronoscope_CheckPresenceChronoscope", System_Chronoscope_CheckPresenceChronoscope);
    //删除计时器
    lua_register(L, "System_Chronoscope_DelChronoscope", System_Chronoscope_DelChronoscope);
    //向游戏内发送消息
    lua_register(L, "System_Message_ShowMessage", System_Message_ShowMessage);
    //向控制台发送消息
    lua_register(L, "System_Console_Info", System_Console_Info);
    //向控制台发送错误消息
    lua_register(L, "System_Console_Error", System_Console_Error);
#pragma endregion
#pragma region Memory
    //获取内存地址
    lua_register(L, "System_Memory_GetAddress", System_Memory_GetAddress);
    //获取内存地址数据
    lua_register(L, "System_Memory_GetAddressData", System_Memory_GetAddressData);
    //修改内存地址数据
    lua_register(L, "System_Memory_SetAddressData", System_Memory_SetAddressData);
#pragma endregion
}
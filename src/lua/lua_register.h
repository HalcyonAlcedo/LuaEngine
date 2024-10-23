#pragma once
#include "md5.h"
#include "lua_core_ffi.h"
#include "CircularBufferLogger.h"

using namespace loader;

namespace LuaData {
	map<string, int> IntVariable;
	map<string, float> FloatVariable;
	map<string, string> StringVariable;
}

typedef void (*StatesProcessor) (lua_State* L);

namespace LuaExternalData {
	std::vector<StatesProcessor> AllStatesProcessor;
	map<const char*, lua_CFunction> StatelessFunctions;
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
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	int argType = lua_type(pL, 1);
	std::string message;
	if (argType == LUA_TSTRING) {
		message = lua_tostring(pL, 1);
	}
	else if (argType == LUA_TNUMBER) {
		lua_Number num = lua_tonumber(pL, 1);
		message = std::to_string(num);
	}
	else if (argType == LUA_TTABLE) {
		lua_pushnil(pL);
		while (lua_next(pL, 1) != 0) {
			if (lua_isstring(pL, -1) || lua_isnumber(pL, -1)) {
				message += lua_tostring(pL, -1);
			}
			else {
				lua_pop(pL, 2);
				return luaL_error(pL, "Table contains non-string or non-number values");
			}
			lua_pop(pL, 1);
		}
	}
	else {
		std::vector<CustomDataEntry> customData = {};
		LuaCore::logger.logOperation(script, "System_Message_ShowMessage", MsgLevel::LogError, "向游戏内发送消息失败，参数必须是字符串、数字或字符串数组", customData);
		return luaL_error(pL, "Argument must be a string, number, or table of strings/numbers");
	}
	std::vector<CustomDataEntry> customData = {
		{"message", message},
	};
	LuaCore::logger.logOperation(script, "System_Message_ShowMessage", MsgLevel::INFO, "向游戏内发送消息", customData);
	MH::Chat::ShowGameMessage(*(undefined**)MH::Chat::MainPtr, (undefined*)&message[0], -1, -1, 0);
	return 0;
}
static int System_SendChatMessage(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	int argType = lua_type(pL, 1);
	std::string msg;
	if (argType == LUA_TSTRING) {
		msg = lua_tostring(pL, 1);
	}
	else if (argType == LUA_TNUMBER) {
		lua_Number num = lua_tonumber(pL, 1);
		msg = std::to_string(num);
	}
	else if (argType == LUA_TTABLE) {
		lua_pushnil(pL);
		while (lua_next(pL, 1) != 0) {
			if (lua_isstring(pL, -1) || lua_isnumber(pL, -1)) {
				msg += lua_tostring(pL, -1);
			}
			else {
				lua_pop(pL, 2);
				return luaL_error(pL, "Table contains non-string or non-number values");
			}
			lua_pop(pL, 1);
		}
	}
	else {
		std::vector<CustomDataEntry> customData = {};
		LuaCore::logger.logOperation(script, "System_SendChatMessage", MsgLevel::LogError, "发送聊天消息失败，参数必须是字符串、数字或字符串数组", customData);
		return luaL_error(pL, "Argument must be a string, number, or table of strings/numbers");
	}
	char buffer[256] = {};
	std::vector<CustomDataEntry> customData = {
		{"message", msg},
	};
	LuaCore::logger.logOperation(script, "System_SendChatMessage", MsgLevel::INFO, "发送聊天消息", customData);
	strncpy_s(buffer, sizeof(buffer), msg.c_str(), _TRUNCATE);
	Chat::SendChatMessage(buffer);
	return 0;
}
static int System_Console_Info(lua_State* pL) {
	int argType = lua_type(pL, 1);
	std::string message;
	if (argType == LUA_TSTRING) {
		message = lua_tostring(pL, 1);
	}
	else if (argType == LUA_TNUMBER) {
		lua_Number num = lua_tonumber(pL, 1);
		message = std::to_string(num);
	}
	else if (argType == LUA_TTABLE) {
		lua_pushnil(pL);
		while (lua_next(pL, 1) != 0) {
			if (lua_isstring(pL, -1) || lua_isnumber(pL, -1)) {
				message += lua_tostring(pL, -1);
			}
			else {
				lua_pop(pL, 2);
				return luaL_error(pL, "Table contains non-string or non-number values");
			}
			lua_pop(pL, 1);
		}
	}
	else {
		return luaL_error(pL, "Argument must be a string, number, or table of strings/numbers");
	}
	LOG(INFO) << utils::UTF8_To_string(message);
	return 0;
}
static int System_Console_Error(lua_State* pL) {
	int argType = lua_type(pL, 1);
	std::string message;
	if (argType == LUA_TSTRING) {
		message = lua_tostring(pL, 1);
	}
	else if (argType == LUA_TNUMBER) {
		lua_Number num = lua_tonumber(pL, 1);
		message = std::to_string(num);
	}
	else if (argType == LUA_TTABLE) {
		lua_pushnil(pL);
		while (lua_next(pL, 1) != 0) {
			if (lua_isstring(pL, -1) || lua_isnumber(pL, -1)) {
				message += lua_tostring(pL, -1);
			}
			else {
				lua_pop(pL, 2);
				return luaL_error(pL, "Table contains non-string or non-number values");
			}
			lua_pop(pL, 1);
		}
	}
	else {
		return luaL_error(pL, "Argument must be a string, number, or table of strings/numbers");
	}
	LOG(ERR) << utils::UTF8_To_string(message);
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
static int System_GetFileMD5(lua_State* pL)
{
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	string file = (string)lua_tostring(pL, 1);
	std::vector<CustomDataEntry> customData = {
		{"file", file},
	};
	LuaCore::logger.logOperation(script, "System_GetFileMD5", MsgLevel::INFO, "获取文件MD5?", customData);
	ifstream in(file.c_str(), ios::binary);
	if (!in) {
		lua_pushstring(pL, "");
	}
	else {
		MD5 md5;
		std::streamsize length;
		char buffer[1024];
		while (!in.eof()) {
			in.read(buffer, 1024);
			length = in.gcount();
			if (length > 0)
				md5.update(buffer, length);
		}
		in.close();
		lua_pushstring(pL, md5.toString().c_str());
	}
	return 1;
}

#pragma endregion
#pragma region MemoryFun
static int System_Memory_GetAddress(lua_State* pL) {
	vector<int> bytes;
	uintptr_t ptr = (uintptr_t)lua_tointeger(pL, 1);

	if (ptr == 0 || !utils::IsMemoryReadable((void*)ptr, sizeof(ptr))) {
		lua_pushboolean(pL, false);
		return 1;
	}

	lua_pushnil(pL);
	while (lua_next(pL, 2) != 0) {
		uintptr_t offset = (uintptr_t)lua_tointeger(pL, -1);
		bytes.push_back(offset);
		lua_pop(pL, 1);
	}

	void* address = utils::GetPlot((void*)ptr, bytes);

	if (address != nullptr) {
		uintptr_t addr = (uintptr_t)address;
		lua_pushinteger(pL, addr);
	}
	else {
		lua_pushboolean(pL, false);
	}
	return 1;
}
static int System_Memory_GetAddressData(lua_State* pL) {
	uintptr_t ptr = (uintptr_t)lua_tointeger(pL, 1);
	string type = (string)lua_tostring(pL, 2);

	if (ptr == 0 || !utils::IsMemoryReadable((void*)ptr, sizeof(ptr))) {
		lua_pushboolean(pL, false);
		return 1;
	}

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
		else if (type == "string") {
			string memory_string = (char*)(ptr);
			lua_pushstring(pL, memory_string.c_str());
		}
		else
			lua_pushinteger(pL, *(char*)(ptr));
	}
	else {
		lua_pushboolean(pL, false);
	}
	return 1;
}
static int System_Memory_SetAddressData(lua_State* pL) {
	uintptr_t ptr = (uintptr_t)lua_tointeger(pL, 1);
	string type = (string)lua_tostring(pL, 2);

	if (ptr == 0 || !utils::IsMemoryReadable((void*)ptr, sizeof(ptr))) {
		lua_pushboolean(pL, false);
		return 1;
	}

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
		else {
			lua_pushboolean(pL, false);
		}
	}
	else {
		lua_pushboolean(pL, false);
	}
	return 1;
}
static int System_Memory_SearchPattern(lua_State* pL) {
	std::vector<std::pair<BYTE, bool>> pattern;

	if (!lua_istable(pL, 1)) {
		lua_pushboolean(pL, false); // 参数不是表时返回 false
		return 1;
	}

	lua_pushnil(pL); // 先将 nil 压栈，作为 table 的初始键
	while (lua_next(pL, 1)) {
		if (lua_isnumber(pL, -1)) {  // 确保 Lua 表中的值为数字
			int value = lua_tointeger(pL, -1);
			pattern.push_back({ static_cast<BYTE>(value), false }); // 处理字节码
		}
		else if (lua_isstring(pL, -1)) {  // 处理通配符 ?? 的情况
			std::string value = lua_tostring(pL, -1);
			if (value == "??") {
				pattern.push_back({ 0x00, true }); // true 表示通配符
			}
		}
		lua_pop(pL, 1); // 弹出值，保留键进行下一次迭代
	}

	// 调用 C++ SearchPattern 函数
	void* foundAddress = utils::SearchPattern(pattern);

	if (foundAddress) {
		lua_pushinteger(pL, reinterpret_cast<ptrdiff_t>(foundAddress)); // 找到时返回地址
	}
	else {
		lua_pushboolean(pL, false); // 未找到时返回 false
	}
	return 1;
}
static int System_Memory_GetAddress_Safe(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	vector<int> bytes;
	uintptr_t ptr = (uintptr_t)lua_tointeger(pL, 1);

	if (ptr == 0 || !utils::IsMemoryReadable((void*)ptr, sizeof(ptr))) {
		lua_pushboolean(pL, false);
		return 1;
	}

	std::stringstream stamemoryAddr;
	stamemoryAddr << "0x" << std::hex << reinterpret_cast<uintptr_t>((void*)ptr);
	lua_pushnil(pL);

	std::vector<CustomDataEntry> customData = {};
	customData.push_back({ "address",stamemoryAddr.str() });

	while (lua_next(pL, 2) != 0) {
		uintptr_t offset = (uintptr_t)lua_tointeger(pL, -1);
		bytes.push_back(offset);
		std::stringstream memoryAddr;
		memoryAddr << "0x" << std::hex << reinterpret_cast<uintptr_t>((void*)offset);
		customData.push_back({ "address",memoryAddr.str() });
		lua_pop(pL, 1);
	}

	void* address = utils::GetPlot((void*)ptr, bytes);

	std::stringstream memoryAddr;
	memoryAddr << "0x" << std::hex << reinterpret_cast<uintptr_t>(address);
	customData.push_back({ "back address",memoryAddr.str() });
	LuaCore::logger.logOperation(script, "System_Memory_GetAddress", MsgLevel::INFO, "获取内存地址", customData);

	if (address != nullptr) {
		uintptr_t addr = (uintptr_t)address;
		lua_pushinteger(pL, addr);
	}
	else {
		lua_pushboolean(pL, false);
	}
	return 1;
}
static int System_Memory_GetAddressData_Safe(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	uintptr_t ptr = (uintptr_t)lua_tointeger(pL, 1);
	string type = (string)lua_tostring(pL, 2);

	if (ptr == 0 || !utils::IsMemoryReadable((void*)ptr, sizeof(ptr))) {
		lua_pushboolean(pL, false);
		return 1;
	}

	void* address = (void*)ptr;

	std::stringstream memoryAddr;
	memoryAddr << "0x" << std::hex << reinterpret_cast<uintptr_t>(address);
	std::vector<CustomDataEntry> customData = {
		{"address", memoryAddr.str()},
		{"value", utils::readHexValueAtAddress(address)}
	};
	LuaCore::logger.logOperation(script, "System_Memory_GetAddressData", MsgLevel::INFO, "读取内存数据", customData);

	if (address != nullptr) {
		if (type == "int")
			lua_pushinteger(pL, *(int*)(ptr));
		else if (type == "float")
			lua_pushnumber(pL, *(float*)(ptr));
		else if (type == "bool")
			lua_pushboolean(pL, *(bool*)(ptr));
		else if (type == "byte")
			lua_pushinteger(pL, *(char*)(ptr));
		else if (type == "string") {
			string memory_string = (char*)(ptr);
			lua_pushstring(pL, memory_string.c_str());
		}
		else
			lua_pushinteger(pL, *(char*)(ptr));
	}
	else {
		lua_pushboolean(pL, false);
	}
	return 1;
}
static int System_Memory_SetAddressData_Safe(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	uintptr_t ptr = (uintptr_t)lua_tointeger(pL, 1);
	string type = (string)lua_tostring(pL, 2);

	if (ptr == 0 || !utils::IsMemoryReadable((void*)ptr, sizeof(ptr))) {
		lua_pushboolean(pL, false);
		return 1;
	}

	void* address = (void*)ptr;

	std::stringstream memoryAddr;
	memoryAddr << "0x" << std::hex << reinterpret_cast<uintptr_t>(address);
	double number = lua_tointeger(pL, 3);
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(16)
		<< std::hex << static_cast<unsigned long long>(number);
	std::vector<CustomDataEntry> customData = {
		{ "address", memoryAddr.str() },
		{ "value", ss.str()}
	};
	LuaCore::logger.logOperation(script, "System_Memory_SetAddressData", MsgLevel::INFO, "写入内存数据", customData);

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
		else {
			lua_pushboolean(pL, false);
		}
	}
	else {
		lua_pushboolean(pL, false);
	}
	return 1;
}
static int System_Memory_SearchPattern_Safe(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	std::vector<std::pair<BYTE, bool>> pattern;

	if (!lua_istable(pL, 1)) {
		lua_pushboolean(pL, false); // 参数不是表时返回 false
		return 1;
	}

	std::vector<CustomDataEntry> customData = {};

	lua_pushnil(pL); // 先将 nil 压栈，作为 table 的初始键
	while (lua_next(pL, 1)) {
		if (lua_isnumber(pL, -1)) {  // 确保 Lua 表中的值为数字
			int value = lua_tointeger(pL, -1);
			pattern.push_back({ static_cast<BYTE>(value), false }); // 处理字节码
			std::stringstream memoryAddr;
			memoryAddr << "0x" << std::hex << reinterpret_cast<uintptr_t>((void*)value);
			customData.push_back({ "address",memoryAddr.str() });
		}
		else if (lua_isstring(pL, -1)) {  // 处理通配符 ?? 的情况
			std::string value = lua_tostring(pL, -1);
			if (value == "??") {
				pattern.push_back({ 0x00, true }); // true 表示通配符
				customData.push_back({ "address","??" });
			}
		}
		lua_pop(pL, 1); // 弹出值，保留键进行下一次迭代
	}

	// 调用 C++ SearchPattern 函数
	void* foundAddress = utils::SearchPattern(pattern);

	if (foundAddress) {
		lua_pushinteger(pL, reinterpret_cast<ptrdiff_t>(foundAddress)); // 找到时返回地址
		std::stringstream memoryAddr;
		memoryAddr << "0x" << std::hex << reinterpret_cast<uintptr_t>(foundAddress);
		customData.push_back({ "address",memoryAddr.str() });
	}
	else {
		customData.push_back({ "back address","??" });
		lua_pushboolean(pL, false); // 未找到时返回 false
	}

	LuaCore::logger.logOperation(script, "System_Memory_GetAddress", MsgLevel::INFO, "搜索内存地址", customData);

	return 1;
}

#pragma endregion
#pragma region GameFun
//添加特效
static int Game_Player_AddEffect(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	int group = (int)lua_tointeger(pL, 1);
	int record = (int)lua_tointeger(pL, 2);
	uintptr_t effects = (uintptr_t)lua_tointeger(pL, 3);
	void* Effects = nullptr;

	std::vector<CustomDataEntry> customData = {
		{"group", std::to_string(group)},
		{"record", std::to_string(record)}
	};
	LuaCore::logger.logOperation(script, "Game_Player_AddEffect", MsgLevel::INFO, "添加特效", customData);

	if (effects) {
		Effects = (void*)effects;
	}
	else {
		void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
		PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
		Effects = *offsetPtr<void*>(PlayerPlot, 0x8808);
	}
	MH::Player::Effects((undefined*)Effects, group, record);
	return 0;
}
//执行Fsm动作
static int Game_Player_RunFsmAction(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	int type = (int)lua_tointeger(pL, 1);
	int id = (int)lua_tointeger(pL, 2);

	std::vector<CustomDataEntry> customData = {
		{"type", std::to_string(type)},
		{"id", std::to_string(id)}
	};
	LuaCore::logger.logOperation(script, "Game_Player_RunFsmAction", MsgLevel::INFO, "执行Fsm动作", customData);

	void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
	PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
	*offsetPtr<int>(PlayerPlot, 0x6284) = type;
	*offsetPtr<int>(PlayerPlot, 0x6288) = id;
	*offsetPtr<int>(PlayerPlot, 0x628C) = type;
	*offsetPtr<int>(PlayerPlot, 0x6290) = id;
	return 0;
}
//执行Lmt动作
static int Game_Player_RunLmtAction(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	int id = (int)lua_tointeger(pL, -1);

	std::vector<CustomDataEntry> customData = {
	{"id", std::to_string(id)}
	};
	LuaCore::logger.logOperation(script, "Game_Player_RunLmtAction", MsgLevel::INFO, "执行Lmt动作", customData);

	void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
	PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
	MH::Player::CallLmt((undefined*)PlayerPlot, id, 0);
	return 0;
}
//切换武器
static int Game_Player_Weapon_ChangeWeapons(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	void* PlayerCountPlot = *(undefined**)MH::World::PlayerCount;
	PlayerCountPlot = *offsetPtr<undefined**>((undefined(*)())PlayerCountPlot, 0x258);
	PlayerCountPlot = *offsetPtr<undefined**>((undefined(*)())PlayerCountPlot, 0x10);
	int PlayerCount = *offsetPtr<int>(PlayerCountPlot, 0x6574);
	if (PlayerCount > 1)
	{
		return 0;
	}
	int type = (int)lua_tointeger(pL, 1);
	int id = (int)lua_tointeger(pL, 2);
	std::vector<CustomDataEntry> customData = {
		{ "type", std::to_string(type) },
		{ "id", std::to_string(id) }
	};
	LuaCore::logger.logOperation(script, "Game_Player_Weapon_ChangeWeapons", MsgLevel::INFO, "切换武器", customData);
	if (type <= 13 and type >= 0 and id >= 0) {
		void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
		PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
		if (lua_gettop(pL) > 3) {
			*offsetPtr<int>(PlayerPlot, 0x13860) = type;
			*offsetPtr<int>(PlayerPlot, 0x13864) = id;
			MH::Weapon::CompleteChangeWeapon(PlayerPlot, 1, 0);
		}
		else
			MH::Weapon::ChangeWeapon(PlayerPlot, type, id);
	}
	return 0;
}
//临时刷新装备
static int Game_Player_RefreshEquip(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	void* PlayerCountPlot = *(undefined**)MH::World::PlayerCount;
	PlayerCountPlot = *offsetPtr<undefined**>((undefined(*)())PlayerCountPlot, 0x258);
	PlayerCountPlot = *offsetPtr<undefined**>((undefined(*)())PlayerCountPlot, 0x10);
	int PlayerCount = *offsetPtr<int>(PlayerCountPlot, 0x6574);
	if (PlayerCount > 1)
	{
		return 0;
	}
	std::vector<CustomDataEntry> customData = {};
	LuaCore::logger.logOperation(script, "Game_Player_RefreshEquip", MsgLevel::INFO, "临时刷新装备", customData);
	void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
	PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
	PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x12610);
	MH::Weapon::RefreshEquip(PlayerPlot);
	return 0;
}
//发射投射物
struct Vector3 {
	float x, y, z;
	Vector3(float x = 0, float y = 0, float z = 0) :x(x), y(y), z(z) { };
};
//执行投射物生成
static bool CallProjectilesGenerate(int Id, float* Coordinate, void* ShlpList = nullptr, void* FromPtr = nullptr) {
	void* ShlpRoute = MH::Shlp::GetShlp(ShlpList, Id);
	if (ShlpRoute == nullptr)
		return false;
	ShlpRoute = *offsetPtr<void*>(ShlpRoute, 0x8);
	MH::Shlp::CallShlp(ShlpRoute, FromPtr, FromPtr, Coordinate);
	return true;
}
//处理投射物路径数据
static void GenerateProjectilesCoordinateData(float*& CalculationCoordinates, Vector3 startPoint, Vector3 endPoint) {
	//缓存指针
	float* temp_float = CalculationCoordinates;
	//写入起始坐标
	*temp_float = startPoint.x;
	temp_float++;
	*temp_float = startPoint.y;
	temp_float++;
	*temp_float = startPoint.z;
	temp_float++;
	//起始坐标写入完成，空4个字节
	*temp_float = 0;
	temp_float++;
	//更换指针为单字节并写入1
	unsigned char* temp_byte = (unsigned char*)temp_float;
	*temp_byte = 1;

	//重设缓存指针至坐标地址40处
	temp_float = offsetPtr<float>(CalculationCoordinates, 0x40);
	//写入结束坐标
	*temp_float = endPoint.x;
	temp_float++;
	*temp_float = endPoint.y;
	temp_float++;
	*temp_float = endPoint.z;
	temp_float++;
	//结束坐标写入完成，空4个字节
	*temp_float = 0;
	temp_float++;
	//更换指针为单字节并写入1
	temp_byte = (unsigned char*)temp_float;
	*temp_byte = 1;

	//重设缓存指针至坐标地址A0处
	int* tempCoordinateTailData = offsetPtr<int>(CalculationCoordinates, 0xA0);
	//写入坐标数据尾部信息
	*tempCoordinateTailData = 0x12;
	tempCoordinateTailData++;
	longlong* tempCoordinateTailData_longlong = (longlong*)tempCoordinateTailData;
	*tempCoordinateTailData_longlong = -1;
}
//生成投射物
static bool CreateProjectiles(int Id, Vector3 startPoint, Vector3 endPoint, void* ShlpList = nullptr, void* FromPtr = nullptr) {
	//创建投射物路径数据缓存指针
	float* CoordinatesData = new float[73];
	//填充缓存区数据
	memset(CoordinatesData, 0, 73 * 4);
	//处理投射物路径数据
	GenerateProjectilesCoordinateData(CoordinatesData, startPoint, endPoint);
	//执行生成投射物
	bool GenerateResults = CallProjectilesGenerate(Id, CoordinatesData, ShlpList, FromPtr);
	//清理缓冲区
	delete[]CoordinatesData;
	return GenerateResults;
}
static int Game_Player_CreateProjectiles(lua_State* pL) {
	lua_getglobal(pL, "reserv_Script");
	const char* script = lua_tostring(pL, -1);
	lua_pop(pL, 1);

	int id = (int)lua_tointeger(pL, 1);
	float startx = (float)lua_tonumber(pL, 2);
	float starty = (float)lua_tonumber(pL, 3);
	float startz = (float)lua_tonumber(pL, 4);
	float endx = (float)lua_tonumber(pL, 5);
	float endy = (float)lua_tonumber(pL, 6);
	float endz = (float)lua_tonumber(pL, 7);

	std::vector<CustomDataEntry> customData = {
	{ "id", std::to_string(id) },
	{ "startx", std::to_string(startx) },
	{ "starty", std::to_string(starty) },
	{ "startz", std::to_string(startz) },
	{ "endx", std::to_string(endx) },
	{ "endy", std::to_string(endy) },
	{ "endz", std::to_string(endz) }
	};
	LuaCore::logger.logOperation(script, "Game_Player_CreateProjectiles", MsgLevel::INFO, "生成投射物", customData);

	uintptr_t entity = (uintptr_t)lua_tointeger(pL, 8);
	uintptr_t shlpList = (uintptr_t)lua_tointeger(pL, 9);
	void* EntityAddress = (void*)entity;
	void* ShlpListAddress = (void*)shlpList;
	if (EntityAddress != nullptr) {
		lua_pushboolean(pL, CreateProjectiles(
			id, Vector3(startx, starty, startz), Vector3(endx, endy, endz), ShlpListAddress, EntityAddress
		));
	}
	return 1;
}
//获取游戏版本
static int Game_Version(lua_State* pL) {
	lua_pushstring(pL, loader::GameVersion);
	return 1;
}
#pragma endregion

static void applyExternalFunc(lua_State* L) {
	for (auto& p : LuaExternalData::StatelessFunctions) {
		lua_register(L, p.first, p.second);
	}
}

static void applyExternalProcessor(lua_State* L) {
	for (auto& processor : LuaExternalData::AllStatesProcessor) {
		processor(L);
	}
}

static void applyExternalChange(lua_State* L) {
	applyExternalFunc(L);
	applyExternalProcessor(L);
}

static void registerFunc(lua_State* L, string script) {

	lua_pushstring(L, script.c_str());
	lua_setglobal(L, "reserv_Script");

#pragma region LuaFun
	//存入整数变量
	lua_register(L, "setGlobalVariable_int", Lua_Variable_SaveIntVariable);
	//存入浮点数变量
	lua_register(L, "setGlobalVariable_float", Lua_Variable_SaveFloatVariable);
	//存入字符串变量
	lua_register(L, "setGlobalVariable_string", Lua_Variable_SaveStringVariable);
	//读取整数变量
	lua_register(L, "GlobalVariable_int", Lua_Variable_ReadIntVariable);
	//读取浮点数变量
	lua_register(L, "GlobalVariable_float", Lua_Variable_ReadFloatVariable);
	//读取字符串变量
	lua_register(L, "GlobalVariable_string", Lua_Variable_ReadStringVariable);
	//销毁变量
	lua_register(L, "DestroyGlobalVariable", Lua_Variable_DestroyVariable);
#pragma endregion
#pragma region System
	//检查按键
	lua_register(L, "CheckKey", System_Keyboard_CheckKey);
	//检查按键是否处于按下状态
	lua_register(L, "CheckKeyIsPressed", System_Keyboard_CheckKeyIsPressed);
	//检查Xbox按键
	lua_register(L, "XCheckKey", System_XboxPad_CheckKey);
	//检查Xbox按键是否处于按下状态
	lua_register(L, "XCheckKeyIsPressed", System_XboxPad_CheckKeyIsPressed);
	//添加计时器
	lua_register(L, "AddChronoscope", System_Chronoscope_AddChronoscope);
	//检查计时器
	lua_register(L, "CheckChronoscope", System_Chronoscope_CheckChronoscope);
	//检查计时器是否存在
	lua_register(L, "CheckPresenceChronoscope", System_Chronoscope_CheckPresenceChronoscope);
	//删除计时器
	lua_register(L, "DelChronoscope", System_Chronoscope_DelChronoscope);
	//向游戏内发送消息
	lua_register(L, "Message", System_Message_ShowMessage);
	//发送聊天消息
	lua_register(L, "SendChatMessage", System_SendChatMessage);
	//向控制台发送消息
	lua_register(L, "Console_Info", System_Console_Info);
	//向控制台发送错误消息
	lua_register(L, "Console_Error", System_Console_Error);
	//获取文件Md5
	lua_register(L, "GetFileMD5", System_GetFileMD5);
#pragma endregion
#pragma region Memory
	//获取内存地址
	lua_register(L, "GetAddress", System_Memory_GetAddress);
	//获取内存地址数据
	lua_register(L, "GetAddressData", System_Memory_GetAddressData);
	//修改内存地址数据
	lua_register(L, "SetAddressData", System_Memory_SetAddressData);
	//搜索内存地址
	lua_register(L, "SearchPattern", System_Memory_SearchPattern);
	//获取内存地址(安全)
	lua_register(L, "SafeGetAddress", System_Memory_GetAddress_Safe);
	//获取内存地址数据(安全)
	lua_register(L, "SafeGetAddressData", System_Memory_GetAddressData_Safe);
	//修改内存地址数据(安全)
	lua_register(L, "SafeSetAddressData", System_Memory_SetAddressData_Safe);
	//搜索内存地址(安全)
	lua_register(L, "SafeSearchPattern", System_Memory_SearchPattern_Safe);
#pragma endregion
#pragma region Game
	//添加特效
	lua_register(L, "AddEffect", Game_Player_AddEffect);
	//执行Fsm动作(完全可通过修改内存实现，暂且先加上吧)
	lua_register(L, "RunFsmAction", Game_Player_RunFsmAction);
	//执行Lmt动作
	lua_register(L, "RunLmtAction", Game_Player_RunLmtAction);
	//更换玩家武器
	lua_register(L, "ChangeWeapons", Game_Player_Weapon_ChangeWeapons);
	//刷新装备数据
	lua_register(L, "RefreshEquip", Game_Player_RefreshEquip);
	//发射投射物
	lua_register(L, "CreateProjectiles", Game_Player_CreateProjectiles);
	//获取游戏版本
	lua_register(L, "GameVersion", Game_Version);
#pragma endregion
#pragma region External
	//加载外部来源
	applyExternalChange(L);
#pragma endregion
}
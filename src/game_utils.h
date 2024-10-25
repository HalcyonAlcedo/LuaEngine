#pragma once
#include <io.h>
#include <filesystem>
#include <tlhelp32.h>
#include <Windows.h>
#include <Psapi.h>

using namespace loader;
using namespace std;

#pragma region General tools
namespace utils {

	BYTE* startAddress = nullptr;
	BYTE* endAddress = nullptr;
	struct CacheEntry {
		bool readable;
		size_t size;
	};

	std::unordered_map<void*, CacheEntry> addressCache;

	// 获取 MonsterHunterWorld.exe 的内存范围
	bool GetMonsterHunterWorldModuleRange() {
		if (startAddress != nullptr && endAddress != nullptr)
		{
			return true;
		}

		HMODULE hMods[1024];
		DWORD cbNeeded;

		if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
			for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
				char szModName[MAX_PATH];
				GetModuleFileNameExA(GetCurrentProcess(), hMods[i], szModName, sizeof(szModName));
				if (strstr(szModName, "MonsterHunterWorld.exe")) { // 检查模块名
					MODULEINFO moduleInfo;
					if (GetModuleInformation(GetCurrentProcess(), hMods[i], &moduleInfo, sizeof(moduleInfo))) {
						startAddress = (BYTE*)moduleInfo.lpBaseOfDll;
						endAddress = startAddress + moduleInfo.SizeOfImage;
						return true; // 找到模块并获取范围
					}
				}
			}
		}
		return false; // 如果未找到，返回 false
	}

	//获取偏移地址
	static void* GetPlot(void* plot, const std::vector<int>& bytes) {
		void* Plot = plot;

		// 获取 MonsterHunterWorld.exe 的内存范围
		if (GetMonsterHunterWorldModuleRange() && (reinterpret_cast<BYTE*>(plot) >= startAddress && reinterpret_cast<BYTE*>(plot) < endAddress)) {
			Plot = *(undefined**)plot;
		}
		for (int i : bytes) {
			if (Plot != nullptr) {
				Plot = *offsetPtr<undefined**>((undefined(*)())Plot, i);
			}
			else {
				return nullptr;
			}
		}
		return Plot;
	}
	// 检查内存保护状态
	bool IsReadableMemory(PMEMORY_BASIC_INFORMATION mbi) {
		// 判断内存区域是否是可读的
		return (mbi->Protect & PAGE_READONLY) || (mbi->Protect & PAGE_READWRITE) ||
			(mbi->Protect & PAGE_EXECUTE_READ) || (mbi->Protect & PAGE_EXECUTE_READWRITE);
	}
	bool IsMemoryReadable(void* ptr, size_t size) {
		// 检查缓存
		/*
		auto it = addressCache.find(ptr);
		if (it != addressCache.end() && it->second.size == size) {
			return it->second.readable;
		}

		MEMORY_BASIC_INFORMATION mbi;
		if (VirtualQuery(ptr, &mbi, sizeof(mbi))) {
			bool readable = (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_READ)) != 0;
			bool inRange = size <= (mbi.RegionSize - ((uintptr_t)ptr - (uintptr_t)mbi.BaseAddress));

			// 更新缓存
			addressCache[ptr] = { readable && inRange, size };
			return readable && inRange;
		}
		*/
		__try {
			// 尝试访问内存
			volatile char temp;
			for (size_t i = 0; i < size; i++) {
				temp = *((volatile char*)ptr + i);  // 尝试读取每个字节
			}
			return true;  // 如果没有异常，内存可读
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return false;  // 如果捕获到访问异常，内存不可读
		}
		return false;
	}

	// 比较当前内存区域是否匹配特征码，支持通配符 ?? 代表任意字节
	bool CompareMemory(const BYTE* memory, const std::vector<std::pair<BYTE, bool>>& pattern) {
		for (size_t i = 0; i < pattern.size(); ++i) {
			// 如果是通配符，跳过比较
			if (pattern[i].second) {
				continue;
			}
			// 不是通配符的情况，进行字节比较
			if (memory[i] != pattern[i].first) {
				return false;
			}
		}
		return true;
	}

	// 搜索指定地址范围内的特征码
	void* SearchPattern(const std::vector<std::pair<BYTE, bool>>& pattern) {
		// 获取 MonsterHunterWorld.exe 的内存范围
		if (!GetMonsterHunterWorldModuleRange()) {
			std::cerr << "Failed to get MonsterHunterWorld.exe module range." << std::endl;
			return nullptr; // 如果未找到模块范围，返回 nullptr
		}

		BYTE* currentAddress = startAddress;

		while (currentAddress < endAddress) {
			MEMORY_BASIC_INFORMATION mbi;
			if (VirtualQuery(currentAddress, &mbi, sizeof(mbi)) == sizeof(mbi)) {
				if (IsReadableMemory(&mbi)) {
					BYTE* regionStart = (BYTE*)mbi.BaseAddress;
					BYTE* regionEnd = regionStart + mbi.RegionSize;

					if (regionEnd > endAddress) {
						regionEnd = endAddress;
					}

					for (BYTE* addr = regionStart; addr <= regionEnd - pattern.size(); ++addr) {
						if (CompareMemory(addr, pattern)) {
							return addr; // 找到特征码的位置，返回地址
						}
					}
				}
			}
			currentAddress += mbi.RegionSize;
		}
		return nullptr; // 如果未找到特征码，返回 nullptr
	}

	//获取随机数
	static float GetRandom(float min, float max)
	{
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_real_distribution<float> dist(min, max);
		return dist(eng);
	}
	//转utf8
	static string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}
	//转unicode
	std::string UTF8_To_string(const std::string& str)
	{
		int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		memset(pwBuf, 0, nwLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr = pBuf;
		delete[]pBuf;
		delete[]pwBuf;
		pBuf = NULL;
		pwBuf = NULL;
		return retStr;
	}
	//安全读取内存
	std::string readHexValueAtAddress(void* address) {
		// 确保指针非空
		if (address == nullptr) {
			return "00000000"; // 或者返回错误信息
		}

		// 使用union来避免未定义行为
		union {
			uint32_t intValue;
			char chars[4];
		} data;

		// 读取内存地址
		memcpy(&data.intValue, address, sizeof(uint32_t));

		// 转换为十六进制字符串
		std::stringstream ss;
		ss << std::hex << data.intValue;
		std::string hexValue = ss.str();

		// 补充到8个字符，确保格式一致
		if (hexValue.length() < 8) {
			hexValue = std::string(8 - hexValue.length(), '0') + hexValue;
		}

		return hexValue;
	}
}
#pragma endregion

#pragma region Timer
namespace Chronoscope {
	struct ChronoscopeData {
		float StartTime = 0;
		float EndTime = 0;
	};

	//现在的时间
	float NowTime = 0;
	//计时器列表
	map<string, ChronoscopeData> ChronoscopeList;
	//添加计时器(时长，计时器名称，是否覆盖)
	static bool AddChronoscope(float duration, string name, bool Overlay = false) {
		if (ChronoscopeList.find(name) == ChronoscopeList.end() || Overlay) {
			ChronoscopeList[name].EndTime = NowTime + duration;
			ChronoscopeList[name].StartTime = NowTime;
			return true;
		}
		else
			return false;
	}
	//删除计时器
	static void DelChronoscope(string name) {
		if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
			ChronoscopeList.erase(name);
		}
	}
	//检查计时器是否存在
	static bool CheckPresenceChronoscope(string name) {
		if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
			return true;
		}
		return false;
	}
	//检查计时器是否结束
	static bool CheckChronoscope(string name) {
		if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
			if (ChronoscopeList[name].EndTime < NowTime) {
				DelChronoscope(name);
				return true;
			}
			else
				return false;
		}
		return false;
	}
	//计时器更新程序
	static void chronoscope() {
		void* TimePlot = utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50, 0x7D20 });
		if(TimePlot != nullptr)
			NowTime = *offsetPtr<float>(TimePlot, 0xC24);
	}
}
#pragma endregion

#pragma region Key board
namespace Keyboard {
	namespace TempData {
		map<int, bool> t_KeyDown;
		map<int, int> t_KeyCount;
	}
	//检查窗口
	static bool CheckWindows() {
		// 获取前台窗口句柄
		HWND foreground_hwnd = GetForegroundWindow();
		// 窗口所在的进程id
		DWORD window_pid = NULL;
		GetWindowThreadProcessId(foreground_hwnd, &window_pid);
		if (window_pid == NULL) {
			// error ignored
			return false;
		}

		DWORD process_pid = GetCurrentProcessId();

		return process_pid == window_pid;
	}
	//按键检查
	static bool CheckKey(int vk, int ComboClick = 1, float Duration = 0.3) {
		if (!CheckWindows())
			return false;
		//建立按键档案
		if (TempData::t_KeyDown.find(vk) == TempData::t_KeyDown.end()) {
			TempData::t_KeyDown[vk] = false;
		}
		//按键检查
		if (GetKeyState(vk) < 0 and !TempData::t_KeyDown[vk]) {
			TempData::t_KeyDown[vk] = true;
			//连击检查
			if (TempData::t_KeyCount.find(vk) != TempData::t_KeyCount.end()) {
				//计时器检查
				if (TempData::t_KeyCount[vk] == 1)
					Chronoscope::AddChronoscope(Duration, "KEY_" + to_string(vk), true);
				if (Chronoscope::CheckChronoscope("KEY_" + to_string(vk))) {
					TempData::t_KeyCount[vk] = 0;
				}
				TempData::t_KeyCount[vk]++;
			}
			else {
				Chronoscope::AddChronoscope(Duration, "KEY_" + to_string(vk), true);
				TempData::t_KeyCount[vk] = 1;
			}

			//检查结果
			if (TempData::t_KeyCount[vk] == ComboClick)
				return true;
			else
				return false;
		}
		else if (GetKeyState(vk) >= 0)
			TempData::t_KeyDown[vk] = false;
		return false;
	}
}
#pragma endregion

#pragma region Xbox Pad
namespace XboxPad {
	namespace TempData {
		map<int, bool> t_KeyDown;
		map<int, int> t_KeyCount;
	}
	namespace KeyState {
		bool LJoystickUp = false;
		bool LJoystickRight = false;
		bool LJoystickDown = false;
		bool LJoystickLeft = false;
		bool LJoystickPress = false;
		bool RJoystickUp = false;
		bool RJoystickRight = false;
		bool RJoystickDown = false;
		bool RJoystickLeft = false;
		bool RJoystickPress = false;
		bool LT = false;
		bool RT = false;
		bool LB = false;
		bool RB = false;
		bool Up = false;
		bool Right = false;
		bool Down = false;
		bool Left = false;
		bool Y = false;
		bool B = false;
		bool A = false;
		bool X = false;
		bool Window = false;
		bool Menu = false;
	}
	static bool KeyIdHandle(int Id) {
		switch (Id)
		{
		case 0:
			return KeyState::LJoystickUp;
		case 1:
			return KeyState::LJoystickRight;
		case 2:
			return KeyState::LJoystickDown;
		case 3:
			return KeyState::LJoystickLeft;
		case 4:
			return KeyState::LJoystickPress;
		case 5:
			return KeyState::RJoystickUp;
		case 6:
			return KeyState::RJoystickRight;
		case 7:
			return KeyState::RJoystickDown;
		case 8:
			return KeyState::RJoystickLeft;
		case 9:
			return KeyState::RJoystickPress;
		case 10:
			return KeyState::LT;
		case 11:
			return KeyState::RT;
		case 12:
			return KeyState::LB;
		case 13:
			return KeyState::RB;
		case 14:
			return KeyState::Up;
		case 15:
			return KeyState::Right;
		case 16:
			return KeyState::Down;
		case 17:
			return KeyState::Left;
		case 18:
			return KeyState::Y;
		case 19:
			return KeyState::B;
		case 20:
			return KeyState::A;
		case 21:
			return KeyState::X;
		case 22:
			return KeyState::Window;
		case 23:
			return KeyState::Menu;
		default:
			return false;
		}
	}
	//按键检查
	static bool CheckKey(int Key, int ComboClick = 1, float Duration = 0.3) {
		//建立按键档案
		if (TempData::t_KeyDown.find(Key) == TempData::t_KeyDown.end()) {
			TempData::t_KeyDown[Key] = false;
		}
		//按键检查
		if (KeyIdHandle(Key) and !TempData::t_KeyDown[Key]) {
			TempData::t_KeyDown[Key] = true;
			//连击检查
			if (TempData::t_KeyCount.find(Key) != TempData::t_KeyCount.end()) {
				//计时器检查
				if (TempData::t_KeyCount[Key] == 1)
					Chronoscope::AddChronoscope(Duration, "XKEY_" + to_string(Key), true);
				if (Chronoscope::CheckChronoscope("XKEY_" + to_string(Key))) {
					TempData::t_KeyCount[Key] = 0;
				}
				TempData::t_KeyCount[Key]++;
			}
			else {
				Chronoscope::AddChronoscope(Duration, "XKEY_" + to_string(Key), true);
				TempData::t_KeyCount[Key] = 1;
			}

			//检查结果
			if (TempData::t_KeyCount[Key] == ComboClick)
				return true;
			else
				return false;
		}
		else
			TempData::t_KeyDown[Key] = false;
		return false;
	}
	static void Updata() {
		KeyState::LJoystickUp = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC44) > 0.0;
		KeyState::LJoystickRight = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC40) > 0.0;
		KeyState::LJoystickDown = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC44) < 0.0;
		KeyState::LJoystickLeft = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC40) < 0.0;
		KeyState::RJoystickUp = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC48) > 0.0;
		KeyState::RJoystickRight = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC4C) > 0.0;
		KeyState::RJoystickDown = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC48) < 0.0;
		KeyState::RJoystickLeft = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC4C) < 0.0;
		KeyState::LJoystickPress = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC64) != 0.0;
		KeyState::RJoystickPress = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC68) != 0.0;
		KeyState::LT = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC88) != 0.0;
		KeyState::RT = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC8C) != 0.0;
		KeyState::LB = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC80) != 0.0;
		KeyState::RB = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC84) != 0.0;
		KeyState::Up = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC70) != 0.0;
		KeyState::Right = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC74) != 0.0;
		KeyState::Down = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC78) != 0.0;
		KeyState::Left = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC7C) != 0.0;
		KeyState::Y = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC90) != 0.0;
		KeyState::B = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC94) != 0.0;
		KeyState::A = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC98) != 0.0;
		KeyState::X = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC9C) != 0.0;
		KeyState::Window = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC60) != 0.0;
		KeyState::Menu = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC6C) != 0.0;
	}
}
#pragma endregion

#pragma region GameChat
namespace Chat
{
	typedef struct uGUIChat
	{
		long long* vtable_ref;
		long long unkptrs[42]; // Most of them are pointers, but there's also some unk data there
		int chatIndex;
		int unk; // Probably either padding, or chatIndex is an uint64_t
		int isTextBarVisible;
		char space;
		char chatBuffer[256];
	};

	template <class T>
	T* resolvePtrs(long long* base, std::vector<int> offsets)
	{
		for (int offset : offsets)
			base = ((long long*)(*base + offset));

		return reinterpret_cast<T*>(base);
	}

	bool SendChatMessage(char message[256]) {
		uGUIChat* chat = resolvePtrs<uGUIChat>(MH::Chat::uGuiChatBase, { 0x13FD0, 0x28F8 });
		bool* sendMessage = resolvePtrs<bool>(MH::Chat::uGuiChatBase, { 0x13FD0, 0x325E });

		memcpy(chat->chatBuffer, message, 256);
		*sendMessage = true;

		return true;
	}
}
#pragma endregion